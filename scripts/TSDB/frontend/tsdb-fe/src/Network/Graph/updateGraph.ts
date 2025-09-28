interface GraphData {
  timestamp: number;
  value: number;
}

export const writeGraphPoint = async ({ timestamp, value }: GraphData) => {
  try {
    const response = await fetch("http://localhost:8000/write-graph", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ timestamp, value }),
    });

    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }

    const result = await response.json();
    console.log("Data written:", result);
    return result;
  } catch (error) {
    console.error("Error writing data:", error);
    throw error;
  }
};
