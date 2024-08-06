# The _racecar_ wiki

## Development

We use [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) to generate beautiful documentation without worrying about the intricacies of making a modern website. The content is written with Markdown which has a low learning curve for anyone not already familiar with it.

These links should get you started in writing content for the wiki.

- Generic Markdown
  <https://www.markdownguide.org/cheat-sheet/>
- Material for MkDocs formatting elements
  <https://squidfunk.github.io/mkdocs-material/reference/>

### Environment Setup

> These steps only need to be performed once.

1. Create a new Python virtual environment.

    > This is optional but recommended.

        python -m venv .venv
        source .venv/Scripts/activate

2. Install Material for MkDocs

        pip install mkdocs-material

### Editing and Viewing the Site

> Follow these steps each time you want to work on the wiki.

0. Activate the virtual environment (if you created one).

        source .venv/Scripts/activate

1. Launch an `mkdocs` server to locally host the site.

        $ cd racecar/wiki
        $ mkdocs serve
        ...
        INFO    -  [20:59:35] Serving on http://127.0.0.1:8000/
        ...

2. Open the listed URL in your web browser. You will see the webpage exactly as will be published later.

3. Edit the contents of the docs/ folder. Your locally hosted site will update whenever you edit __and save__ a relevant file.

### Publishing Changes

When you are satisfied with your changes, commit and make a pull request.

A Github action will automatically run when the PR is merged into `main` and the website will update within a few minutes.
