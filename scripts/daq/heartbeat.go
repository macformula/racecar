package main

import (
	"crypto/rand"
	"encoding/hex"
	"fmt"
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
