import os
import subprocess
import json

METADATA_FILE = "metadata.json"

def load_metadata():
    if os.path.exists(METADATA_FILE):
        with open(METADATA_FILE, "r") as f:
            return json.load(f)
    return {}

def save_metadata(file_name: str, notes: str, timestamp: str, ecu: str, upload_name: str):

    metadata = load_metadata()

    metadata[upload_name] = {
        "file name": file_name,
        "upload name": upload_name,
        "ecu": ecu,
        "notes": notes,
        "uploaded_at": timestamp,
    }

    
    with open(METADATA_FILE, "w") as f:
        json.dump(metadata, f, indent=2)
