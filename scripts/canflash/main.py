from nicegui import ui
from CanFlashApp import CanFlashApp
from LiveTuning import LiveTuning
from contextlib import contextmanager

@contextmanager
def frame():
    ui.colors(primary="#AA1F26")
    with ui.header(elevated=True).classes('width-full text-white bg-[#AA1F26] '):
        ui.label("Raspberry Pi Resources").classes("text-xl font-bold mr-4 ")
        ui.button("Home", on_click=lambda : ui.navigate.to('/')).classes("!bg-[#df535a]")
        ui.button('Can Flash', on_click= lambda : ui.navigate.to("/canflash")).classes("!bg-[#df535a]")
        ui.button('Tuning', on_click= lambda : ui.navigate.to("/tune")).classes("!bg-[#df535a]")
    yield

@ui.page("/canflash")  
def main_page():
    with frame():
        CanFlashApp()

@ui.page("/tune")
def tuning_page():
    with frame():
        LiveTuning()

@ui.page('/')
def home() -> None:
    with frame():                                 # keep the shared header
        with ui.column().classes('items-center q-mt-xl gap-4'):
            ui.label('Welcome to the Formula‑Electric Raspberry Pi Interface') \
                .classes('text-2xl font-bold')

            ui.markdown(
                'This dashboard lets you interact with the **Formula Electric** car’s '
                'Raspberry Pi.  \n'
                'Use it to *live‑tune* parameters while the car is running or to '
                '*flash* new firmware over CAN.  '
                'More modules will be added soon!'
            ).classes('max-w-xl text-center')

            with ui.row().classes('gap-4'):
                ui.button('Go to Live Tuning', on_click=lambda: ui.navigate.to('/tune')) \
                    .classes('!bg-[#df535a] text-white')
                ui.button('Go to CAN Flash',  on_click=lambda: ui.navigate.to('/canflash')) \
                    .classes('!bg-[#df535a] text-white')

            ui.link('Read the full documentation',
                    'https://macformula.github.io/racecar/',
                    new_tab=True) \
              .classes('text-primary underline q-mt-md')
if __name__ in {"__main__", "__mp_main__"}:
    ui.run(
        title="CAN Flash",
        favicon="favicon.ico",
        show=False,
        dark=None,
        port=8000,
    )