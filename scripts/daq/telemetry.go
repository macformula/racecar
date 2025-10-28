package main

import (
	"container/list"
	"database/sql"
	"fmt"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can"
	_ "modernc.org/sqlite"
)

const BufferSize = 512

type TelemetryPacket struct {
	Id        int64    `db:"id"`
	Timestamp int64    `db:"timestamp"`
	FrameId   uint32   `db:"frame_id"`
	FrameData can.Data `db:"frame_data"`
}

type TelemetryHandler struct {
	buf *list.List
	db  *sql.DB
}

func NewTelemetryHandler() (*TelemetryHandler, error) {
	db, err := sql.Open("sqlite", "./can_cache.sqlite")
	if err != nil {
		return nil, err
	}

	_, err = db.Exec(`
		CREATE TABLE IF NOT EXISTS can_cache (
			id INTEGER PRIMARY KEY,
			timestamp INTEGER,
			frame_id INTEGER,
			frame_data INTEGER
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
	}

	err = t.fillBufferFromDisk()
	if err != nil {
		db.Close()
		return nil, err
	}

	return &t, nil
}

func (t *TelemetryHandler) Enqueue(frame canlink.TimestampedFrame) error {
	query, err := t.db.Prepare(`
		INSERT INTO can_cache (timestamp, frame_id, frame_data) VALUES (?, ?, ?)
	`)
	if err != nil {
		return err
	}

	res, err := query.Exec(frame.Time.UnixMilli(), frame.Frame.ID, frame.Frame.Data.PackBigEndian())
	if err != nil {
		return err
	}

	// Ignore the error since the schema enforces an auto-incrementing primary key
	id, _ := res.LastInsertId()

	elem := list.Element{
		Value: TelemetryPacket{
			Id:        id,
			Timestamp: frame.Time.UnixMilli(),
			FrameId:   frame.Frame.ID,
			FrameData: frame.Frame.Data,
		},
	}

	if t.buf.Len() >= BufferSize {
		t.buf.Remove(t.buf.Back())
	}
	t.buf.MoveToFront(&elem)

	return nil
}

func (t *TelemetryHandler) Upload() error {
	// pop from front of buffer and write to backend

	return nil
}

func (t *TelemetryHandler) fillBufferFromDisk() error {
	t.buf.Init()

	query, err := t.db.Prepare(`
		SELECT id, timestamp, frame_id, frame_data FROM can_cache ORDER BY timestamp DESC LIMIT ?
	`)
	if err != nil {
		return err
	}

	rows, err := query.Query(BufferSize)
	if err != nil {
		return err
	}

	for rows.Next() {
		var packet TelemetryPacket
		var packedFrameData uint64

		err = rows.Scan(&packet.Id, &packet.Id, &packet.FrameId, &packedFrameData)
		if err != nil {
			fmt.Printf("Failed to scan packet: %v\n", err)
			continue
		}

		var frameData can.Data
		frameData.UnpackBigEndian(packedFrameData)
		packet.FrameData = frameData

		t.buf.PushBack(packet)
	}

	rows.Close()
	return nil
}
