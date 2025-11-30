package main

import (
	"bytes"
	"crypto/rand"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"net/http"
	"os"
	"time"

	"go.einride.tech/can/pkg/socketcan"
	"go.uber.org/zap"
)

type HeartbeatHandler struct {
	serverURL string
	vehicleID string
	sessionID string
	can0      *socketcan.Receiver
	can1      *socketcan.Receiver
	logger    *zap.Logger
}

func NewHeartbeatHandler(can0, can1 *socketcan.Receiver, logger *zap.Logger) *HeartbeatHandler {

	// Configuring Vehicle ID
	vehicleID := os.Getenv("VEHICLE_ID")
	if vehicleID == "" {
		logger.Error("VEHICLE_ID not found, using default.")
		vehicleID = "default"
	}

	// Generating Session ID
	sessionBytes := make([]byte, 16)
	_, err := rand.Read(sessionBytes)
	var sessionID string
	if err != nil {
		logger.Warn("Failed to generate session ID, defaulting to time based session ID.")
		sessionID = fmt.Sprintf("fallback-%d", time.Now().UnixNano())
	} else {
		sessionID = hex.EncodeToString(sessionBytes)
	}

	serverURL := os.Getenv("SERVER_URL")
	if serverURL == "" {
		logger.Error("SERVER_URL for heartbeatnot found")
	}

	return &HeartbeatHandler{
		serverURL: serverURL,
		vehicleID: vehicleID,
		sessionID: sessionID,
		can0:      can0,
		can1:      can1,
		logger:    logger,
	}
}

func (h *HeartbeatHandler) SendHeartbeat() error {
	can0Active, can1Active := h.checkCAN()

	payload := map[string]interface{}{
		"timestamp":  time.Now().UnixMilli(),
		"vehicle_id": h.vehicleID,
		"session_id": h.sessionID,
		"can_status": map[string]bool{
			"can0": can0Active,
			"can1": can1Active,
		},
	}

	jsonPayload, err := json.Marshal(payload)
	if err != nil {
		h.logger.Error("Failed to convert heartbeat payload to JSON", zap.Error(err))
		return err
	}

	response, err := http.Post(h.serverURL, "application/json", bytes.NewBuffer(jsonPayload))
	if err != nil {
		h.logger.Error("Failed to send heartbeat", zap.Error(err))
		return err
	}
	defer response.Body.Close()

	return nil
}

func (h *HeartbeatHandler) checkCAN() (bool, bool) {
	can0Active := h.can0 != nil
	can1Active := h.can1 != nil
	return can0Active, can1Active
}
