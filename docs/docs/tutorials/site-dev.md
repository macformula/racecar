# :material-pencil-plus: Editing this site

We use [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) to generate beautiful documentation without worrying about the intricacies of modern web development. The content is written with Markdown which has a low learning curve for anyone not already familiar with it.

The site is hosted with [GitHub Pages](https://docs.github.com/en/pages/quickstart).

## Environment Setup

These steps only need to be performed once.

1. Clone the racecar repository.

        git clone https://github.com/macformula/racecar.git

1. Create a Python [virtual environment](https://docs.python.org/3/library/venv.html).

    > This step is optional but recommended.

        cd racecar/docs
        python -m venv .venv

    Activate the virtual environment.<a name="activate_venv"></a>

    === "Windows"

            .venv\Scripts\activate

    === "Unix"

            source .venv/Scripts/activate

1. Install the documentation dependencies.

        pip install -r requirements.txt

## Editing and Viewing the Site

> Follow these steps each time you want to edit the site.
>
> This section assumes that you have already [activated](#activate_venv) your virtual environment, if applicable.

1. Launch an `mkdocs` server to locally host the site.

        $ cd racecar/docs
        $ mkdocs serve
        ...
        INFO    -  [22:01:58] Serving on http://127.0.0.1:8000/

2. Open the listed URL in your web browser.

    > In this example, open <http://127.0.0.1:8000/> in a browser.

3. Edit the contents of the `docs/` folder. Your locally hosted site will update whenever you edit __and save__ a relevant file.

    !!! info "Markdown References"

        Generic Markdown
        <https://www.markdownguide.org/cheat-sheet/>
        
        Material for MkDocs
        <https://squidfunk.github.io/mkdocs-material/reference/>

## Publishing Changes

When you are satisfied with your changes, commit and make a pull request.

A Github action will automatically re-build the website when the PR is merged into `main`. The build artefacts are pushed to the `ghpages` branch of the repository from which the website is hosted. The website will update within a few minutes.

!!! info ""

    The action is specified in `racecar/.github/workflows/deploy-ghpages.yml` and was copied from <https://squidfunk.github.io/mkdocs-material/publishing-your-site/>.

!!! warning "The `ghpages` branch"

    The action force-pushes build artefacts to the `ghpages` branch, so any changes made on this branch will be overwritten. Edit the contents of `racecar/docs` to modify the site.
