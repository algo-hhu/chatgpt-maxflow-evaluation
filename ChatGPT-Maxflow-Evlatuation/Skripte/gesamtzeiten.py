import os
import pandas as pd

# Pfad zum Ordner mit den CSV-Dateien
folder_path = '/home/josch/Schreibtisch/Bachelorarbeit/GraphGenerator/bank/Bench'

# Liste aller CSV-Dateien im Ordner
csv_files = [f for f in os.listdir(folder_path) if f.endswith('.csv')]

# Initialisiere eine leere Liste für die Ergebnisse
results = []

# Lese jede CSV-Datei ein, summiere die Duration-Werte und füge die Ergebnisse zur Liste hinzu
for csv_file in csv_files:
    file_path = os.path.join(folder_path, csv_file)
    df = pd.read_csv(file_path)
    total_duration = df['Duration'].sum()
    results.append({'File': csv_file, 'Total Duration': total_duration})

# Erstelle einen DataFrame aus den Ergebnissen
result_df = pd.DataFrame(results)

# Speichere die Ergebnisse in einer neuen CSV-Datei im gleichen Ordner
output_file_path = os.path.join(folder_path, 'summed_durations.csv')
result_df.to_csv(output_file_path, index=False)

print(f"Die summierten Berechnungszeiten wurden in '{output_file_path}' gespeichert.")

