import ctypes
from ctypes import c_char_p, c_uint16, c_void_p, POINTER
import tkinter as tk
from tkinter import filedialog, simpledialog, messagebox, ttk
import threading
import os
from pathlib import Path

# Load your C library
try:
    lib = ctypes.CDLL("librcex.so")
except OSError:
    messagebox.showerror("Error", "librcex.so not found. Please ensure library is in current directory.")
    exit(1)

# Define function signatures
lib.rcexinit.argtypes = [c_char_p, c_uint16]
lib.rcexinit.restype = ctypes.c_void_p

lib.rcexencrypt.argtypes = [ctypes.c_void_p, c_char_p, c_uint16]
lib.rcexencrypt.restype = POINTER(ctypes.c_ubyte)

# rcexwipe() zeroes the context's key schedule and frees it - use this
# instead of a raw free() so no key material is left sitting in memory.
lib.rcexwipe.argtypes = [c_void_p]
lib.rcexwipe.restype = None

# libc handle, used only to free the per-chunk ciphertext buffers that
# rcexencrypt() mallocs - those are separate from the Rcex context itself.
libc = ctypes.CDLL(None)
libc.free.argtypes = [c_void_p]
libc.free.restype = None

class ModernRCEXApp:
    def __init__(self, root):
        self.root = root
        self.mode = None  # Will be determined by input file
        self.setup_window()
        self.setup_styles()
        self.create_widgets()
        
    def setup_window(self):
        self.root.title("🔐 RCEX File Processor - Encryption & Decryption")
        self.root.geometry("600x500")
        self.root.minsize(500, 400)
        
        # Center window on screen
        self.root.update_idletasks()
        x = (self.root.winfo_screenwidth() // 2) - (600 // 2)
        y = (self.root.winfo_screenheight() // 2) - (500 // 2)
        self.root.geometry(f"600x500+{x}+{y}")
        
        # Configure grid weights
        self.root.grid_rowconfigure(0, weight=1)
        self.root.grid_columnconfigure(0, weight=1)
        
    def setup_styles(self):
        self.root.configure(bg='#1e1e1e')
        
        # Configure ttk styles
        style = ttk.Style()
        style.theme_use('clam')
        
        # Configure button style
        style.configure('Modern.TButton',
                       background='#4CAF50',
                       foreground='white',
                       borderwidth=0,
                       focuscolor='none',
                       font=('Segoe UI', 11, 'bold'))
        style.map('Modern.TButton',
                 background=[('active', '#45a049')])
        
        # Configure frame style
        style.configure('Modern.TFrame',
                       background='#2d2d2d',
                       relief='flat',
                       borderwidth=1)
        
        # Configure label style
        style.configure('Modern.TLabel',
                       background='#1e1e1e',
                       foreground='#ffffff',
                       font=('Segoe UI', 10))
        
        # Configure entry style
        style.configure('Modern.TEntry',
                       fieldbackground='#3d3d3d',
                       foreground='#ffffff',
                       borderwidth=1,
                       insertcolor='#ffffff')
        
    def create_widgets(self):
        # Main container
        main_frame = ttk.Frame(self.root, style='Modern.TFrame')
        main_frame.grid(row=0, column=0, padx=20, pady=20, sticky='nsew')
        main_frame.grid_rowconfigure(1, weight=1)
        main_frame.grid_columnconfigure(0, weight=1)
        
        # Title
        self.title_label = tk.Label(main_frame, 
                              text="🔐 RCEX File Processor",
                              font=('Segoe UI', 24, 'bold'),
                              bg='#1e1e1e',
                              fg='#4CAF50')
        self.title_label.grid(row=0, column=0, pady=(0, 10))
        
        # Subtitle
        self.subtitle_label = tk.Label(main_frame,
                                     text="Automatic Encryption & Decryption",
                                     font=('Segoe UI', 12),
                                     bg='#1e1e1e',
                                     fg='#b0b0b0')
        self.subtitle_label.grid(row=1, column=0, pady=(0, 20))
        
        # File selection frame
        file_frame = ttk.Frame(main_frame, style='Modern.TFrame')
        file_frame.grid(row=2, column=0, pady=10, sticky='ew', padx=20)
        file_frame.grid_columnconfigure(1, weight=1)
        
        # File input
        self.file_label = tk.Label(file_frame, text="📁 Input File:", 
                                   font=('Segoe UI', 11, 'bold'),
                                   bg='#1e1e1e', fg='#ffffff')
        self.file_label.grid(row=0, column=0, sticky='w', pady=5)
        
        self.file_var = tk.StringVar()
        self.file_entry = tk.Entry(file_frame, textvariable=self.file_var,
                                  font=('Segoe UI', 10),
                                  bg='#3d3d3d', fg='#ffffff',
                                  insertbackground='#ffffff',
                                  relief='flat', bd=5)
        self.file_entry.grid(row=0, column=1, sticky='ew', padx=(10, 10), pady=5)
        
        self.browse_btn = tk.Button(file_frame, text="Browse",
                                   command=self.browse_file,
                                   font=('Segoe UI', 9),
                                   bg='#2196F3', fg='white',
                                   relief='flat', bd=0,
                                   activebackground='#1976D2')
        self.browse_btn.grid(row=0, column=2, pady=5)
        
        # Key input
        tk.Label(file_frame, text="🔑 Encryption Key:",
                font=('Segoe UI', 11, 'bold'),
                bg='#1e1e1e', fg='#ffffff').grid(row=1, column=0, sticky='w', pady=5)
        
        self.key_var = tk.StringVar()
        self.key_entry = tk.Entry(file_frame, textvariable=self.key_var,
                                 font=('Segoe UI', 10),
                                 bg='#3d3d3d', fg='#ffffff',
                                 insertbackground='#ffffff',
                                 relief='flat', bd=5, show='*')
        self.key_entry.grid(row=1, column=1, sticky='ew', padx=(10, 10), pady=5)
        
        self.show_key_btn = tk.Button(file_frame, text="👁",
                                     command=self.toggle_key_visibility,
                                     font=('Segoe UI', 9),
                                     bg='#607D8B', fg='white',
                                     relief='flat', bd=0,
                                     activebackground='#455A64')
        self.show_key_btn.grid(row=1, column=2, pady=5)
        
        # Output file
        self.output_label = tk.Label(file_frame, text="💾 Output File:",
                                    font=('Segoe UI', 11, 'bold'),
                                    bg='#1e1e1e', fg='#ffffff')
        self.output_label.grid(row=2, column=0, sticky='w', pady=5)
        
        self.output_var = tk.StringVar()
        self.output_entry = tk.Entry(file_frame, textvariable=self.output_var,
                                    font=('Segoe UI', 10),
                                    bg='#3d3d3d', fg='#ffffff',
                                    insertbackground='#ffffff',
                                    relief='flat', bd=5)
        self.output_entry.grid(row=2, column=1, sticky='ew', padx=(10, 10), pady=5)
        
        self.save_btn = tk.Button(file_frame, text="Save As",
                                command=self.browse_output,
                                font=('Segoe UI', 9),
                                bg='#2196F3', fg='white',
                                relief='flat', bd=0,
                                activebackground='#1976D2')
        self.save_btn.grid(row=2, column=2, pady=5)
        
        # Mode indicator
        self.mode_label = tk.Label(main_frame, 
                                 text="",
                                 font=('Segoe UI', 11, 'bold'),
                                 bg='#1e1e1e',
                                 fg='#FF9800')
        self.mode_label.grid(row=3, column=0, pady=10)
        
        # Progress bar
        self.progress_var = tk.DoubleVar()
        self.progress_bar = ttk.Progressbar(main_frame, 
                                           variable=self.progress_var,
                                           maximum=100,
                                           style='Modern.Horizontal.TProgressbar')
        self.progress_bar.grid(row=4, column=0, sticky='ew', padx=20, pady=(20, 10))
        
        # Status label
        self.status_var = tk.StringVar(value="Select a file to begin processing")
        self.status_label = tk.Label(main_frame, textvariable=self.status_var,
                                   font=('Segoe UI', 10),
                                   bg='#1e1e1e',
                                   fg='#4CAF50')
        self.status_label.grid(row=5, column=0, pady=5)
        
        # Action button
        self.action_btn = tk.Button(main_frame, text="🚀 Process File",
                                   command=self.process_file_threaded,
                                   font=('Segoe UI', 14, 'bold'),
                                   bg='#4CAF50', fg='white',
                                   relief='flat', bd=0,
                                   activebackground='#45a049',
                                   padx=30, pady=15)
        self.action_btn.grid(row=6, column=0, pady=20)
        
        # Info frame
        info_frame = ttk.Frame(main_frame, style='Modern.TFrame')
        info_frame.grid(row=7, column=0, sticky='ew', padx=20, pady=(10, 0))
        
        self.info_text = "ℹ️  Select any file (including .enc files), enter encryption key, and choose where to save result. The app will automatically detect whether to encrypt or decrypt."
        self.info_label = tk.Label(info_frame, text=self.info_text,
                                  font=('Segoe UI', 9),
                                  bg='#2d2d2d', fg='#b0b0b0',
                                  wraplength=500,
                                  justify='left')
        self.info_label.pack(pady=10)
        
    def browse_file(self):
        # Accept all file types including .enc files
        filetypes = [
            ("All files", "*.*"),
            ("Encrypted files", "*.enc"),
            ("Text files", "*.txt"),
            ("Documents", "*.docx *.pdf"),
            ("Images", "*.jpg *.png *.gif *.bmp")
        ]
            
        filename = filedialog.askopenfilename(
            title="Select file to process",
            filetypes=filetypes
        )
        if filename:
            self.file_var.set(filename)
            self.detect_mode_and_update_ui()
            
    def detect_mode_and_update_ui(self):
        """Detect if we should encrypt or decrypt based on file extension"""
        if not self.file_var.get():
            return
            
        file_path = Path(self.file_var.get())
        
        if file_path.suffix.lower() == '.enc':
            self.mode = "decrypt"
            mode_text = "🔓 Decryption Mode"
            mode_color = '#FF9800'
            button_text = "🔓 Decrypt File"
            button_color = '#FF9800'
            button_active = '#F57C00'
            # Suggest output filename without .enc extension
            suggested_output = str(file_path.parent / f"{file_path.stem}_decrypted")
        else:
            self.mode = "encrypt"
            mode_text = "🔒 Encryption Mode"
            mode_color = '#4CAF50'
            button_text = "🔒 Encrypt File"
            button_color = '#4CAF50'
            button_active = '#45a049'
            # Suggest output filename with .enc extension
            suggested_output = str(file_path.parent / f"{file_path.stem}.enc")
        
        # Update UI elements
        self.mode_label.config(text=mode_text, fg=mode_color)
        self.action_btn.config(text=button_text, bg=button_color, activebackground=button_active)
        
        # Auto-suggest output filename if user hasn't entered one
        if not self.output_var.get():
            self.output_var.set(suggested_output)
            
        self.status_var.set(f"Ready to {self.mode} file")
        
    def browse_output(self):
        if self.mode == "encrypt":
            default_ext = ".enc"
            filetypes = [("Encrypted files", "*.enc"), ("All files", "*.*")]
        else:
            default_ext = ""
            filetypes = [("All files", "*.*"), ("Text files", "*.txt"), ("Documents", "*.docx *.pdf")]
            
        filename = filedialog.asksaveasfilename(
            title=f"Save processed file as",
            defaultextension=default_ext,
            filetypes=filetypes
        )
        if filename:
            self.output_var.set(filename)
            
    def toggle_key_visibility(self):
        if self.key_entry.cget('show') == '*':
            self.key_entry.config(show='')
            self.show_key_btn.config(text='🙈')
        else:
            self.key_entry.config(show='*')
            self.show_key_btn.config(text='👁')
            
    def process_file_threaded(self):
        if not self.validate_inputs():
            return
            
        self.action_btn.config(state='disabled', bg='#757575')
        self.progress_var.set(0)
        self.status_var.set(f"Starting {self.mode}ion...")
        
        # Run processing in separate thread
        thread = threading.Thread(target=self.process_file)
        thread.daemon = True
        thread.start()
        
    def validate_inputs(self):
        if not self.file_var.get():
            messagebox.showerror("Error", "Please select an input file.")
            return False
        if not self.key_var.get():
            messagebox.showerror("Error", "Please enter an encryption key.")
            return False
        if not self.output_var.get():
            messagebox.showerror("Error", "Please specify an output file.")
            return False
        if not os.path.exists(self.file_var.get()):
            messagebox.showerror("Error", "Input file does not exist.")
            return False
        if self.mode is None:
            messagebox.showerror("Error", "Please select a file first to detect processing mode.")
            return False
        return True
        
    def process_file(self):
        try:
            infile = self.file_var.get()
            key = self.key_var.get()
            outfile = self.output_var.get()
            
            self.root.after(100, lambda: self.status_var.set(f"Initializing {self.mode}ion context..."))
            self.root.after(100, lambda: self.progress_var.set(10))
            
            # Fixed: use byte length, not character count - a non-ASCII key
            # encodes to more bytes than len(key), which threw the key schedule off.
            key_bytes = key.encode()
            ctx = lib.rcexinit(key_bytes, len(key_bytes))
            
            self.root.after(100, lambda: self.status_var.set("Processing input file..."))
            self.root.after(100, lambda: self.progress_var.set(30))
            
            buffer_size = 4096
            total_size = os.path.getsize(infile)
            processed_size = 0
            
            with open(infile, "rb") as f_in:
                with open(outfile, "wb") as f_out:
                    while True:
                        chunk = f_in.read(buffer_size)
                        if not chunk:
                            break
                        
                        chunk_size = len(chunk)
                        
                        # Fixed: pass the bytes straight through as c_char_p.
                        # The old code round-tripped through a (c_char_p*1) array
                        # and read it back, which re-reads the pointer as a
                        # NUL-terminated C string and silently truncates any
                        # chunk containing a 0x00 byte - i.e. most binary files.
                        processed_chunk = lib.rcexencrypt(ctx, chunk, chunk_size)
                        
                        # Write processed data
                        processed_bytes = bytearray(processed_chunk[:chunk_size])
                        f_out.write(processed_bytes)
                        
                        # Fixed: rcexencrypt() mallocs a fresh buffer every call
                        # and it was never freed - leaked memory for every chunk
                        # of every file processed. Free it once we've copied it out.
                        libc.free(processed_chunk)
                        
                        processed_size += chunk_size
                        progress = 30 + int((processed_size / total_size) * 50)
                        # Fixed: capture processed_size/total_size/mode as default
                        # args so each scheduled update shows the value at the time
                        # it was queued, not whatever they'd become by the time
                        # root.after() actually fires.
                        self.root.after(10, lambda p=progress: self.progress_var.set(p))
                        self.root.after(10, lambda ps=processed_size, ts=total_size, m=self.mode:
                                         self.status_var.set(f"{m.capitalize()}ing data... {ps:,}/{ts:,} bytes"))
                
            # Fixed: the old cleanup block was indented at column 0 (outside the
            # method entirely), which is a syntax error - the file couldn't even
            # be imported. rcexwipe() also zeroes the key schedule before freeing,
            # which plain free() doesn't do.
            lib.rcexwipe(ctx)
            
            self.root.after(100, lambda: self.progress_var.set(100))
            self.root.after(100, lambda: self.status_var.set(f"{self.mode.capitalize()}ion completed successfully!"))
            
            self.root.after(500, lambda: messagebox.showinfo("Success", 
                f"File {self.mode}ed successfully!\n\nOriginal: {os.path.basename(infile)}\n{self.mode.capitalize()}ed: {os.path.basename(outfile)}\nSize: {total_size:,} bytes"))
            
        except Exception as e:
            error_msg = str(e)
            self.root.after(100, lambda: self.status_var.set(f"Error: {error_msg}"))
            self.root.after(100, lambda: messagebox.showerror(f"{self.mode.capitalize()}ion Error", f"An error occurred during {self.mode}ion:\n{error_msg}"))
        finally:
            button_color = '#4CAF50' if self.mode == 'encrypt' else '#FF9800'
            self.root.after(100, lambda: self.action_btn.config(state='normal', bg=button_color))
            self.root.after(2000, lambda: self.progress_var.set(0))
            self.root.after(2000, lambda: self.status_var.set(f"Ready to {self.mode} files"))

if __name__ == "__main__":
    root = tk.Tk()
    app = ModernRCEXApp(root)
    root.mainloop()