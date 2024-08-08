# Editing this site

We use [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) to generate beautiful documentation without worrying about the intricacies of making a modern website. The content is written with Markdown which has a low learning curve for anyone not already familiar with it.

The site is hosted with [GitHub Pages](https://docs.github.com/en/pages/quickstart).

## Environment Setup

These steps only need to be performed once.

1. Clone the racecar repository.

        git clone https://github.com/macformula/racecar.git

1. Create a Python [virtual environment](https://docs.python.org/3/library/venv.html).

    > This step is optional but recommended.

        cd racecar/website
        python -m venv .venv

    Activate the virtual environment.

        .venv\Scripts\activate          # Windows
        source .venv/Scripts/activate   # Unix

1. Install Material for MkDocs.

        pip install mkdocs-material

## Editing and Viewing the Site

> Follow these steps each time you want to edit the site.
>
> This section assumes that you have already activated your virtual environment (see above).

1. Launch an `mkdocs` server to locally host the site.

        $ cd racecar/website
        $ mkdocs serve
        ...
        INFO    -  [20:59:35] Serving on http://127.0.0.1:8000/
        ...

1. Open the listed URL in your web browser. You will see the webpage exactly as will be published later.

    > In this example, open <http://127.0.0.1:8000> in a browser.

1. Edit the contents of the docs/ folder. Your locally hosted site will update whenever you edit __and save__ a relevant file.

    !!! info "Markdown References"

        Generic Markdown
        <https://www.markdownguide.org/cheat-sheet/>
        
        Material for MkDocs
        <https://squidfunk.github.io/mkdocs-material/reference/>

## Publishing Changes

When you are satisfied with your changes, commit and make a pull request.

A Github action will automatically re-build the website when the PR is merged into `main`. The build artifacts are pushed to the `ghpages` branch of the repository from which the website is hosted. The website will update within a few minutes.

!!! info ""

    The action is specified in `racecar/.github/workflows/deploy-ghpages.yml` and was nearly copied from <https://squidfunk.github.io/mkdocs-material/publishing-your-site/>.

!!! warning "The `ghpages` branch"

    The action force-pushes build artifacts to the `ghpages` branch, so any changes made on this branch will be overwritten. Edit the contents of `racecar/website` to modify the site.
