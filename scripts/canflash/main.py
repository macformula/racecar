from nicegui import ui
from nicegui.events import UploadEventArguments
from flash_logic import save_uploaded_file, flash_file
from metadata import load_metadata, save_metadata
from constants import ECU_CONFIG
import os
import json


class CanFlashApp:
    def __init__(self):
        self.starting_ecu = "Select ECU"
        self.selected_ecu = self.starting_ecu
        self.uploaded_file_path = None
        self.build_ui()

    def handle_upload(self, file: UploadEventArguments):
        self.uploaded_file_path = save_uploaded_file(self.selected_ecu, file.name, file.content.read())
        self.flash_button.enable()

    def handle_flash(self):
        exception = flash_file(self.selected_ecu, self.uploaded_file_path)

        if not exception:
            uploaded_file_name = os.path.split(self.uploaded_file_path)[-1]
            ui.notify(
                f"Successfully flashed {uploaded_file_name} to {self.selected_ecu}",
                position="center",
            )

            # Reset upload box and disable flash button
            self.upload_box.reset()
            self.flash_button.disable()

            # Reset ECU selection after a short delay (or else the select box won't reset)
            ui.timer(0.1, lambda: self.ecu_select.set_value(self.starting_ecu), once=True)
        else:
            ui.notify(str(exception), type="negative")

    def save_notes(self, notes: str, file_name: str):
        try:
            with open("metadata.json", 'r') as f:
                content = f.read().strip()
                metadata = json.loads(content) if content else {}
        except FileNotFoundError:
            metadata = {}

        # Update or add the notes for this file
        if file_name not in metadata:
            metadata[file_name] = {}
        metadata[file_name]["notes"] = notes

        with open("metadata.json", 'w') as f:
            json.dump(metadata, f, indent=2)

    def build_ui(self):
        ui.colors(primary="#AA1F26")
        ui.markdown("Welcome to CAN Flash! Select an ECU and upload a file to flash. See [docs](https://macformula.github.io/racecar/) for more information.")

        # ECU Selection
        self.ecu_select = ui.select([self.starting_ecu] + list(ECU_CONFIG.keys()))
        self.ecu_select.bind_value(self, "selected_ecu")

        # File Upload
        self.upload_box = ui.upload(
            label="Select File",
            on_upload=self.handle_upload,
            auto_upload=True,
        ).props("accept=.bin")
        self.upload_box.bind_enabled_from(
            self.ecu_select, "value", lambda v: v != self.starting_ecu
        )

        # Flash Button
        self.flash_button = ui.button("Flash", on_click=self.handle_flash)
        self.flash_button.disable()

        file_dropdown = ui.select(
        label='Select file',
        options=list(load_metadata().keys())
)

        self.textbox = ui.input(label='Enter Metadata Notes')

        ui.button('Submit', on_click=lambda: (
            self.save_notes(self.textbox.value, file_dropdown.value), 
            setattr(self.textbox, 'value', '')))
        
        


@ui.page("/")
def main_page():
    CanFlashApp()


if __name__ in {"__main__", "__mp_main__"}:
    ui.run(
        title="CAN Flash",
        favicon="favicon.ico",
        show=False,
        dark=None,
        port=8000,
    )
