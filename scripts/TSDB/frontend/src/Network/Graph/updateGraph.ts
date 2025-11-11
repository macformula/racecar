import { tokenStorage } from '../../api/auth';

interface GraphData {
  timestamp: number;
  value: number;
}

export const writeGraphPoint = async ({ timestamp, value }: GraphData) => {
  const token = tokenStorage.getToken();
  
  if (!token) {
    throw new Error("Not authenticated. Please login first.");
  }

  try {
    const response = await fetch("/api/write-graph", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "Authorization": `Bearer ${token}`,
      },
      body: JSON.stringify({ timestamp, value }),
    });

    if (!response.ok) {
      const error = await response.json();
      throw new Error(error.detail || `HTTP error! status: ${response.status}`);
    }

    const result = await response.json();
    console.log("Data written:", result);
    return result;
  } catch (error) {
    console.error("Error writing data:", error);
    throw error;
  }
};
