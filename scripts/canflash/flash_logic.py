import os
import time
from datetime import datetime

# Constants
ECUS = ["FrontController", "LVController", "TMS"]
UPLOAD_DIR = "uploads"

# Ensure the upload directory exists
os.makedirs(UPLOAD_DIR, exist_ok=True)


def generate_ecu_directory(ecu: str) -> str:
    timestamp = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
    ecu_dir = f"{ecu.lower()}-{timestamp}"
    full_path = os.path.join(UPLOAD_DIR, ecu_dir)

    os.makedirs(full_path, exist_ok=True)  # Create the directory if it doesn't exist
    return full_path


def save_uploaded_file(ecu: str, file_name: str, file_content: bytes) -> str:
    ecu_dir = generate_ecu_directory(ecu)
    file_path = os.path.join(ecu_dir, file_name)

    with open(file_path, "wb") as f:
        f.write(file_content)

    return file_path


def flash_file(ecu: str, file_path: str):
    if not ecu or not file_path:
        raise ValueError("ECU and file must be selected.")

    print(f"[INFO] Flashing {file_path} to {ecu}...")
    time.sleep(2)  # Simulate flashing delay
    print("[INFO] Flashing complete.")
