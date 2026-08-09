# Week 01: Linux Terminal Operations & C Program Execution

This directory documents **Week 01** of the Operating Systems Lab, focusing on **Ubuntu Terminal mechanics**, file handling, GCC compilation, process signals, and basic C program execution.

---

## Ubuntu Terminal Commands & Workflow Showcase

Below is the breakdown of terminal concepts demonstrated in this lab session:

```text
+-----------------------------------------------------------------------------------+
|                           Linux Terminal Action Summary                           |
+-----------------------------------------------------------------------------------+
| 1. Terminal Prompt    : aditya-forge@LAPTOP-184D82EB:~$                           |
| 2. File Creation       : echo "Hello world this is a test" > file.txt             |
| 3. Terminal Editor     : nano Q6.c, nano Q7.c ... nano Q10.c                      |
| 4. Directory Listing   : ls (Lists files with green highlight for executables)   |
| 5. Compilation         : gcc QX.c -o QX                                           |
| 6. Binary Execution    : ./QX                                                     |
| 7. System Search       : find / -name "OS_Lab-1" 2>/dev/null                      |
| 8. Job Suspension      : Ctrl+Z (Sends SIGTSTP to pause process)                  |
| 9. Windows Bridge      : explorer.exe .                                           |
+-----------------------------------------------------------------------------------+
```

---

## Terminal Session Record

```bash
# Edit C source code in GNU Nano
aditya-forge@LAPTOP-184D82EB:~$ nano Q6.c

# Create test input file via output redirection
aditya-forge@LAPTOP-184D82EB:~$ echo "Hello world this is a test" > file.txt

# List files in working directory
aditya-forge@LAPTOP-184D82EB:~$ ls
OS_Lab-1  Q3  Q3.c  Q4  Q4.c  Q5  Q5.c  Q6  Q6.c  file.txt

# Compile and Run Q6 (Character and Word Counter)
aditya-forge@LAPTOP-184D82EB:~$ gcc Q6.c -o Q6
aditya-forge@LAPTOP-184D82EB:~$ ./Q6
Characters=26 Words=6

# Compile and Run Q7 (Fibonacci Series)
aditya-forge@LAPTOP-184D82EB:~$ nano Q7.c
aditya-forge@LAPTOP-184D82EB:~$ gcc Q7.c -o Q7
aditya-forge@LAPTOP-184D82EB:~$ ./Q7
Enter number of terms: 7
0 1 1 2 3 5 8 

# Compile and Run Q8 (Factorial Calculator)
aditya-forge@LAPTOP-184D82EB:~$ nano Q8.c
aditya-forge@LAPTOP-184D82EB:~$ gcc Q8.c -o Q8
aditya-forge@LAPTOP-184D82EB:~$ ./Q8
Enter a number: 6
Factorial=720

# Compile and Run Q9 (Sum of Digits Calculator)
aditya-forge@LAPTOP-184D82EB:~$ nano Q9.c
aditya-forge@LAPTOP-184D82EB:~$ gcc Q9.c -o Q9
aditya-forge@LAPTOP-184D82EB:~$ ./Q9
Enter a number: 1234567
Sum of digits=28

# Compile and Run Q10 (Palindrome String Checker)
aditya-forge@LAPTOP-184D82EB:~$ nano Q10.c
aditya-forge@LAPTOP-184D82EB:~$ gcc Q10.c -o Q10
aditya-forge@LAPTOP-184D82EB:~$ ./Q10
Enter a string: madam
Palindrome

# System Search & Job Suspension (Ctrl+Z)
aditya-forge@LAPTOP-184D82EB:~$ find / -name "OS_Lab-1" 2>/dev/null
^Z
[2]+ Stopped                 find / -name "OS_Lab-1" 2>/dev/null

# Open current WSL directory in Windows File Explorer
aditya-forge@LAPTOP-184D82EB:~$ explorer.exe .
```

---

## Detailed Terminal Command Explanations

### 1. GNU Nano Text Editor (`nano`)
- **Command**: `nano Q10.c`
- **Explanation**: A command-line text editor in Linux. Used to write and edit C source files directly inside the terminal window.

### 2. Output Redirection (`>`)
- **Command**: `echo "Hello world this is a test" > file.txt`
- **Explanation**: The `>` operator redirects output from `echo` into `file.txt`.

### 3. GCC Compiler (`gcc`)
- **Command**: `gcc Q6.c -o Q6`
- **Explanation**: Compiles `Q6.c` and outputs an executable named `Q6` (`-o` flag specifies output name).

### 4. Executing Local Binaries (`./`)
- **Command**: `./Q6`
- **Explanation**: Executes the binary named `Q6` in current directory (`.`). The `./` prefix is mandatory in Linux shells for executing binaries in the current path.

### 5. File Search & Error Redirection (`find` & `2>/dev/null`)
- **Command**: `find / -name "OS_Lab-1" 2>/dev/null`
- **Explanation**: Searches for `OS_Lab-1`. `2>/dev/null` suppresses "Permission Denied" errors by sending stderr (fd 2) to `/dev/null`.

### 6. Job Control (`Ctrl + Z`)
- **Command**: `^Z`
- **Explanation**: Pauses a running process (SIGTSTP signal) and returns control back to the terminal prompt.

### 7. Opening WSL Path in Windows File Explorer (`explorer.exe .`)
- **Command**: `explorer.exe .`
- **Explanation**: Calls Windows File Explorer to open the current Linux directory (`.`) from within WSL.

---

## Week 01 Programs Matrix

| Program | Source File | Description | Terminal Execution Example |
|---|---|---|---|
| **Q1** | [Q1.c](Q1.c) | Find Greatest of 3 Numbers | `./Q1` -> `Enter three numbers: 5 12 8` -> `Greatest = 12` |
| **Q2** | [Q2.c](Q2.c) | Check Even or Odd | `./Q2` -> `Enter a number: 7` -> `Odd` |
| **Q3** | [Q3.c](Q3.c) | Calculate Average of $N$ Numbers | `./Q3` -> `Enter how many numbers: 3` -> `Average=15.00` |
| **Q4** | [Q4.c](Q4.c) | Prime Number Verification | `./Q4` -> `Enter a number: 13` -> `Prime number` |
| **Q5** | [Q5.c](Q5.c) | Check Input Type (Number / String) | `./Q5` -> `Enter input: 123` -> `Number` |
| **Q6** | [Q6.c](Q6.c) | File Character & Word Counter | `./Q6` -> `Characters=26 Words=6` |
| **Q7** | [Q7.c](Q7.c) | Fibonacci Series Generator | `./Q7` -> `Enter number of terms: 7` -> `0 1 1 2 3 5 8` |
| **Q8** | [Q8.c](Q8.c) | Factorial Calculator | `./Q8` -> `Enter a number: 6` -> `Factorial=720` |
| **Q9** | [Q9.c](Q9.c) | Sum of Digits Calculator | `./Q9` -> `Enter a number: 1234567` -> `Sum of digits=28` |
| **Q10** | [Q10.c](Q10.c) | Palindrome String Verification | `./Q10` -> `Enter a string: madam` -> `Palindrome` |
