import tkinter as tk
from tkinter import ttk
from tkinter import filedialog, messagebox
# from data_utils import generate_csv
from pathlib import Path
import shutil
import neurokit2 as nk
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from tkcalendar import DateEntry
from datetime import timedelta
import os
from neurokit2.hrv.hrv_utils import _hrv_format_input
from neurokit2.hrv.intervals_utils import _intervals_successive

# Initialize the columns for the DataFrame
vns_df_cols = {
    'filename': [],
    'id': [],
    'date': [],
    'name': [],
    'age': [],
    'gender': [],
    'weight': [],
    'ecg_quality': [],
    'mean_hr': [],
    'mean_ibi': [],
    'rmssd': [],
    'lf/hf': [],
    'cvi': []
}

# Directory paths and constants
DATA_DIR = "./VNS_Monitor_Storage/"
SD_DIRNAME = "VNS_ECG_Data"
SAMPLE_RATE = 512
DAYS_DELTA = 7

# This is code poorly written due to a strict timeline and is by no means a measure of anyone's capabilities

# Main class to manage the VNS data
class VNSManager():
    def __init__(self, root):
        self.root = root
        self.elements = []
        root.title("VNS Monitor Data Manager")

        # Create info frame
        info_frame = tk.LabelFrame(root, text="", height=40)
        info_frame.pack(fill="x", padx=10, pady=10)

        info_var = tk.StringVar()
        self.info_var = info_var
        info_var.set("Welcome to VNS Monitor")
        info_label = tk.Label(info_frame, textvariable=info_var, anchor=tk.CENTER, pady=5, padx=5)
        info_label.pack(pady=5)

        # Create tree frame
        tree_frame = tk.Frame(root)
        tree_frame.pack(fill="both", expand=True, padx=10, pady=10)

        # Add a scrollbar to the tree frame
        tree_scroll = tk.Scrollbar(tree_frame)
        tree_scroll.pack(side=tk.RIGHT, fill='y')

        # Create the tree view
        my_tree = ttk.Treeview(tree_frame, yscrollcommand=tree_scroll.set, selectmode="extended")
        my_tree.pack(fill="both", expand=True)
        self.my_tree = my_tree

        # Configure the scrollbar
        tree_scroll.config(command=my_tree.yview)

        # Define columns for the tree view
        my_tree['columns'] = ("Filename", "ID", "Name", "Date")

        my_tree.column("#0", width=25, stretch=False)
        my_tree.column("Filename", anchor=tk.W, width=120)
        my_tree.column("ID", anchor=tk.W, width=80)
        my_tree.column("Name", anchor=tk.W, width=80)
        my_tree.column("Date", anchor=tk.W, width=80)

        my_tree.heading("#0", text="", anchor=tk.W)
        my_tree.heading("Filename", text="Filename", anchor=tk.W)
        my_tree.heading("ID", text="ID", anchor=tk.W)
        my_tree.heading("Name", text="Name", anchor=tk.W)
        my_tree.heading("Date", text="Date", anchor=tk.W)

        # Initialize control frame
        control_frame = tk.LabelFrame(root, text="Controls", height=100)
        control_frame.pack(fill="x", padx=10, pady=10)

        # Create buttons in the control frame
        load_btn = ttk.Button(control_frame, text='Load & Process Data', command=self.loadData)
        load_btn.pack(side=tk.LEFT, expand=True, padx=10, pady=10)
        self.elements.append(load_btn)

        export_btn = ttk.Button(control_frame, text='Export CSV', command=self.exportData)
        export_btn.pack(side=tk.LEFT, expand=True, padx=10, pady=10)
        self.elements.append(export_btn)

        average_btn = ttk.Button(control_frame, text="Average Trend", command=self.averageData)
        average_btn.pack(side=tk.LEFT, expand=True, padx=10, pady=10)
        self.elements.append(average_btn)

        # Initial setup
        self.disable_tree()
        self.process_data()
        self.populate_tree()
        self.enable_tree()

    # Method to populate the tree view with data
    def populate_tree(self):
        for idx, entry in self.vns_df.iterrows():
            user_id = entry['id']
            if not self.my_tree.exists(user_id):
                self.my_tree.insert(parent='', index='end', iid=user_id, values=(user_id, user_id, entry['name'], ''))
            self.my_tree.insert(parent=user_id, index='end', iid=entry['filename'], values=(entry['filename'], user_id, entry['name'], entry['date']))

        # Bind double-click event to the tree view items
        self.my_tree.bind("<Double-1>", self.onItemClick)

    # Method to clear the tree view
    def clear_tree(self):
        self.my_tree.delete(*self.my_tree.get_children())

    # Method to process data and update the display
    def process_data(self):
        self.info_var.set("Processing Data...")
        self.root.update_idletasks()
        self.vns_df, self.csv_path = generate_csv(DATA_DIR)
        self.info_var.set("Welcome to VNS Monitor")

    # Method to export data to CSV
    def exportData(self):
        csv_src = self.csv_path
        csv_dest = filedialog.asksaveasfilename(initialfile=("VNS_Monitor_data"+'.csv'), defaultextension='.csv')
        shutil.copy2(csv_src, csv_dest)

    # Method to show average data trends within a date range
    def averageData(self):
        average_popup = tk.Toplevel()
        tk.Label(average_popup, text="From Date").grid(row=0, column=0, padx=5, pady=5)
        from_date = DateEntry(average_popup, year=2024)
        from_date.grid(row=0, column=1, padx=5, pady=5)

        tk.Label(average_popup, text="To Date").grid(row=1, column=0, padx=5, pady=5)
        to_date = DateEntry(average_popup, year=2024)
        to_date.grid(row=1, column=1, padx=5, pady=5)

        show_btn = tk.Button(average_popup, text="Show Average Trend", command=lambda: self.showaverage(from_date.get_date(), to_date.get_date()))
        show_btn.grid(row=2, column=0, columnspan=2, padx=5, pady=5)
        
        return
    
    # Method to display average trends between two dates
    def showaverage(self, from_date, to_date):
        avg_dict = {'mean_ibi':[], 'rmssd':[], 'lf/hf':[], 'cvi':[], 'date':[]}
        avg_df = pd.DataFrame(avg_dict)
        date_d1 = from_date
        date_d2 = from_date + timedelta(days=DAYS_DELTA)
        idx = 0

        while (date_d2 <= to_date):
            filtered_df = self.vns_df[(self.vns_df['date'] >= str(date_d1)) & (self.vns_df['date'] <= str(date_d2))]
            avg_entry = {
                'mean_ibi': filtered_df.loc[:, 'mean_ibi'].mean(),
                'rmssd': filtered_df.loc[:, 'rmssd'].mean(),
                'lf/hf': filtered_df.loc[:, 'lf/hf'].mean(), 
                'cvi': filtered_df.loc[:, 'cvi'].mean(),
                'date': str(date_d1)
            }
            if avg_entry['mean_ibi'] == avg_entry['mean_ibi']: # NaN check
                print('adding')
                avg_df = pd.concat([avg_df, pd.DataFrame([avg_entry])], ignore_index=True)
            date_d1 = date_d2
            date_d2 = date_d1 + timedelta(days=DAYS_DELTA)
            idx += 1

        fig, axs = plt.subplots(2, 2)
        fig.suptitle(f"Average trend from {from_date} to {to_date}", fontweight="bold") 

        axs[0, 0].plot(avg_df['date'], avg_df['mean_ibi'])
        axs[0, 0].set_title('Mean IBI')
        axs[0, 0].set_ylabel("mean_ibi (ms)")

        axs[0, 1].plot(avg_df['date'], avg_df['rmssd'], color='tab:orange')
        axs[0, 1].set_title('RMSSD')
        axs[0, 1].set_ylabel("rmssd (ms)")

        axs[1, 0].plot(avg_df['date'], avg_df['lf/hf'], color='tab:green')
        axs[1, 0].set_title('LF/HF')
        axs[1, 0].set_ylabel('lf/hf')

        axs[1, 1].plot(avg_df['date'], avg_df['cvi'], color='tab:red')
        axs[1, 1].set_title('CVI')
        axs[1, 1].set_ylabel('cvi')

        fig.autofmt_xdate()
        plt.show()
        return

    # Method to load data from a selected directory
    def loadData(self):
        load_popup = tk.Toplevel()
        self.load_popup = load_popup
        self.load_popup.geometry("450x200")

        tk.Label(load_popup, text="ID").grid(row=0, column=0, padx=5, pady=5)
        id_entry = tk.Entry(load_popup)
        id_entry.grid(row=0, column=1, padx=5, pady=5)

        tk.Label(load_popup, text="Name").grid(row=1, column=0, padx=5, pady=5)
        name_entry = tk.Entry(load_popup)
        name_entry.grid(row=1, column=1, padx=5, pady=5)

        tk.Label(load_popup, text="Age").grid(row=2, column=0, padx=5, pady=5)
        age_entry = tk.Entry(load_popup)
        age_entry.grid(row=2, column=1, padx=5, pady=5)

        tk.Label(load_popup, text="Gender").grid(row=3, column=0, padx=5, pady=5)
        gender_var = tk.StringVar()
        gender_combo = ttk.Combobox(load_popup, textvariable=gender_var)
        gender_combo['values'] = ("Male", "Female", "Other")
        gender_combo.grid(row=3, column=1, padx=5, pady=5)

        tk.Label(load_popup, text="Weight (kg)").grid(row=4, column=0, padx=5, pady=5)
        weight_entry = tk.Entry(load_popup)
        weight_entry.grid(row=4, column=1, padx=5, pady=5)

        self.load_entries = {
            'id_entry': id_entry,
            'name_entry': name_entry,
            'age_entry': age_entry,
            'gender_combo': gender_combo,
            'weight_entry': weight_entry
        }

        select_button = tk.Button(load_popup, text="Select Data Folder", command=self.openFilePicker)
        select_button.grid(row=5, column=0, columnspan=2, padx=5, pady=5)

        save_button = tk.Button(load_popup, text="Save", command=self.saveUserDetails)
        save_button.grid(row=6, column=0, columnspan=2, padx=5, pady=5)
    
    # Method to open a file picker dialog
    def openFilePicker(self):
        dirname = filedialog.askdirectory(initialdir=Path(os.getcwd()))
        self.dirname = dirname
        return
    
    # Method to save user details and process the loaded data
    def saveUserDetails(self):
        entries = self.load_entries
        user_id = entries['id_entry'].get()
        user_name = entries['name_entry'].get()
        user_age = entries['age_entry'].get()
        user_gender = entries['gender_combo'].get()
        user_weight = entries['weight_entry'].get()

        if user_id == "" or user_name == "":
            messagebox.showinfo("Information", "ID and Name cannot be empty")
            return

        # Process user details
        input_details = {
            'user_id': user_id,
            'user_name': user_name,
            'user_age': user_age,
            'user_gender': user_gender,
            'user_weight': user_weight,
        }
        self.processUserData(self.dirname, input_details)
        self.load_popup.destroy()

    # Method to process user data
    def processUserData(self, folder_path, input_details):
        print(input_details)
        data_filenames = [f for f in os.listdir(folder_path) if f.endswith(".txt")]

        user_id = input_details['user_id']
        user_name = input_details['user_name']
        user_age = input_details['user_age']
        user_gender = input_details['user_gender']
        user_weight = input_details['user_weight']

        for filename in data_filenames:
            ecg_quality = 'Good'
            mean_hr = 0
            mean_ibi = 0
            rmssd = 0
            lfhf = 0
            cvi = 0

            # Read ECG data
            ecg_path = os.path.join(folder_path, filename)
            with open(ecg_path, 'r') as ecg_file:
                ecg_data = ecg_file.readlines()
            
            ecg_data = list(map(float, ecg_data))

            try:
                rpeaks, _ = nk.ecg_peaks(ecg_data, sampling_rate=SAMPLE_RATE)
                rpeaks = rpeaks['ECG_R_Peaks']
                intervals = _intervals_successive(rpeaks, sampling_rate=SAMPLE_RATE)

                mean_hr = 60 / np.mean(intervals)
                mean_ibi = np.mean(intervals) * 1000
                rmssd = np.sqrt(np.mean(np.diff(intervals) ** 2)) * 1000
                cvi = np.std(intervals) / np.mean(intervals) * 100
                lf, hf = nk.hrv_frequency(intervals, sampling_rate=SAMPLE_RATE)['LF'], nk.hrv_frequency(intervals, sampling_rate=SAMPLE_RATE)['HF']
                lfhf = lf / hf if hf != 0 else 0

                if len(rpeaks) < 10:
                    ecg_quality = 'Poor'

            except Exception as e:
                print(f"Error processing file {filename}: {e}")
                ecg_quality = 'Poor'

            # Append the data to the global DataFrame
            entry = {
                'filename': filename,
                'id': user_id,
                'name': user_name,
                'date': filename.split('_')[0],
                'age': user_age,
                'gender': user_gender,
                'weight': user_weight,
                'ecg_quality': ecg_quality,
                'mean_hr': mean_hr,
                'mean_ibi': mean_ibi,
                'rmssd': rmssd,
                'lf/hf': lfhf,
                'cvi': cvi
            }
            self.vns_df = self.vns_df.append(entry, ignore_index=True)
        self.clear_tree()
        self.populate_tree()
        return

    # Method to disable the tree view
    def disable_tree(self):
        self.my_tree['selectmode'] = "none"
        for elem in self.elements:
            elem.config(state="disabled")

    # Method to enable the tree view
    def enable_tree(self):
        self.my_tree['selectmode'] = "extended"
        for elem in self.elements:
            elem.config(state="normal")

    # Method to handle item clicks in the tree view
    def onItemClick(self, event):
        item = self.my_tree.selection()[0]
        parent_item = self.my_tree.parent(item)
        if parent_item == '':
            child_items = self.my_tree.get_children(item)
            for child in child_items:
                self.my_tree.detach(child)
            self.my_tree.delete(item)
        else:
            parent = self.my_tree.parent(item)
            self.my_tree.detach(item)
        self.root.update_idletasks()
        return

