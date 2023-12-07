import csv
import subprocess

def read_csv_file(file_path):
    runs_delivered = []

    with open(file_path, 'r') as csvfile:
        reader = csv.reader(csvfile)
        next(reader)  # Skip the header line

        for row in reader:
            if not row[0].startswith("#"):  # Skip lines starting with "#"
                run_fill = row[0].split(':')[0]
                delivered = row[5]
                runs_delivered.append((run_fill, delivered))

    return runs_delivered


if __name__ == '__main__':

    file_path = '/eos/home-j/jlidrych/Tracker/CMSSW_12_6_4/src/2022_all.csv'
    data = read_csv_file(file_path)

    for run, delivered in data:
        command = f"cmsRun SiStripQualityStatistics_cfg.py runNumber={run} globalTag=124X_dataRun3_Prompt_v10"
        print(f"{command}")
        # subprocess.run(command, shell=True)

