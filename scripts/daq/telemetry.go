package main

import (
	"container/list"
	"database/sql"
	"fmt"

	"github.com/macformula/hil/canlink"
	"go.einride.tech/can"
	"go.uber.org/zap"
	_ "modernc.org/sqlite"
)

const BufferSize = 512

// cached CAN frame with its database ID and timestamp in milliseconds
type TelemetryPacket struct {
	Id        int64    `db:"id"`
	Timestamp int64    `db:"timestamp"`
	FrameId   uint32   `db:"frame_id"`
	FrameData can.Data `db:"frame_data"`
}

// holds the SQLite connection and memory buffer for recorded CAN frames
type TelemetryHandler struct {
	buf    *list.List
	db     *sql.DB
	l      *zap.Logger
	dbFile string
}

// open the telemetry cache and load recent packets into memory
func NewTelemetryHandler(dbFile string) (*TelemetryHandler, error) {
	db, err := sql.Open("sqlite", dbFile)
	if err != nil {
		return nil, err
	}

	// create the frame cache if it does not already exist
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

	// index timestamps to speed up loading the most recent packets
	_, err = db.Exec("CREATE INDEX IF NOT EXISTS idx_timestamp ON can_cache(timestamp)")
	if err != nil {
		db.Close()
		return nil, err
	}

	// set up the memory buffer and database connection
	t := TelemetryHandler{
		buf: list.New(),
		db:  db,
	}

	// restore recent packets saved during previous runs
	err = t.fillBufferFromDisk()
	if err != nil {
		db.Close()
		return nil, err
	}

	fmt.Printf("Initial buffer size: %v\n", t.buf.Len())

	return &t, nil
}

// save a received frame and its source bus to SQLite, then update the memory buffer
func (t *TelemetryHandler) Enqueue(frame canlink.TimestampedFrame, busName string) error {
	// prepare the database insert for this frame
	query, err := t.db.Prepare(`
		INSERT INTO can_cache (timestamp, frame_id, frame_data, bus_name) VALUES (?, ?, ?, ?)
	`)
	if err != nil {
		return err
	}

	// store the timestamp in milliseconds and pack the CAN data into an integer
	res, err := query.Exec(frame.Time.UnixMilli(), frame.Frame.ID, frame.Frame.Data.PackBigEndian(), busName)
	if err != nil {
		return err
	}

	// Ignore the error since the schema enforces an auto-incrementing primary key
	id, _ := res.LastInsertId()

	// build the in-memory packet using the ID assigned by SQLite
	elem := list.Element{
		Value: TelemetryPacket{
			Id:        id,
			Timestamp: frame.Time.UnixMilli(),
			FrameId:   frame.Frame.ID,
			FrameData: frame.Frame.Data,
		},
	}

	// discard the last buffered packet when full; its SQLite row is kept
	if t.buf.Len() >= BufferSize {
		t.buf.Remove(t.buf.Back())
	}
	// intended to add the newest packet at the front, but MoveToFront only moves
	// existing list elements, so this new element is currently not inserted
	t.buf.MoveToFront(&elem)

	return nil
}

// upload is currently disabled and returns without sending any data
func (t *TelemetryHandler) Upload() error {
	// retained upload code serializes buffered frames and posts them to the backend
	/*buf := t.emptyBuffer()

	type temp struct {
		Value     []byte `json:"value"`
		Timestamp int64  `json:"timestamp"`
	}

	for i := range len(buf) {
		frame := can.Frame{
			ID:     buf[i].FrameId,
			Data:   buf[i].FrameData,
			Length: 8,
		}
		rawFrame, _ := json.Marshal(frame)

		toWrite := temp{
			Timestamp: buf[i].Timestamp,
			Value:     rawFrame,
		}

		data, err := json.Marshal(toWrite)
		if err != nil {
			return err
		}
		if i == 0 {
			fmt.Printf("payload: %s\n", string(data))
		}

		// If this fails then we should re-queue the entire buffer to not lose anything
		_, err = http.Post("http://localhost:5000/write-graph", "application/json", bytes.NewBuffer(data))
		if err != nil {
			fmt.Printf("telemetry/upload: failed to post frame data: %v\n", err)
			return err
		}
	}

	// In case of an error here, just log and keep going, we still need to upload our data
	err := t.fillBufferFromDisk()
	if err != nil {
		fmt.Printf("telemetry/upload: failed to fill buffer: %v\n", err)
	}*/

	return nil
}

// copy buffered packets into a slice without removing them from the list
func (t *TelemetryHandler) emptyBuffer() []TelemetryPacket {
	// this allocates entries before appending, leaving empty packets at the start
	packets := make([]TelemetryPacket, t.buf.Len())
	next := t.buf.Front()

	for range t.buf.Len() {
		packets = append(packets, next.Value.(TelemetryPacket))
		next = next.Next()
	}

	return packets
}

// replace the memory buffer with up to BufferSize recent packets from SQLite
func (t *TelemetryHandler) fillBufferFromDisk() error {
	// clear the current buffer before loading cached packets
	t.buf.Init()

	// read cached frames in order from newest to oldest
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

	// convert each database row back into a telemetry packet
	for rows.Next() {
		var packet TelemetryPacket
		var packedFrameData uint64

		// timestamp currently overwrites Id here instead of populating Timestamp
		err = rows.Scan(&packet.Id, &packet.Id, &packet.FrameId, &packedFrameData)
		if err != nil {
			fmt.Printf("Failed to scan packet: %v\n", err)
			continue
		}

		// unpack the stored integer back into CAN data bytes
		var frameData can.Data
		frameData.UnpackBigEndian(packedFrameData)
		packet.FrameData = frameData

		t.buf.PushBack(packet)
	}

	rows.Close()
	return nil
}
