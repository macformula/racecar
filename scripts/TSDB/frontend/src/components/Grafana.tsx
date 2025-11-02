// In your React component
function GrafanaChart() {
  return (
    <iframe
      src="http://localhost:3000/public-dashboards/e3ef192b990a49e8bcf5325aa62c8268"
      width="800"
      height="500"
      frameBorder={0}
      title="Grafana Dashboard"
    />
  );
}
export default GrafanaChart;
