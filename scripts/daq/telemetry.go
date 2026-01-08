package main

import (
	"context"
	"container/list"
	"database/sql"
	"errors"
	"fmt"
	"os"
	"strconv"
	"sync"
    "time"

	influxdb2 "github.com/influxdata/influxdb-client-go/v2"
	"github.com/influxdata/influxdb-client-go/v2/api"
	"github.com/macformula/hil/canlink"
	"go.einride.tech/can"
	"go.uber.org/zap"
	_ "modernc.org/sqlite"
   
)

const BufferSize = 512

type TelemetryPacket struct {
	Id        int64    `db:"id"`
	Timestamp int64    `db:"timestamp"`
	FrameId   uint32   `db:"frame_id"`
	FrameData can.Data `db:"frame_data"`
	CanBus	 string   `db:"bus_name"` 
}

type TelemetryHandler struct {
	buf    *list.List
	db     *sql.DB
	l      *zap.Logger
	dbFile string

	mu sync.Mutex

	// Influx
    influxClient influxdb2.Client
    writeAPI      api.WriteAPIBlocking
    influxOrg     string
    influxBucket  string
}

func NewTelemetryHandler(dbFile string, logger *zap.logger) (*TelemetryHandler, error) {
	db, err := sql.Open("sqlite", dbFile)
	if err != nil {
		return nil, err
	}

	_, err = db.Exec(`
		CREATE TABLE IF NOT EXISTS can_cache (
			id INTEGER PRIMARY KEY,
			timestamp INTEGER,
			frame_id INTEGER,
			frame_data INTEGER,
			bus_name TEXT
		)
	`)
	if err != nil {
		db.Close()
		return nil, err
	}

	_, err = db.Exec("CREATE INDEX IF NOT EXISTS idx_timestamp ON can_cache(timestamp)")
	if err != nil {
		db.Close()
		return nil, err
	}

	t := TelemetryHandler{
		buf: list.New(),
		db:  db,
		l:	logger,
	}

	err = t.fillBufferFromDisk()
	if err != nil {
		db.Close()
		return nil, err
	}

	fmt.Printf("Initial buffer size: %v\n", t.buf.Len())

	influxURL := os.Getenv("INFLUX_URL")
    influxToken := os.Getenv("INFLUX_TOKEN")
    influxOrg := os.Getenv("INFLUX_ORG")
    influxBucket := os.Getenv("INFLUX_BUCKET")

    if influxURL != "" && influxToken != "" && influxOrg != "" && influxBucket != "" {
        t.influxClient = influxdb2.NewClient(influxURL, influxToken)
        t.writeAPI = t.influxClient.WriteAPIBlocking(influxOrg, influxBucket)
        t.influxOrg = influxOrg
        t.influxBucket = influxBucket
    }

	return &t, nil
}

func (t *TelemetryHandler) Enqueue(frame canlink.TimestampedFrame, busName string) error {

	t.mu.Lock()
	defer t.mu.Unlock()

	query, err := t.db.Prepare(`
		INSERT INTO can_cache (timestamp, frame_id, frame_data, bus_name) VALUES (?, ?, ?, ?)
	`)
	if err != nil {
		return err
	}

	defer query.Close()

	res, err := query.Exec(frame.Time.UnixMilli(), frame.Frame.ID, frame.Frame.Data.PackBigEndian(), busName)
	if err != nil {
		return err
	}

	// Ignore the error since the schema enforces an auto-incrementing primary key
	id, _ := res.LastInsertId()

	packet :=TelemetryPacket{
		Id:        id,
		Timestamp: frame.Time.UnixMilli(),
		FrameId:   frame.Frame.ID,
		FrameData: frame.Frame.Data,
		CanBus:    busName,
	}

	t.buf.PushFront(packet)

	if t.buf.Len() > BufferSize {
		// go upload ?
		t.buf.Remove(t.buf.Back())
	}

	return nil
}

