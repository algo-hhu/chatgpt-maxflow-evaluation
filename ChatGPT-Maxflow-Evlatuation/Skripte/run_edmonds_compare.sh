#!/bin/bash

# Pfad zu dem Verzeichnis, das die Eingabedateien enthält
input_dir="path/to/input"

# Pfad zu dem Verzeichnis, das das Programm maxflow enthält
program_dir="path/to/Edmonds_Karp_Compare/maxflow-master"
program_path="$program_dir/maxflow"

# Pfad zu der Ausgabedatei (CSV-Datei)
output_csv="path/to/output/maxflow_output.csv"

# Schreibe die Kopfzeile in die Ausgabedatei
echo "Run,Graph,Duration" > "$output_csv"

# Ein Durchlauf
run=1

# Debugging information
echo "Input Directory: $input_dir"
echo "Program Path: $program_path"
echo "Output CSV: $output_csv"

for input_file in "$input_dir"/*.max; do
    if [ -f "$input_file" ]; then
        echo "Processing $input_file..."
        
        # Call the maxflow program with the input file
        output=$("$program_path" ek -f "$input_file")
        
        if [ $? -ne 0 ]; then
            echo "Error processing $input_file"
            continue
        fi
        
        # Extract duration from the output
        duration_ms=$(echo "$output" | grep "time solve:" | awk '{print $3}')
        
        # Convert duration from ms to seconds and format to 6 decimal places with a dot as the decimal separator
        duration_sec=$(LC_NUMERIC="en_US.UTF-8" awk "BEGIN {printf \"%.6f\", $duration_ms / 1000}")
        
        # Extract base name of the input file (without path and extension)
        base_name=$(basename "$input_file" .max)
        
        # Write the result to the output CSV
        echo "$run,$base_name,$duration_sec" >> "$output_csv"
        
        # Increment the run counter
        run=$((run + 1))
    else
        echo "No .max files found in $input_dir"
    fi
done

