This is just a temporary working directory for daq...

## Real-time CAN dashboard (WSL / Linux)

Requires `can-utils`, `vcan0` (or real CAN), Python venv with deps from `requirements.txt`.

```bash
cd scripts/daq
source .venv/bin/activate
python3 -m pip install -r requirements.txt
python3 dashboard.py vcan0
```

Open **http://localhost:8000** in a browser. In another terminal, send frames, e.g.:

```bash
cansend vcan0 136#12AB
```

Use `--all` to decode every message ID in the DBC (not only `TARGET_IDS`). Use `--pt-interface vcan1` for a second bus.

The table keeps **all** received rows (use **Clear table** to reset). The chart uses local **HH:MM:SS.mmm** on the x-axis.

### Automated CAN sender (optional)

Runs DBC-encoded frames in a loop via `cansend` (you can still use manual `cansend` in another terminal):

```bash
python3 can_sender.py vcan0
python3 can_sender.py vcan0 --interval 0.2 --all
```

`--all` cycles every encodable message in the DBC with zero defaults. Default mode cycles `TARGET_IDS` messages with demo signal values.

---

Generate Go DBC bindings:
```
go get -u go.einride.tech/can
cd dbc
go run go.einride.tech/can/cmd/cantool generate ./ ../generated/
```

Had to remove the "Reset" message from DashCommand to avoid name conflicts with bindings
> TODO: either find a better Go DBC binding generator or patch the upstream DBC to
> avoid having messages named "Reset" 