func (t *TelemetryHandler) Upload() error {

	// Make sure Influx is configured
	if t.writeAPI == nil {
		return errors.New("telemetry/upload: Influx not configured")
	}

	// Drain in-memory buffer into a slice
	packets := t.emptyBuffer()

	if len(packets) == 0 {
		t.mu.Lock()
		// Tracks whether the buffer is still empty. If empty, load from SQLite
		empty := (t.buf.Len() == 0)
		t.mu.Unlock()

		if empty {
			return t.fillBufferFromDisk()
		}
		return nil
	}

	// Create empty slice that will hold packets that failed upload or failed delete
	failed := make([]TelemetryPacket, 0)

	// delStmt is a prepared statement object. This line compiles a SQL statement for deletion. This is done for efficiency
	delStmt, err := t.db.Prepare(`DELETE FROM can_cache WHERE id = ?`)

	if err != nil {
        // If we can't delete at all, requeue everything (we already drained memory)
        t.mu.Lock()
        for _, p := range packets {
            t.buf.PushFront(p)

			// Drop oldest item if buffer exceeds max size
            if t.buf.Len() > BufferSize {
                t.buf.Remove(t.buf.Back())
            }
        }
        t.mu.Unlock()
        return err
    }
    defer delStmt.Close()

	 // For each packet: write to Influx. Ff success, delete from SQLite.
	for _, packet := range packets {

        // Tags are indexed metadata in Influx. They must be strings.
        tags := map[string]string{
            "can_bus":  packet.CanBus,
            "frame_id": strconv.FormatUint(uint64(packet.FrameId), 10),
        }

        fields := map[string]interface{}{
            "frame_data": packet.FrameData.PackBigEndian(),
        }

        // Create the point (measurement + tags + fields + timestamp)
        p := influxdb2.NewPoint(
            "can_frame",
            tags,
            fields,
            time.UnixMilli(packet.Timestamp),
        )

        // Write to InfluxDB (blocking = waits for success/error)
        if err := t.writeAPI.WritePoint(context.Background(), p); err != nil {
            failed = append(failed, packet)
            continue
        }

        // Only after Influx write succeeds: delete from SQLite
        if _, err := delStmt.Exec(packet.Id); err != nil {
            failed = append(failed, packet)
            continue
        }
    }

	// Requeue failures
	if len(failed) > 0 {
        t.mu.Lock()
        for _, p := range failed {
            t.buf.PushFront(p)
            if t.buf.Len() > BufferSize {
                t.buf.Remove(t.buf.Back())
            }
        }
        t.mu.Unlock()

        return fmt.Errorf("telemetry/upload: %d/%d packets failed", len(failed), len(packets))
    }

	// Load next batch fron disk if no failures and buffer is empty
	t.mu.Lock()
    empty := (t.buf.Len() == 0)
    t.mu.Unlock()

    if empty {
        return t.fillBufferFromDisk()
    }

	return nil
}

func (t *TelemetryHandler) emptyBuffer() []TelemetryPacket { // add fillbufferfromdisk

	t.mu.Lock()
	defer t.mu.Unlock()

	packets := make([]TelemetryPacket, 0, t.buf.Len())

	for e := t.buf.Front(); e != nil; {
		next := e.Next()
		packet := e.Value.(TelemetryPacket)
		packets = append(packets, packet)

		// Remove this element
		t.buf.Remove(e)

		// Move to next
		e = next
	}

	return packets
}

func (t *TelemetryHandler) fillBufferFromDisk() error {

	t.mu.Lock()
	defer t.mu.Unlock()

	t.buf.Init()

	query, err := t.db.Prepare(`
		SELECT id, timestamp, frame_id, frame_data, bus_name FROM can_cache ORDER BY timestamp DESC LIMIT ?
	`)
	if err != nil {
		return err
	}

	defer query.Close()

	rows, err := query.Query(BufferSize)
	if err != nil {
		return err
	}
	defer rows.Close()

	for rows.Next() {
		var packet TelemetryPacket
		var packedFrameData uint64

		err = rows.Scan(
			&packet.Id,
			&packet.Timestamp,
			&packet.FrameId,
			&packedFrameData,
			&packet.CanBus,
		)

		if err != nil {
			fmt.Printf("Failed to scan packet: %v\n", err)
			continue
		}

		var frameData can.Data
		frameData.UnpackBigEndian(packedFrameData)
		packet.FrameData = frameData

		t.buf.PushBack(packet)
	}

	return rows.Err()
}
