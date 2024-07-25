#!/bin/bash

# Pfad zu dem Verzeichnis, das die Eingabedateien enthält
input_dir="path/to/testCases"

# Pfad zum Ausgabeverzeichnis
output_dir="location/where/csv/should/be/saved"



# Liste der Programmpfade
programs=(
    "path/to/program1"
    "path/to/program2"  
)


# Iteriere über jedes Programm
for program_path in "${programs[@]}"; do
    # Extrahiere den Programmnamen aus dem Pfad
    program_name=$(basename "$program_path")

    # Pfad zu der Ausgabedatei (CSV-Datei)
    output_csv="${output_dir}/${program_name}_all_results.csv"

    # Schreibe die Kopfzeile in die Ausgabedatei
    header="Run,Graph,Duration"
    echo "$header" > "$output_csv"

    # Ein Durchlauf
    run=1

    # Iteriere über jede Eingabedatei
    for input_file in "$input_dir"/*.max; do
        if [ -f "$input_file" ]; then
            # Extrahiere den Basisnamen der Eingabedatei
            base_name=$(basename "$input_file" .max)
            
            # Hier das Programm mit der Eingabedatei aufrufen
            output=$("$program_path" "$input_file")
            
            # Extrahiere die Berechnungsdauer aus der Zeile mit "Berechnungsdauer:"
            duration=$(echo "$output" | grep "Berechnungsdauer:" | awk '{print $2}')
            
            row="$run,$base_name,$duration"
            
            # Schreibe die aktuelle Zeile in die CSV-Datei
            echo "$row" >> "$output_csv"
            
            # Gib das Zwischenergebnis auf der Konsole aus
            echo "Run: $run, Graph: $base_name, Duration: $duration"
            
            run=$((run + 1))
        fi
    done
done

