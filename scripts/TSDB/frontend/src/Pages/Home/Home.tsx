import React, { useState } from "react";
import GrafanaChart from "../../components/Grafana";
import Navbar from "../../components/Navbar/Navbar";
import { writeGraphPoint } from "../../Network/Graph/updateGraph";
import styles from "./Home.module.css";

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
      <Navbar />
      <div className={styles.content}>
        <h1>TSDB Dashboard</h1>
        <GrafanaChart />
        <div className={styles.dataForm}>
          <h3>Send Data Point</h3>
          <div className={styles.formInputs}>
            <input
              type="number"
              placeholder="Timestamp (Unix)"
              value={timestamp}
              onChange={(e) => setTimestamp(e.target.value)}
              className={styles.input}
            />
            <input
              type="number"
              step="0.01"
              placeholder="Value"
              value={value}
              onChange={(e) => setValue(e.target.value)}
              className={styles.input}
            />
            <button onClick={handleSubmit} className={styles.button}>
              Send Data
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Home;
