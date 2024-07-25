import os
import subprocess

"""
This script processes .max files to verify the computed maximum flow values using various algorithms.
You can specify the folder containing the .max files and the paths to the executable programs
that should be used to calculate the maximum flow. The script compares the computed values 
with the expected values specified within the .max files and outputs the results.

Steps:
1. Define the folder containing the .max files.
2. Define the paths to the executable programs.
3. The script will read each .max file, run the specified programs on these files, 
   and compare the computed maximum flow values with the expected values.
4. The results, including computation time and discrepancies, are printed for each program.
"""

def extract_max_flow_from_file(file_path):
    """
    Extract the maximum flow value from the .max file.
    """
    with open(file_path, 'r') as f:
        for line in f:
            if line.startswith('c Maximum flow:'):
                return int(line.split(':')[1].strip())
    return None

def get_short_program_path(executable_path):
    """
    Get the last three parts of the program path.
    """
    return '/'.join(executable_path.split('/')[-4:])

def call_program_and_check(file_path, executable_path):
    """
    Call the program with the given .max file and check the maximum flow value.
    """
    expected_max_flow = extract_max_flow_from_file(file_path)
    if expected_max_flow is None:
        print(f"No maximum flow value found in {file_path}")
        return False, None, None, None

    result = subprocess.run([executable_path, file_path], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running the program on {file_path}")
        print(result.stderr)
        return False, None, None, None

    output_lines = result.stdout.splitlines()
    computed_max_flow = None
    computation_time = None

    for line in output_lines:
        if line.startswith("Maximaler Fluss:"):
            computed_max_flow = int(line.split(':')[1].strip())
        elif line.startswith("Berechnungsdauer:"):
            computation_time = line.split(':')[1].strip()

    if computed_max_flow is None:
        print(f"No computed maximum flow found in the output for {file_path}")
        return False, None, None, None

    return computed_max_flow == expected_max_flow, expected_max_flow, computed_max_flow, computation_time

def main():
    # Variable for the folder path containing .max files
    folder_path = "/path/to/your/max/files"

    # List of executable programs to be called
    executable_paths = [
        "path/to/your/program1",
        "path/to/your/program2",
        # Add more programs here
    ]

    max_files = sorted([os.path.join(folder_path, f) for f in os.listdir(folder_path) if f.endswith('.max')])  # Sort .max files alphabetically

    for executable_path in executable_paths:
        short_program_path = get_short_program_path(executable_path)
        print(f"\nRunning {short_program_path}...")
        incorrect_files = []

        for file_path in max_files:
            correct, expected_max_flow, computed_max_flow, computation_time = call_program_and_check(file_path, executable_path)
            if not correct:
                incorrect_files.append((file_path, expected_max_flow, computed_max_flow))
            else:
                print(f"{file_path}: computation time: {computation_time} seconds by {short_program_path}")

        if incorrect_files:
            print(f"\nFiles with incorrect maximum flow values for {short_program_path}:")
            for file_path, expected_max_flow, computed_max_flow in incorrect_files:
                print(f" - {file_path}: correct value: {expected_max_flow}, calculated value: {computed_max_flow}")
        else:
            print(f"\nAll files have correct maximum flow values for {short_program_path}.")

if __name__ == "__main__":
    main()

