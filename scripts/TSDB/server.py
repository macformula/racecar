from fastapi import FastAPI, HTTPException          
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Optional, Dict, Any

from influx import write_row_to_test_bucket

app = FastAPI()

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allows all origins
    allow_credentials=True,
    allow_methods=["*"],  # Allows all methods
    allow_headers=["*"],  # Allows all headers
)

class DataPoint(BaseModel):
    measurement: str
    tags: Optional[Dict[str, str]] = None
    fields: Optional[Dict[str, Any]] = None

class GraphData(BaseModel):
    timestamp: int
    value: float

@app.post("/write")
def write_data(data_point: DataPoint):
    """
    Endpoint to write data to InfluxDB test bucket.
    """
    try:
        success = write_row_to_test_bucket(
            measurement=data_point.measurement,
            tags=data_point.tags,
            fields=data_point.fields
        )
        return {"status": "success", "written": success}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/write-graph")
def write_graph_data(data: GraphData):
    """
    Endpoint to write GraphData point to InfluxDB.
    """
    try:
        success = write_row_to_test_bucket(
            measurement="graph_data",
            tags={"source": "frontend"},
            fields={"value": data.value, "timestamp": data.timestamp}
        )
        return {"status": "success", "written": success}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/write-simple")
def write_simple(query: str):
    """
    Simple endpoint that writes a test measurement with the query as a field.
    """
    try:
        success = write_row_to_test_bucket(
            measurement="test_messages",
            tags={"source": "api"},
            fields={"message": query, "value": len(query)}
        )
        return {"status": "success", "written": success, "message": f"Wrote '{query}' to database"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


if __name__ == "__main__":
    import uvicorn
    uvicorn.run("server:app", host="0.0.0.0", port=8000, reload=True)