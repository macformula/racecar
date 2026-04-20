# This file is used to generate a matrix of projects and platforms for GitHub Actions
# The matrix is used to build all projects its respective platforms
# The output is two JSON files: all_projects_matrix.json and modified_projects_matrix.json
# The all_projects_matrix.json contains all projects and platforms
# The modified_projects_matrix.json contains only the modified projects and platforms from the base branch
# The format of the JSON files are as follows: [ { "project": "project_name", "platform": "platform_name" }, ... ]

# Usage: python3 get_projects_matrix.py <base_git_branch>
# Example: python3 get_projects_matrix.py main

import json
import os
import subprocess
import sys

# Global variables
projects_dir = "firmware/projects"
filter_projects_list = ["debug"]
filter_platforms_list = ["linux", "raspi", "sil"]

# Get all projects in projects directory with a "platforms" subdirectory
def get_all_projects(projects_dir):
    all_projects = set()

    for root, dirs, _ in os.walk(projects_dir):
        if "platforms" in dirs:
            project_path = os.path.relpath(root, projects_dir)
            all_projects.add(project_path)

    return list(all_projects)

# Get projects that have been modified compared to the base branch
def get_modified_projects(projects_dir, projects, base_branch):
    # subprocess.run(["git", "fetch", "origin", base_branch])

    modified_files = subprocess.check_output(
        ["git", "diff", "--name-only", f"origin/{base_branch}", '--', projects_dir],
        universal_newlines=True
    ).splitlines()

    modified_projects = set()
    for modified_file in modified_files:
        for project in projects:
            if project in modified_file:
                modified_projects.add(project)
                break

    modified_projects = list(modified_projects)
    return modified_projects

# Search for directories in the "platforms" subdirectory of a project
def get_project_platforms(projects_dir, project_dir):
    platforms_dir = os.path.join(projects_dir, project_dir, "platforms")
    return os.listdir(platforms_dir)

# Format projects and platforms into a list for GitHub Actions matrix
def create_matrix(projects):
    matrix = []
    for project in projects:
        platforms = get_project_platforms(projects_dir, project)
        platforms = filter_platforms(platforms, filter_platforms_list)

        for platform in platforms:
            matrix.append({"project": project, "platform": platform})

    return matrix

# Remove projects that contain any of the items in the remove_list
def filter_projects(projects, remove_list):
    return [project for project in projects if not any(item in project for item in remove_list)]

# Remove platforms that match any of the items in the remove_list
def filter_platforms(platforms, remove_list):
    return [platform for platform in platforms if platform not in remove_list]

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python modified_projects.py <base_branch>")
        sys.exit(1)

    base_branch = sys.argv[1]
    print(f"Base branch: {base_branch}")

    if not os.path.isdir(projects_dir):
        print(f"Error: {projects_dir} is not a directory")
        sys.exit(1)

    projects = get_all_projects(projects_dir)
    projects = filter_projects(projects, filter_projects_list)
    print(f"All projects: {projects}")

    modified_projects = get_modified_projects(projects_dir, projects, base_branch)
    print(f"Modified projects: {modified_projects}")

    all_projects_matrix = create_matrix(projects)
    modified_projects_matrix = create_matrix(modified_projects)

    print(f"All projects matrix: {json.dumps(all_projects_matrix, indent=4)}")
    print(f"Modified projects matrix: {json.dumps(modified_projects_matrix, indent=4)}")

    with open("all_projects_matrix.json", "w") as f: json.dump(all_projects_matrix, f)
    with open("modified_projects_matrix.json", "w") as f: json.dump(modified_projects_matrix, f)
