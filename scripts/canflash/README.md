# CAN Flash

CAN Flash is a simple tool used to wirelessly flash binary files to various ECUs in the racecar. As of now it supports the following ECUs:

- FrontController
- LvController
- TMS

## Usage

A web server is ran on the Raspberry Pi in the racecar, which you can access from a web browser on your computer **only when connected to the Raspberry Pi's local network**.

The default link is [http://192.168.12.1:8000/](http://192.168.12.1:8000/) (subject to change).

On this web interface, select the ECU and binary firmware file you want to flash it with, and click the "Flash" button.

Your file will be uploaded and stored to the Raspberry Pi, then flashed to the selected ECU. You will receive a notification when the flashing process is complete.

## Development

The server should automatically start when the Raspberry Pi boots up.

To run the server locally or on the Raspberry Pi, use the following steps:

Install the required dependencies:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

Run the server with the following command:

```bash
python3 main.py
```

Access the UI by opening [http://localhost:8000/](http://localhost:8000/) in your web browser.
