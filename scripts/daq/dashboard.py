#!/usr/bin/env python3
"""
Real-time CAN dashboard: candump -> DBC decode -> WebSocket -> browser (chart + table).

Runs alongside logger.py (separate candump processes). Same filtering as logger.py
(TARGET_IDS unless --all).

Usage:
    python dashboard.py vcan0
    python dashboard.py vcan0 --pt-interface vcan1 --port 8000
"""

from __future__ import annotations

import argparse
import asyncio
import json
import queue
import sys
import threading
from contextlib import asynccontextmanager
from pathlib import Path
from typing import Any

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import HTMLResponse
import uvicorn

# Reuse decode pipeline from logger (same directory)
from logger import (
    TARGET_IDS,
    _candump_reader,
    decode_frame,
    load_dbc,
    parse_candump_line,
)

# ---------------------------------------------------------------------------
# HTML + JS (Chart.js from CDN; no build step)
# ---------------------------------------------------------------------------
DASHBOARD_HTML = """<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>CAN live dashboard</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.1/dist/chart.umd.min.js"></script>
  <style>
    body { font-family: system-ui, sans-serif; margin: 1rem; background: #111; color: #eee; }
    h1 { font-size: 1.25rem; }
    #status { margin: 0.5rem 0; color: #8f8; }
    #status.disconnected { color: #f88; }
    #chartsWrap { display: flex; gap: 1rem; flex-wrap: wrap; margin: 1rem 0; }
    .chartBox { flex: 1 1 420px; min-width: 320px; }
    .chartBox h2 { font-size: 1rem; margin: 0 0 0.5rem 0; }
    table { border-collapse: collapse; width: 100%; max-width: 1100px; font-size: 0.85rem; }
    th, td { border: 1px solid #444; padding: 0.35rem 0.5rem; text-align: left; }
    th { background: #222; }
    tr:nth-child(even) { background: #1a1a1a; }
    .mono { font-family: ui-monospace, monospace; }
  </style>
</head>
<body>
  <h1>CAN — real time</h1>
  <p id="status" class="disconnected">Connecting…</p>
  <div id="chartsWrap">
    <div class="chartBox">
      <h2>Speed (DashCommand)</h2>
      <canvas id="chartSpeed"></canvas>
    </div>
    <div class="chartBox">
      <h2>Linear Pots (SuspensionTravel34)</h2>
      <canvas id="chartPots"></canvas>
    </div>
  </div>
  <h2>Latest values (all rows)</h2>
  <p><button type="button" id="clearBtn">Clear table</button></p>
  <table>
    <thead><tr><th>Time</th><th>ID</th><th>Message</th><th>Signal</th><th>Value</th><th>Unit</th></tr></thead>
    <tbody id="tbody"></tbody>
  </table>
  <script>
    const MAX_POINTS = 60;
    const SPEED_KEY = 'DashCommand :: Speed';
    const STP3_KEY = 'SuspensionTravel34 :: STP3';
    const STP4_KEY = 'SuspensionTravel34 :: STP4';

    const statusEl = document.getElementById('status');
    const tbody = document.getElementById('tbody');
    const clearBtn = document.getElementById('clearBtn');
    const wsProto = location.protocol === 'https:' ? 'wss:' : 'ws:';
    const ws = new WebSocket(wsProto + '//' + location.host + '/ws');

    const latestByKey = new Map();
    const allRows = [];
    const speedPoints = [];
    const stp3Points = [];
    const stp4Points = [];

    function keyFor(msg) {
      return msg.message + ' :: ' + msg.signal;
    }

    function epochToHms(t) {
      const d = new Date(t * 1000);
      return d.toTimeString().slice(0, 8) + '.' +
        String(d.getMilliseconds()).padStart(3, '0');
    }

    const chartOpts = {
      responsive: true,
      animation: false,
      scales: {
        x: {
          type: 'category',
          title: { display: true, text: 'time (HH:MM:SS.mmm)' },
          ticks: { maxRotation: 45, minRotation: 45, autoSkip: true, maxTicksLimit: 12 }
        },
        y: { title: { display: true, text: 'value' } }
      }
    };

    const chartSpeed = new Chart(document.getElementById('chartSpeed'), {
      type: 'line',
      data: { datasets: [{ label: 'Speed', data: [], borderColor: '#6cf', tension: 0.1 }] },
      options: chartOpts
    });

    const chartPots = new Chart(document.getElementById('chartPots'), {
      type: 'line',
      data: {
        datasets: [
          { label: 'STP3', data: [], borderColor: '#9c6', tension: 0.1 },
          { label: 'STP4', data: [], borderColor: '#f9a', tension: 0.1 }
        ]
      },
      options: chartOpts
    });

    function pushPoint(k, t, y) {
      const num = typeof y === 'number' ? y : parseFloat(y);
      if (!Number.isFinite(num)) return;
      const label = epochToHms(t);
      if (k === SPEED_KEY) {
        speedPoints.push({ label, y: num });
        while (speedPoints.length > MAX_POINTS) speedPoints.shift();
        chartSpeed.data.datasets[0].data = speedPoints.map(p => ({ x: p.label, y: p.y }));
        chartSpeed.update('none');
      } else if (k === STP3_KEY) {
        stp3Points.push({ label, y: num });
        while (stp3Points.length > MAX_POINTS) stp3Points.shift();
        chartPots.data.datasets[0].data = stp3Points.map(p => ({ x: p.label, y: p.y }));
        chartPots.update('none');
      } else if (k === STP4_KEY) {
        stp4Points.push({ label, y: num });
        while (stp4Points.length > MAX_POINTS) stp4Points.shift();
        chartPots.data.datasets[1].data = stp4Points.map(p => ({ x: p.label, y: p.y }));
        chartPots.update('none');
      }
    }

    function renderTable() {
      tbody.innerHTML = allRows.map(r => '<tr><td class="mono">' + epochToHms(r.timestamp) +
        '</td><td class="mono">' + r.id_hex + '</td><td>' + escapeHtml(r.message) +
        '</td><td>' + escapeHtml(r.signal) + '</td><td class="mono">' + escapeHtml(String(r.value)) +
        '</td><td>' + escapeHtml(r.unit || '') + '</td></tr>').join('');
    }

    function escapeHtml(s) {
      const d = document.createElement('div');
      d.textContent = s;
      return d.innerHTML;
    }

    ws.onopen = () => {
      statusEl.textContent = 'Connected';
      statusEl.classList.remove('disconnected');
    };
    ws.onclose = () => {
      statusEl.textContent = 'Disconnected';
      statusEl.classList.add('disconnected');
    };
    ws.onerror = () => {
      statusEl.textContent = 'WebSocket error';
      statusEl.classList.add('disconnected');
    };
    ws.onmessage = (ev) => {
      const msg = JSON.parse(ev.data);
      const k = keyFor(msg);
      latestByKey.set(k, msg);
      allRows.unshift(msg);
      pushPoint(k, msg.timestamp, msg.value);
      renderTable();
    };

    clearBtn.addEventListener('click', () => {
      allRows.length = 0;
      renderTable();
    });
  </script>
</body>
</html>
"""


