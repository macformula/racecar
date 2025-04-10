# racecar website

The raw files in this directory are not intended for viewing. View the website at <https://macformula.github.io/racecar>.

## Edit the documentation

Instructions are available at <https://macformula.github.io/racecar/tutorials/site-dev>.

## Local viewing

In case the internet or website is down, you can still view the documentation locally.

With internet access, install the dependencies.

```bash
pip install uv
uv pip install -r requirements.txt
```

Then you can view the docs at any time with

```bash
$ uv run mkdocs serve
Serving on http://127.0.0.1:8000
```

Enter the printed address in your browser to see the docs.
