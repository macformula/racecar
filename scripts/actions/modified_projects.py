# Usage: python modified_projects.py <base_git_branch>
# Usage: python3 modified_projects.py <base_git_branch>

import json
import os
import subprocess
import sys

def get_all_projects(projects_dir):
    all_projects = set()

    for root, dirs, _ in os.walk(projects_dir):
        if "platforms" in dirs:
            project_path = os.path.relpath(root, projects_dir)
            all_projects.add(project_path)

    return list(all_projects)

def get_modified_projects(projects_dir, projects, base_branch):
    modified_files = subprocess.check_output(
        ["git", "diff", "--name-only", base_branch, '--', projects_dir],
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

def get_project_platforms(projects_dir, project_dir):
    platforms_dir = os.path.join(projects_dir, project_dir, "platforms")
    return os.listdir(platforms_dir)

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

    projects_dir = "firmware/projects"
    base_branch = sys.argv[1]

    print(f"Base branch: {base_branch}")

    if not os.path.isdir(projects_dir):
        print(f"Error: {projects_dir} is not a directory")
        sys.exit(1)

    projects = get_all_projects(projects_dir)
    print(f"Projects: {projects}")

    modified_projects = get_modified_projects(projects_dir, projects, base_branch)
    modified_projects = filter_projects(modified_projects, ["debug"])
    print(f"Modified projects: {modified_projects}")

    matrix = []
    for project in modified_projects:
        platforms = get_project_platforms(projects_dir, project)
        platforms = filter_platforms(platforms, ["linux", "raspi", "sil"])

        for platform in platforms:
            matrix.append({"project": project, "platform": platform})

    print(json.dumps(matrix, indent=4))
    with open("modified_projects_matrix.json", "w") as f: json.dump(matrix, f)
