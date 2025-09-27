from fastapi import FastAPI, HTTPException          
from pydantic import BaseModel
from typing import Optional, Dict, Any

from scripts.TSDB.influx import write_row_to_test_bucket

app = FastAPI()

class DataPoint(BaseModel):
    measurement: str
    tags: Optional[Dict[str, str]] = None
    fields: Optional[Dict[str, Any]] = None

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