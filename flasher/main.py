import os
import shutil
from nicegui import ui
from nicegui.events import UploadEventArguments

shutil.rmtree("uploads", ignore_errors=True)

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
    ui.timer(0.1, lambda: ecu_select.set_value('Select an ECU'), once=True) # Timer since upload doesn't reset in time

# UI components and homepage

# with ui.row().classes('items-center space-x-4'):
#     ui.image('logo.png').classes('w-16')
#     ui.markdown("# **Formula Flasher**")

ui.markdown("""
            
Welcome to Formula Flasher!
            
Formula Flasher is a simple tool to flash binary files to different ECUs in the racecar. See [docs](https://macformula.github.io/racecar/) for more information.
            
Use the following steps to flash a binary file:
            
1. Select the ECU
            
2. Select the binary file (it will be automatically uploaded)
            
3. Click the "Flash" button      
""")

file_path = ui.label(None)
file_path.set_visibility(False)

ecu_select = ui.select(['Select an ECU', 'Dashboard', 'FrontController', 'LVController', 'TMS'], value='Select an ECU')

upload_box = ui.upload(label="Select File (*.bin)", on_upload=lambda file: process_upload(file), auto_upload=True).props("accept=.bin")
upload_box.bind_enabled_from(ecu_select, 'value', lambda v: v != 'Select an ECU')

flash_button = ui.button("Flash", on_click=flash_file)
flash_button.disable()

ui.label().bind_text(globals(), 'file_name')

ui.colors(primary="#AA1F26")

ui.run(
    title="Formula Flasher",
    favicon="favicon.ico",
    show=False,
    dark=None,
    port=8000
)
