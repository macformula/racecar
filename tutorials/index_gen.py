# Generate an index file with links to all tutorials

import os

import jinja2
import mkdocs_gen_files
from mkdocs.structure.files import File, Files

CONFIG = mkdocs_gen_files.config
OUTPUT = "tutorials/index.md"


def get_title(file: File) -> str:
    """Get the text content of the (first) # Header"""
    prefix = "# "  # note the trailing space to avoid matching ## and ###
    title_line = next(
        filter(lambda i: i.startswith(prefix), file.content_string.split("\n"))
    )
    return title_line.lstrip(prefix)


# Make a copy of the list to avoid editing the actual NAV
tutorial_files = next(d["Tutorials"] for d in CONFIG["nav"] if "Tutorials" in d).copy()
tutorial_files.remove(OUTPUT)

articles = [
    {
        "title": get_title(f),
        "filename": os.path.relpath(f.src_uri, os.path.dirname(OUTPUT)),
    }
    for f in Files(File(i, CONFIG["docs_dir"], None, True) for i in tutorial_files)
]

# Load and render template
env = jinja2.Environment(loader=jinja2.FileSystemLoader(CONFIG["docs_dir"]))
template = env.get_template(f"{OUTPUT}.jinja")
content = template.render(articles=articles)

# Save generated file
with mkdocs_gen_files.open("tutorials/index.md", "w") as f:
    print(content, file=f)
