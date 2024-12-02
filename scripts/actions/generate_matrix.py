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

    return matrix


if __name__ == "__main__":
    print("Generating matrix for projects")

    if len(sys.argv) < 2:
        print("Usage: python generate_matrix.py <projects_dir>")
        sys.exit(1)

    projects_dir = sys.argv[1]
    if not os.path.isdir(projects_dir):
        print(f"Error: {projects_dir} is not a directory")
        sys.exit(1)

    matrix = generate_matrix(projects_dir)
    print(json.dumps(matrix, indent=4))

    # Save the matrix to a file
    with open("matrix.json", "w") as f: json.dump(matrix, f)