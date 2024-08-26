"""
Generate a glossary markdown file to insert in the documentation.
"""

import re
import warnings
from pathlib import Path

import jinja2
import mkdocs_gen_files

CONFIG = mkdocs_gen_files.config
HERE = Path(__file__).parent
RACECAR = next(i for i in HERE.parents if i.name == "racecar")

file_glossary_input = "abbreviations.md"
file_template = "glossary.md.jinja"
file_glossary_output = "glossary.md"

# Parse glossary terms
term_pattern = re.compile(r"^\*\[(?P<term>.*?)\]: (?P<definition>.*)$")

glossary: list[dict] = []
with open(HERE / file_glossary_input, "r") as f:

    for idx, line in enumerate(f.readlines()):
        match = re.match(term_pattern, line)
        if match:
            glossary.append(match.groupdict())
        elif line != "":
            warnings.warn(
                f"Line {idx+1} of {file_glossary_input} is not a valid glossary term."
            )

# Render content with template.
env = jinja2.Environment(loader=jinja2.FileSystemLoader(HERE))
rendered = env.get_template(file_template).render(
    terms=glossary,
    gen_file=Path(__file__).relative_to(RACECAR).as_posix(),
)

# Export the markdown file.
with mkdocs_gen_files.open(file_glossary_output, "w") as f:
    f.write(rendered)