# Generate CSV from the VNS data
def generate_csv(data_dir):
    global vns_df_cols
    vns_df = pd.DataFrame(vns_df_cols)
    data_files = [f for f in os.listdir(data_dir) if f.endswith('.txt')]
    
    for data_file in data_files:
        data_path = os.path.join(data_dir, data_file)
        with open(data_path, 'r') as file:
            data = file.readlines()
        
        data = [float(d.strip()) for d in data]
        vns_df = vns_df.append({
            'filename': data_file,
            'id': data_file.split('_')[0],
            'date': data_file.split('_')[1].split('.')[0],
            'name': data_file.split('_')[2],
            'age': int(data_file.split('_')[3]),
            'gender': data_file.split('_')[4],
            'weight': float(data_file.split('_')[5]),
            'ecg_quality': 'Good' if len(data) > 10 else 'Poor',
            'mean_hr': np.mean(data),
            'mean_ibi': np.mean(data),
            'rmssd': np.sqrt(np.mean(np.diff(data) ** 2)),
            'lf/hf': np.mean(data) / np.std(data) if np.std(data) != 0 else 0,
            'cvi': np.std(data) / np.mean(data) * 100 if np.mean(data) != 0 else 0
        }, ignore_index=True)

    csv_path = os.path.join(data_dir, 'VNS_Monitor_data.csv')
    vns_df.to_csv(csv_path, index=False)
    return vns_df, csv_path

if __name__ == "__main__":
    root = tk.Tk()
    app = VNSManager(root)
    root.mainloop()
