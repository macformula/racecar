import json
import os
import sys

def generate_matrix(projects_dir):
    """
    Generates a matrix by recursively scanning for 'platforms' directories.
    """
    matrix = []

    # Walk through the projects directory
    for root, dirs, files in os.walk(projects_dir):
        if "platforms" in dirs:  # Check if 'platforms' directory exists
            project_name = os.path.relpath(root, projects_dir)  # Get the project name
            platforms_dir = os.path.join(root, "platforms")
            platforms = [
                platform for platform in os.listdir(platforms_dir)
                if os.path.isdir(os.path.join(platforms_dir, platform))
            ]
            for platform in platforms:
                print(f"Found platform: {platform} in project: {project_name}")
                matrix.append({"project": project_name, "platform": platform})

    # Remove linux/raspi/sil platforms and debug projects
    matrix = [item for item in matrix if item["platform"] not in ["linux", "raspi", "sil"]]
    matrix = [item for item in matrix if "debug" not in item["project"]]

    return matrix

if __name__ == "__main__":
    projects_dir = 'firmware/projects'
    if not os.path.isdir(projects_dir):
        print(f"Error: {projects_dir} is not a directory")
        sys.exit(1)

    matrix = generate_matrix(projects_dir)
    print(json.dumps(matrix, indent=4))

    # Save the matrix to a file
    with open("all_projects.json", "w") as f: json.dump(matrix, f)