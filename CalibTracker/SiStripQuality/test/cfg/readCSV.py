import csv
import subprocess
import numpy as np
import uproot

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

    file_path = '2022_all.csv'
    data = read_csv_file(file_path)

    output_info = []
    print("test: ",len(data))

    # Create a dictionary to store the data
    tree_data = {'run_fill': [], 'delivered': []}

    for i, (run_value, delivered_value) in enumerate(data, start=1):
        command = f"cmsRun SiStripQualityStatistics_cfg.py runNumber={run_value} globalTag=124X_dataRun3_Prompt_v10"
        print(f"{command}")
        output = subprocess.check_output(command, shell=True, universal_newlines=True)

        # Fill the TTree with data
        tree_data['run_fill'].append(run_value)
        tree_data['delivered'].append(delivered_value)

        lines = output.split('\n')
        fourth_line = lines[3]
        fourth_part = fourth_line.split()
        sixth_line = lines[5]
        sixth_part = sixth_line.split()
        eight_line = lines[7]
        eight_part = eight_line.split()
        tenth_line = lines[9]
        tenth_part = tenth_line.split()
        output_info.append((run_value,delivered_value,fourth_part[0],fourth_part[1],fourth_part[2],fourth_part[3],sixth_part[0],sixth_part[1],sixth_part[2],sixth_part[3],eight_part[0],eight_part[1],tenth_part[0],tenth_part[1]))
        print(f"{i}/{len(data)} done")


#    array_data = np.array(data, dtype=[('run_fill', np.uint64), ('delivered', np.float64)])
    array_output_info = np.array(output_info, dtype=[('run_fill', np.uint64),('delivered', np.float64),('uncool0',np.uint64),('uncool1',np.uint64),('uncool2',np.uint64),('uncool3',np.uint64),('ref0',np.uint64),('ref1',np.uint64),('ref2',np.uint64),('ref3',np.uint64),('uncoolBAD',np.uint64), ('uncoolTOTAL',np.uint64),('refBAD',np.uint64),('refTOTAL',np.uint64)])
    # Create a ROOT file
    root_file_path = 'data.root'
    root_file = uproot.recreate(root_file_path)

    # Create a TTree and write the data
    root_file['modules'] = array_output_info
    root_file.close()
