# 🖥️ SRM University AP — Operating Systems Lab

Coursework repository for the **Operating Systems Lab** at **SRM University AP**. This repository documents weekly lab experiments, Linux terminal workflows, GCC compilation pipelines, system calls, and C program executions inside an Ubuntu WSL environment.

---

## 🏛️ Course Details

- **Institution**: SRM University AP
- **Course**: Operating Systems Lab (CSE)
- **Environment**: Ubuntu Linux / WSL
- **Compiler**: GCC (GNU Compiler Collection)
- **Editor**: GNU Nano
- **Terminal**: Bash CLI (`aditya-forge@LAPTOP-184D82EB`)

---

## 📁 Repository Structure

```text
OS-Lab/
├── README.md             <- Main Lab Overview & Ubuntu Terminal Architecture
├── .gitignore            <- Git rules ignoring compiled Linux binaries (*.o, executables)
└── Week-01/              <- Week 01: Linux Terminal Operations & C Programs
    ├── README.md         <- Week 01 Terminal Workflow Breakdown & Execution Logs
    ├── file.txt          <- Input file created using terminal redirection
    ├── Q1.c ... Q10.c    <- C source files (Programs 1 to 10)
```

---

## 📅 Completed Lab Sessions

| Week | Directory | Topic / Focus | Key Terminal Operations |
|---|---|---|---|
| **Week 01** | **[Week-01](Week-01/)** | **Linux Terminal Operations & Basic C Programs** | `nano`, `echo >`, `gcc`, `./binary`, `find`, `Ctrl+Z`, `explorer.exe .` |

> *Subsequent weekly lab directories (`Week-02`, `Week-03`, etc.) will be added as new lab sessions are completed.*

---

## 🐧 How Ubuntu Terminal Works (Environment Overview)

```text
+-------------------------------------------------------------------------------+
|                       Linux Terminal Environment (WSL)                       |
+-------------------------------------------------------------------------------+
| Shell Prompt    : aditya-forge@LAPTOP-184D82EB:~$                            |
| Operating System: Ubuntu Linux on Windows Subsystem for Linux (WSL)           |
| Shell           : GNU bash (Bourne-Again SHell)                                |
| Working Dir     : /home/aditya-forge (~ represents Home directory)            |
| Text Editor     : GNU Nano                                                     |
| Compiler        : GCC (GNU Compiler Collection)                                |
| Interop Bridge  : explorer.exe . (Launches Windows Explorer for Linux path)   |
+-------------------------------------------------------------------------------+
```

---

## 🛠️ Core Linux Terminal Concepts Applied

### 1. Terminal File Editing (`nano`)
Edit C source code directly inside the Linux terminal:
```bash
nano Q6.c
```

### 2. Output Redirection (`>`)
Redirect standard output from commands directly into a file:
```bash
echo "Hello world this is a test" > file.txt
```

### 3. GCC Compilation Pipeline
Compile C source code into a standalone Linux binary executable:
```bash
gcc Q6.c -o Q6
```

### 4. Running Local Executables (`./`)
Execute binaries in the current directory (`.`):
```bash
./Q6
```

### 5. File System Search & Error Suppression (`2>/dev/null`)
Search the filesystem while filtering out "Permission Denied" errors:
```bash
find / -name "OS_Lab-1" 2>/dev/null
```

### 6. Job Control (`Ctrl + Z`)
Suspend a running process and return control back to the terminal prompt.

### 7. Windows Interoperability (`explorer.exe .`)
Open the current WSL Linux directory directly in Windows File Explorer:
```bash
explorer.exe .
```

---

*Maintained by aditya-forge | SRM University AP — Operating Systems Lab*
