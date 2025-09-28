import React, { useState } from "react";
import GrafanaChart from "../Components/Grafana";
import { writeGraphPoint } from "../Network/Graph/updateGraph";

const Home: React.FC = () => {
  const [timestamp, setTimestamp] = useState<string>("");
  const [value, setValue] = useState<string>("");

  const handleSubmit = async () => {
    if (!timestamp || !value) {
      alert("Please fill in both fields");
      return;
    }

    try {
      const timestampNum = parseInt(timestamp);
      const valueNum = parseFloat(value);

      await writeGraphPoint({ timestamp: timestampNum, value: valueNum });
      alert("Data sent successfully!");

      // Clear the inputs
      setTimestamp("");
      setValue("");
    } catch (error) {
      console.error("Error sending data:", error);
      alert("Error sending data");
    }
  };

  return (
    <div className="home">
      <h1>TSDB Dashboard</h1>
      <GrafanaChart />

      <div
        style={{
          margin: "20px 0",
          padding: "20px",
          border: "1px solid #ccc",
          borderRadius: "8px",
        }}
      >
        <h3>Send Data Point</h3>
        <div style={{ display: "flex", gap: "10px", alignItems: "center" }}>
          <input
            type="number"
            placeholder="Timestamp (Unix)"
            value={timestamp}
            onChange={(e) => setTimestamp(e.target.value)}
            style={{
              padding: "8px",
              borderRadius: "4px",
              border: "1px solid #ccc",
            }}
          />
          <input
            type="number"
            step="0.01"
            placeholder="Value"
            value={value}
            onChange={(e) => setValue(e.target.value)}
            style={{
              padding: "8px",
              borderRadius: "4px",
              border: "1px solid #ccc",
            }}
          />
          <button
            onClick={handleSubmit}
            style={{
              padding: "8px 16px",
              backgroundColor: "#007bff",
              color: "white",
              border: "none",
              borderRadius: "4px",
              cursor: "pointer",
            }}
          >
            Send Data
          </button>
        </div>
      </div>

      <p>Welcome to the Time Series Database Dashboard</p>
      <div className="home-content">
        <div className="card">
          <h2>Real-time Data</h2>
          <p>Monitor your race car telemetry data in real-time</p>
        </div>
        <div className="card">
          <h2>Historical Data</h2>
          <p>Analyze past performance and track improvements</p>
        </div>
        <div className="card">
          <h2>Data Management</h2>
          <p>Manage and configure your data sources</p>
        </div>
      </div>
    </div>
  );
};

export default Home;
