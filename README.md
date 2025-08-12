# Advanced Operating Systems – Assignment 1

**Roll Number:** 2025202019  
**Files:**

- `2025202019_A1_Q1.cpp` – File Reversal Program
- `2025202019_A1_Q2.cpp` – File and Permission Verification Program

---

## 📌 Overview

This assignment contains two programs:

1. **Q1 – File Reversal with Multiple Modes**  
   Reverses a file's content in one of three ways based on a flag.
2. **Q2 – File & Permission Verification**  
   Checks if the reversed file from Q1 has correct permissions and contents.

Both programs are implemented **only with system calls** (`read`, `write`, `lseek`, `open`, `stat`, `mkdir`, `close`).

---

## 🖥 Q1 – File Reversal

### **Usage**

```bash
g++ 2025202019_A1_Q1.cpp
./a.out <input file> <flag> [additional arguments]
```

### **Flags & Functionality**

| Flag  | Description            | Additional Arguments        | Output File Name            | Example                    |
| ----- | ---------------------- | --------------------------- | --------------------------- | -------------------------- |
| **0** | Block-wise reversal    | `<block size>`              | `Assignment1/0_<input file>` | `./a.out input.txt 0 1024` |
| **1** | Full file reversal     | _(none)_                    | `Assignment1/1_<input file>` | `./a.out input.txt 1`      |
| **2** | Partial range reversal | `<start index> <end index>` | `Assignment1/2_<input file>` | `./a.out input.txt 2 5 10` |

---

### **Execution Flow – Q1**

```
┌───────────────────────────────────┐
│ Start                              │
└─────────────┬─────────────────────┘
              │
     Check command-line arguments
              │
              ▼
  Create "Assignment1" directory (700)
              │
              ▼
     Open input file (read-only)
              │
              ▼
  Create output file in directory (600)
              │
              ▼
 ┌────────────┼─────────────┐
 │            │             │
Flag 0   Flag 1         Flag 2
 │            │             │
 ▼            ▼             ▼
Read in   Read from    Reverse bytes
blocks,   both ends,   before start
reverse   swap, write index & after end
each,     reversed     index, keep
write in  data         middle same
order
 │            │             │
 └───────┬────┴───────┬─────┘
         ▼            ▼
 Display completion %
         │
         ▼
       Finish
```

---

## 🖥 Q2 – File and Permission Verification

### **Usage**

```bash
g++ 2025202019_A1_Q2.cpp
./a.out <newfilepath> <oldfilepath> <directory> <flag> [args]
```

**Arguments by Flag:**

- **Flag 0:** `<newfile> <oldfile> <directory> 0 <blockSize>`
- **Flag 1:** `<newfile> <oldfile> <directory> 1`
- **Flag 2:** `<newfile> <oldfile> <directory> 2 <start> <end>`

---

### **Execution Flow – Q2**

```
┌─────────────────────────────────────┐
│ Start                                │
└────────────────┬────────────────────┘
                 │
      Check command-line arguments
                 │
                 ▼
      Verify directory permissions (700)
                 │
                 ▼
     Verify new file permissions (600)
                 │
                 ▼
  Verify original file permissions (644)
                 │
                 ▼
 Check file size (old == new ?)
                 │
                 ▼
 ┌───────────────┼────────────────┐
 │               │                │
Flag 0       Flag 1           Flag 2
 │               │                │
 ▼               ▼                ▼
Compare    Compare original   Reverse check
blocks,    from start with    before/after range
check      reversed new from  & identical middle
order      end in chunks
 │               │                │
 └─────────┬─────┴────────────────┘
           ▼
 Print formatted verification output
           │
           ▼
         Finish
```

---

## 📂 Input & Output Examples

### **Example 1 – Flag 0**

```bash
./a.out input.txt 0 4
```

**input.txt**: `ABCDEFGH`  
**Output file (0_input.txt)**: `DCBAHGFE`

---

### **Example 2 – Flag 1**

```bash
./a.out input.txt 1
```

**input.txt**: `Hello, World!`  
**Output file (1_input.txt)**: `!dlroW ,olleH`

---

### **Example 3 – Flag 2**

```bash
./a.out input.txt 2 2 7
```

**input.txt**: `Hello, World!`  
**Output file (2_input.txt)**: `eHllo, W!dlro`

---

## ⚙️ System Calls Used

- `open`, `read`, `write`, `close`
- `lseek` – seek to specific file positions
- `stat` – check file metadata (size, permissions)
- `mkdir` – create output directory
- `perror` – error reporting

---

## 📝 Notes

- Works for **large files (>1GB)**.
- Progress percentage updates **in place** on terminal.
- Handles errors for missing arguments, invalid flags, and missing files.
