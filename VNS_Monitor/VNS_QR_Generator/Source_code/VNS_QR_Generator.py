import qrcode  # Import the qrcode library to generate QR codes
from qrcode.image.pure import PyPNGImage  # Import the image factory for QR code images
import tkinter as tk  # Import the tkinter library for GUI
from tkinter import ttk  # Import the themed widget set for tkinter
from tkinter import messagebox, filedialog  # Import messagebox and filedialog for dialog boxes
import os  # Import the os module for file path operations

# Function to generate the QR code
def generateQR(*args):
    try:
        # Extract and clean up user inputs
        t_user_id = ''.join(user_id.get().split())
        t_name = ''.join(name.get().split())
        t_age = ''.join(age.get().split())
        t_sex = ''.join(sex.get().split())
        t_weight = ''.join(weight.get().split())
        t_duration = ''.join(duration.get().split())

        # Check for mandatory fields
        if (t_user_id == "") or (t_name == "") or (t_duration == ""):
            messagebox.showerror(message="Error: Please fill in Name, ID & Duration")
            return
        
        # Ensure all values are alphanumeric
        for t in [t_user_id, t_name, t_age, t_sex, t_weight, t_duration]:
            if not t.isalnum():
                messagebox.showerror(message="All values must be alphanumeric")
                return
        
        # Generate QR code with the provided details
        qr = qrcode.make(f"https://192.168.42.1/?ID={t_user_id}&Name={t_name}&Duration={t_duration}&Age={t_age}&Weight={t_weight}&Gender={t_sex}", image_factory=PyPNGImage)
        filename = os.path.abspath(filedialog.asksaveasfilename(initialfile=(t_user_id+'.html'), defaultextension='.html'))
        img_filename = os.path.abspath(filename[:-5] + '_qr.png')
        qr.save(img_filename)

        # Create HTML content with user details and QR code
        html_code = f"""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>VNS Monitor</title>
            <style>
                body {{
                    font-family: Arial, sans-serif;
                    margin: 0;
                    padding: 40px;
                    background-color: #fff;
                    display: flex;
                    justify-content: space-between;
                    align-items: flex-start;
                    height: 100vh;
                    box-sizing: border-box;
                }}
                .details {{
                    flex: 1;
                }}
                .qr-code img {{
                    width: 300px;
                    height: 300px;
                }}
                .title {{
                    font-size: 32px;
                    font-weight: bold;
                    margin-bottom: 20px;
                }}
            </style>
        </head>
        <body>
            <div class="details">
                <p class="title">VNS Monitor</p>
                <p><strong>Name:</strong> {t_name}</p>
                <p><strong>ID:</strong> {t_user_id}</p>
                <p><strong>Age:</strong> {t_age}</p>
                <p><strong>Gender:</strong> {t_sex}</p>
                <p><strong>Weight:</strong> {t_weight}</p>
            </div>
            <div class="qr-code">
                <img src="{img_filename}" alt="QR Code">
            </div>
        </body>
        </html>
        """

        # Save the HTML file
        page = open(filename, 'w')
        page.write(html_code)
        page.close()

        messagebox.showinfo(message="User Form saved successfully")

    except ValueError:
        pass

# Set up the main tkinter window
root = tk.Tk()
root.title("VNS Monitor QR Code Generator")

# Create and configure the main frame
mainframe = tk.Frame(root)
mainframe.grid(column=0, row=0)
root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)

# Add labels and entry fields to the frame
ttk.Label(mainframe, text="ID:", anchor="e").grid(row=0, column=0)

user_id = tk.StringVar()
id_entry = ttk.Entry(mainframe, textvariable=user_id)
id_entry.grid(row=0, column=1)

ttk.Label(mainframe, text="Name:", anchor="e").grid(row=1, column=0)

name = tk.StringVar()
name_entry = ttk.Entry(mainframe, textvariable=name)
name_entry.grid(row=1, column=1)

ttk.Label(mainframe, text="Age:", anchor="e").grid(row=2, column=0)

age = tk.StringVar()
age_entry = ttk.Entry(mainframe, textvariable=age)
age_entry.grid(row=2, column=1)

ttk.Label(mainframe, text="Gender:", anchor="e").grid(row=3, column=0)

sex = tk.StringVar()
sex_entry = ttk.Entry(mainframe, textvariable=sex)
sex_entry.grid(row=3, column=1)

ttk.Label(mainframe, text="Weight:", anchor="e").grid(row=4, column=0)

weight = tk.StringVar()
weight_entry = ttk.Entry(mainframe, textvariable=weight)
weight_entry.grid(row=4, column=1)

ttk.Label(mainframe, text="Duration (s):", anchor="e").grid(row=5, column=0)

duration = tk.StringVar()
duration_entry = ttk.Entry(mainframe, textvariable=duration)
duration_entry.insert(0, "300")
duration_entry.grid(row=5, column=1)

# Add a button to save the QR code
ttk.Button(mainframe, text="Save QR", command=generateQR).grid(row=6, column=0, columnspan=2)

# Add padding to each child of the mainframe
for child in mainframe.winfo_children():
    child.grid_configure(padx=5, pady=5)

# Bind the Enter key to the generateQR function
root.bind("<Return>", generateQR)

# Start the main loop
root.mainloop()
