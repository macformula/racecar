from nicegui import ui, events
import subprocess


class LiveTuning:

    def __init__(self, iface: str = "vcan0") -> None:
        self.iface = iface                    
        self.aggressiveness = 0
        self.dampness = 0
        self._build_ui()

    def _slider_row(
        self,
        title: str,
        field_name: str,
        init_val: int = 0,
        *,
        on_change_cb,
    ):
        with ui.row().classes("items-center gap-4 w-full"):
            ui.label(title).classes("w-32 text-right")

            slider = (
                ui.slider(min=0, max=100, value=init_val, on_change=on_change_cb)
                .props("step=1")
                .classes("flex-1")            
            )

            number = (
                ui.number(value=init_val, min=0, max=100)
                .props("style='width:80px'")
            )

            number.bind_value_from(slider, "value").bind_value_to(slider, "value")

            setattr(self, f"{field_name}_slider", slider)
            setattr(self, f"{field_name}_number", number)

    def _build_ui(self) -> None:
        """Compose the entire visible UI."""
        with ui.card().classes("w-full max-w-md mx-auto mt-14 p-6 shadow-lg"):
            ui.label("Live Tuning").classes("text-2xl font-semibold mb-6 text-center")

            # Aggressiveness row 
            self._slider_row(
                "Aggressiveness",
                "aggressiveness",
                init_val=self.aggressiveness,
                on_change_cb=lambda e: self._on_change("aggressiveness", e),
            )

            ui.separator()

            # Dampness row 
            self._slider_row(
                "Dampness",
                "dampness",
                init_val=self.dampness,
                on_change_cb=lambda e: self._on_change("dampness", e),
            )

            ui.button(
                "Submit",
                icon="send",
                on_click=self._submit,
            ).classes("w-full bg-primary text-white mt-6")

    def _on_change(self, field: str, e: events.ValueChangeEventArguments) -> None:
        # Clamp, validate, and store slider changes
        value = e.value or 0
        value = max(0, min(100, int(value)))
        setattr(self, field, value)
        print(f"{field.capitalize()} set to {value}")

    def _submit(self, _=None) -> None:
        print(f"Submit pressed (agg={self.aggressiveness}, damp={self.dampness})")
        subprocess.run(
            ["bash", "./scripts/tune.sh",
             str(self.aggressiveness), str(self.dampness), self.iface],
            check=True,
        )


if __name__ in {"__mp_main__", "__main__"}:
    LiveTuning()          
    ui.run()
