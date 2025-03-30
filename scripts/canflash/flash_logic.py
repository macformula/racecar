import os
import subprocess
from datetime import datetime
from constants import ECU_CONFIG, UPLOAD_DIR


def save_uploaded_file(ecu: str, file_name: str, file_content: bytes) -> str:
    timestamp = datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
    ecu_dir = f"{ecu.lower()}-{timestamp}"
    full_path = os.path.join(UPLOAD_DIR, ecu_dir)

    os.makedirs(full_path, exist_ok=True)
    file_path = os.path.join(full_path, file_name)

    with open(file_path, "wb") as f:
        f.write(file_content)

    return file_path


def flash_file(ecu: str, file_path: str):
    gpio_pin = str(ECU_CONFIG[ecu]["gpio_pin"])
    can_message_id = str(ECU_CONFIG[ecu]["can_message_id"])

    try:
        subprocess.run(["bash", "./scripts/canflash.sh", file_path, gpio_pin, can_message_id])
        return None
    except Exception as e:
        return e
