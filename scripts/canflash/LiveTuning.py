from nicegui import ui, events
import subprocess
class LiveTuning:
    def __init__(self) -> None:
        self.value1 = 0          
        self.build_ui()

    def build_ui(self) -> None:
        with ui.column().classes('items-center justify-center h-screen'):
            # Slider + numeric input side‑by‑side
            with ui.row().classes('items-center'):
                slider = (
                    ui.slider(min=0, max=100, value=0, on_change=self.on_slider_change)
                    .props('step=1')
                    .classes('w-64')          # narrow the slider a bit
                )
                number = (
                    ui.number(value=0, min=0, max=100)
                    .props('style="width:80px height:20px"')  # small input box
                )

                # keep both widgets in sync
                number.bind_value_from(slider, 'value').bind_value_to(slider, 'value')

            # Submit button (placeholder)
            ui.button('Submit', on_click=self.submit)

    def on_slider_change(self, e : events.ValueChangeEventArguments) -> None:
        self.value1 = e.value 
        if type(e.value) != int and type(e.value) != float:
            self.value1 = 0
            e.value = 0
        if self.value1 > 100:
            self.value1 = 100
        if self.value1 < 0:
            self.value1 = 0
        print(f'Slider changed to {self.value1}')

    def submit(self, _=None, iface = 'can0') -> None:

        print(f'Submit pressed (current value: {self.value1})')
        subprocess.run(["bash", "./scripts/tune.sh", str(self.value1), iface], check=True)