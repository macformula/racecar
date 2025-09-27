import React from "react";
import GrafanaChart from "../Components/Grafana";

const Home: React.FC = () => {
  return (
    <div className="home">
      <GrafanaChart />
      <h1>TSDB Dashboard</h1>
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
