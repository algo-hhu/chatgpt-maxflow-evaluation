#!/bin/bash

# Pfad zu dem Verzeichnis, das die Eingabedateien enthält
input_dir="/path/to/input/files"

# Pfad zu dem Programm solve_dimacs
# change ... to your path to /maxflow_algorithms-master/prd/bin/d_maxflow/solve_dimacs
program_path=".../maxflow_algorithms-master/prd/bin/d_maxflow/solve_dimacs"

# Extrahiere den Programmnamen aus dem Pfad
program_name=$(basename "$program_path")

# Pfad zu der Ausgabedatei (CSV-Datei)
#remove "/path/to/csv with ur desired destination"
output_csv="/path/to/csv/${program_name}_output.csv"

# Schreibe die Kopfzeile in die Ausgabedatei
echo "Run,Graph,Duration" > "$output_csv"

# Ein Durchlauf
run=1

for input_file in "$input_dir"/*.max; do
    if [ -f "$input_file" ]; then
        # Hier das Programm solve_dimacs mit dem Argument GT und der Eingabedatei aufrufen
        output=$("$program_path" GT "$input_file")
        
        # Extrahiere die Berechnungsdauer aus der Zeile mit "solve:"
        duration=$(echo "$output" | grep "solve:" | awk '{print $2}')
        
        # Extrahiere den Basisnamen der Eingabedatei (ohne Pfad und Erweiterung)
        base_name=$(basename "$input_file" .max)
        
        # Schreibe die Zeile in die Ausgabedatei
        echo "$run,$base_name,$duration" >> "$output_csv"
        
        # Erhöhe den Durchlaufzähler
        run=$((run + 1))
    fi
done

# Warte eine Sekunde
sleep 1

# Lösche alle Dateien, die auf .cut oder .sol enden, im input_dir
find "$input_dir" -type f \( -name "*.cut" -o -name "*.sol" \) -exec rm {} +

echo "Alle .cut und .sol Dateien im Verzeichnis $input_dir wurden gelöscht."

