# Directory for file uploads
UPLOAD_DIR = "uploads"

# ECU Configuration with GPIO pins and CAN message IDs
ECU_CONFIG = {
    "FrontController": {
        "gpio_pin": 8,
        "can_message_id": 0,
    },
    "LVController": {
        "gpio_pin": 6,
        "can_message_id": 1,
    },
    "TMS": {
        "gpio_pin": 0,
        "can_message_id": 2,
    },
}