def json_safe_value(v: Any) -> Any:
    if isinstance(v, (int, float, str, bool)) or v is None:
        return v
    return str(v)


def make_app(
    db: Any,
    interfaces: list[str],
    log_all_ids: bool,
) -> FastAPI:
    clients: set[WebSocket] = set()
    line_queue: queue.Queue = queue.Queue()
    processes: list = []
    main_loop: asyncio.AbstractEventLoop | None = None
    lock = threading.Lock()

    async def broadcast(payload: dict[str, Any]) -> None:
        dead: list[WebSocket] = []
        for ws in list(clients):
            try:
                await ws.send_text(json.dumps(payload))
            except Exception:
                dead.append(ws)
        for ws in dead:
            clients.discard(ws)

    def on_frame(payload: dict[str, Any]) -> None:
        loop = main_loop
        if loop is None:
            return
        asyncio.run_coroutine_threadsafe(broadcast(payload), loop)

    def consumer_worker() -> None:
        num_if = len(interfaces)
        done = 0
        while done < num_if:
            line = line_queue.get()
            if line is None:
                done += 1
                continue
            parsed = parse_candump_line(line)
            if not parsed:
                continue
            timestamp, can_id, data = parsed
            if not log_all_ids and can_id not in TARGET_IDS:
                continue
            result = decode_frame(db, can_id, data)
            if not result:
                continue
            msg_name, signals = result
            id_hex = f"0x{can_id:X}"
            for sig_name, (value, unit) in signals.items():
                on_frame(
                    {
                        "timestamp": timestamp,
                        "id_hex": id_hex,
                        "message": msg_name,
                        "signal": sig_name,
                        "value": json_safe_value(value),
                        "unit": unit or "",
                    }
                )

    @asynccontextmanager
    async def lifespan(app: FastAPI):
        nonlocal main_loop
        main_loop = asyncio.get_running_loop()
        for iface in interfaces:
            t = threading.Thread(
                target=_candump_reader,
                args=(iface, line_queue, processes),
                daemon=True,
            )
            t.start()
        threading.Thread(target=consumer_worker, daemon=True).start()
        yield
        for p in processes:
            try:
                p.terminate()
                p.wait(timeout=2)
            except Exception:
                pass

    app = FastAPI(lifespan=lifespan)

    @app.get("/", response_class=HTMLResponse)
    async def index() -> str:
        return DASHBOARD_HTML

    @app.websocket("/ws")
    async def websocket_endpoint(websocket: WebSocket) -> None:
        await websocket.accept()
        with lock:
            clients.add(websocket)
        try:
            while True:
                await websocket.receive_text()
        except WebSocketDisconnect:
            pass
        finally:
            with lock:
                clients.discard(websocket)

    return app


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Real-time CAN dashboard (candump + DBC + WebSocket)"
    )
    parser.add_argument(
        "interface",
        nargs="?",
        default="vcan0",
        help="Primary SocketCAN interface (default: vcan0)",
    )
    parser.add_argument(
        "--pt-interface",
        default=None,
        metavar="INTERFACE",
        help="Optional second interface (e.g. vcan1)",
    )
    parser.add_argument(
        "--veh-dbc",
        type=Path,
        default=None,
        help="Path to veh.dbc (default: <script_dir>/dbc/veh.dbc)",
    )
    parser.add_argument(
        "--pt-dbc",
        type=Path,
        default=None,
        help="Path to pt.dbc (default: <repo_root>/projects/pt.dbc)",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Decode every decodable message ID, not only TARGET_IDS",
    )
    parser.add_argument("--host", default="0.0.0.0", help="Bind address (default: 0.0.0.0)")
    parser.add_argument("--port", type=int, default=8000, help="HTTP port (default: 8000)")
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    veh_dbc = args.veh_dbc or (script_dir / "dbc" / "veh.dbc")
    pt_dbc = args.pt_dbc or (script_dir.parent.parent / "projects" / "pt.dbc")

    if not veh_dbc.exists():
        print(f"Error: veh.dbc not found at {veh_dbc}", file=sys.stderr)
        return 1

    db = load_dbc(veh_dbc, pt_dbc)
    interfaces = [args.interface]
    if args.pt_interface:
        interfaces.append(args.pt_interface)

    app = make_app(db, interfaces, log_all_ids=args.all)

    print(
        f"Dashboard: http://{args.host}:{args.port}/  (interfaces: {', '.join(interfaces)})",
        file=sys.stderr,
    )
    uvicorn.run(app, host=args.host, port=args.port, log_level="info")
    return 0


if __name__ == "__main__":
    sys.exit(main())
