from nicegui import ui
from nicegui.events import UploadEventArguments
from flash_logic import save_uploaded_file, flash_file
from metadata import load_metadata, save_metadata
from constants import ECU_CONFIG, UPLOAD_DIR
import os
import json

class home_page:
    def __init__(self):
        self.build_ui()

    def build_ui(self):
        ui.colors(primary="#AA1F26")
        ui.label('Welcome to CAN Flash!')
        ui.label('Select to flash a new binary file')
        ui.button('Upload a new binary file', on_click=lambda e: ui.navigate.to('/upload_new_bin'))
        ui.label('Select to flash an existing binary file or update its notes')
        ui.button('Flash an existing binary file', on_click=lambda e: ui.navigate.to('/upload_new_bin'))

# Upload new binary file and flash
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

        
# Flash an existing binary file or update notes
# Upload new binary file and flash
class CanFlashUploadExisting(CanFlashApp):
    def __init__(self):
        self.starting_ecu = "Select ECU"
        self.selected_ecu = self.starting_ecu
        self.bin_file = "Select Binary File"
        self.selected_bin_file = self.bin_file
        self.uploaded_file_path = None
        self.build_ui()

    def save_notes(self, notes: str, file_name: str):
        try:
            metadata = load_metadata()
        except FileNotFoundError:
            metadata = {}

        # Update or add the notes for this file
        metadata[file_name]["notes"] = notes

        with open("metadata.json", 'w') as f:
            json.dump(metadata, f, indent=2)

    def on_file_selection(self):
        if hasattr(self, "flash_button"):
            self.flash_button.enable()
        if hasattr(self, "save_notes_button"):
            self.save_notes_button.enable()

        path = os.path.join(UPLOAD_DIR, load_metadata().get(self.selected_bin_file, {}).get("upload name", ""))
        self.uploaded_file_path = os.path.join(path, load_metadata().get(self.selected_bin_file, {}).get("file name", ""))

        

    def build_ui(self):
        ui.colors(primary="#AA1F26")
        ui.markdown("Welcome to CAN Flash! Select an ECU and upload a file to flash. See [docs](https://macformula.github.io/racecar/) for more information.")

        # ECU Selection
        self.ecu_select = ui.select([self.starting_ecu] + list(ECU_CONFIG.keys()))
        self.ecu_select.bind_value(self, "selected_ecu")

        # Binary File Selection
        self.file_dropdown = ui.select([self.selected_bin_file] + list(load_metadata().keys()), 
                                        on_change=self.on_file_selection)
        self.file_dropdown.bind_value(self, "selected_bin_file")

        # Flash Button
        ui.markdown("")
        ui.markdown("Flash Binary File")
        self.flash_button = ui.button("Flash", on_click=self.handle_flash)
        self.flash_button.disable()

        # Update Notes

        ui.markdown("")
        ui.markdown("Update Notes for previously Uploaded Files")

        self.textbox = ui.input(label='Enter Metadata Notes')

        self.save_notes_button = ui.button('Submit', on_click=lambda: (
            self.save_notes(self.textbox.value, file_dropdown.value), 
            setattr(self.textbox, 'value', '')))  
        self.save_notes_button.disable()

        # Delete Binary File 

        #FIXME
        '''ui.markdown("")
        ui.markdown("Delete Binary File")
        self.delete_button = ui.button("Delete Binary File", on_click=lambda: delete_metadata("uploads"))
        self.delete_button.disable()'''


@ui.page("/")
def main_page():
    home_page()
    
    
@ui.page("/upload_new_bin")
def upload_new_bin():
    CanFlashApp()
    
    #Navigation
    ui.markdown("")
    ui.markdown("Navigation")
    ui.button('Home', on_click=lambda e: ui.navigate.to('/'))
    ui.button('Flash an existing binary file or update notes', on_click=lambda e: ui.navigate.to('/upload_old_bin'))

@ui.page("/upload_old_bin")
def reupload_existing_bin():
    CanFlashUploadExisting()
    
    #Navigation
    ui.markdown("")
    ui.markdown("Navigation")
    ui.button('Home', on_click=lambda e: ui.navigate.to('/'))
    ui.button('Upload a new binary file', on_click=lambda e: ui.navigate.to('/upload_new_bin'))

if __name__ in {"__main__", "__mp_main__"}:
    ui.run(
        title="CAN Flash",
        favicon="favicon.ico",
        show=False,
        dark=None,
        port=8000,
    )
