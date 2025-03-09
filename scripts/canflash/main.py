import os
import shutil
from nicegui import ui
from nicegui.events import UploadEventArguments

# Constants

ECU_PLACEHOLDER = 'Select an ECU'
ECUS = ['FrontController', 'LVController']

# Clear uploads folder on startup

shutil.rmtree("uploads", ignore_errors=True)

@ui.page('/')
def main_page():

    ui.colors(primary="#AA1F26")

    def process_upload(file: UploadEventArguments):
        file_path.set_text(file.name)
        os.makedirs("uploads", exist_ok=True)
        with open(os.path.join("uploads", file.name), 'wb') as f:
            f.write(file.content.read())

        flash_button.enable()

    def flash_file():
        ui.notify(f"Flashed {file_path.text} to {ecu_select.value}", position="center")

        upload_box.reset()
        flash_button.disable()

    ui.markdown("""
            
    Welcome to CAN Flash!
                
    CAN Flash is a simple tool to flash binary files to different ECUs in the racecar. See [docs](https://macformula.github.io/racecar/) for more information.
                
    Use the following steps to flash a binary file:
                
    1. Select the ECU
                
    2. Select the binary file (it will be automatically uploaded)
                
    3. Click the "Flash" button      
    """)

    ui.timer(0.1, lambda: ecu_select.set_value(ECU_PLACEHOLDER), once=True) # Timer since upload doesn't reset in time

    file_path = ui.label(None)
    file_path.set_visibility(False)

    ecu_select = ui.select([ECU_PLACEHOLDER] + ECUS, value=ECU_PLACEHOLDER)

    upload_box = ui.upload(label="Select File (*.bin)", on_upload=lambda file: process_upload(file), auto_upload=True).props("accept=.bin")
    upload_box.bind_enabled_from(ecu_select, 'value', lambda v: v != ECU_PLACEHOLDER)

    flash_button = ui.button("Flash", on_click=flash_file)
    flash_button.disable()

ui.label().bind_text(globals(), 'file_name')

if __name__ in {"__main__", "__mp_main__"}:
    ui.run(
        title="CAN Flash",
        favicon="favicon.ico",
        show=False,
        dark=None,
        port=8000
    )

# TODO:
# Implement CAN teardown/setup and flashing
# Security provided by local wifi connection to Raspberry Pi hotspot (no login needed)
# User to enter name so we can track which files were flashed and when
# Extra confirmation if file name being flashed does not match selected ECU
# Remove Dashboard and TMS
# Real time data streaming (pedals, steering, CAN messages, etc.)
