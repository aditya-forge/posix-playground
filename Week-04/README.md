# Week 04: Shell Scripting — Advanced Bash Scripting & File Processing

This directory documents **Week 04** of Operating Systems coursework, focusing on more advanced **Bash Shell Scripting** — string manipulation, file/directory processing, process monitoring, encryption, and spell checking.

---

## Shell Scripting Concepts Demonstrated

```text
+-----------------------------------------------------------------------------------+
|                        Advanced Bash Scripting Action Summary                    |
+-----------------------------------------------------------------------------------+
| 1. String Length        : ${#str} (Returns the length of string 'str')            |
| 2. File Iteration       : for file in "$dir"/* (Loops through files in a dir)     |
| 3. File Comparisons     : cmp -s file1 file2 (Silent comparison of file content)  |
| 4. Process Monitoring   : ps -ef | head (Viewing running processes)               |
| 5. Timestamp Retrieval  : date -r "$file" (Last modification time of a file)      |
| 6. Spell Checking       : aspell or spell (Checks spelling of text files)         |
| 7. Text Manipulation    : tr, awk, sort -u (Translate, pattern scan, unique sort) |
| 8. File Encryption      : openssl enc -aes-256-cbc (Encrypting files via CLI)     |
| 9. String Replacement   : tr -d ' ' (Deletes spaces from a stream)                |
| 10. File Renaming       : mv "old" "new" and tr '[:upper:]' '[:lower:]'           |
+-----------------------------------------------------------------------------------+
```

---

## Terminal Session Record

```bash
# Q11: Minimum String Length Check
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p11.sh hello@123 
Valid string 'hello@123' (length: 9).

# Q12: Echo String Length
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p12.sh "Operating Systems" 
Length of 'Operating Systems' is 17

# Q13: Delete Matching Files Across Directories
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p13.sh dir1 dir2 

# Q14: Process Snapshot Monitor (3 times, 30s interval)
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p14.sh
--- Process Snapshot #1 --- 
Thu Aug 27 09:00:16 UTC 2026 
UID          PID  PPID  C STIME TTY          TIME CMD 
root           1     0  0 08:55 ?        00:00:00 /sbin/init 
...
Waiting 30 seconds... 

# Q15: Last Modification Time of a File
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p15.sh p15.sh 
Last modification time of p15.sh: 
Thu Aug 27 08:18:25 UTC 2026

# Q16: Spell Checker
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p16.sh sample_doc.txt 
Checking spellings in sample_doc.txt... 
No spell utility found. Words in document: 
...
missspelled
operatin
systemmm

# Q17: File Encryption (AES-256-CBC)
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p17.sh secret.txt 
Encrypting secret.txt to secret.txt.enc... 
enter AES-256-CBC encryption password: 
Verifying - enter AES-256-CBC encryption password: 
File encrypted successfully as secret.txt.enc

# Q18: Wordlist Extractor
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p18.sh document.txt wordlist.txt 
Wordlist extracted and saved to wordlist.txt 
Total words: 26

# Q19: Remove Blank Spaces
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p19.sh input_spaced.txt nospaces.txt 
Spaces removed. Output written to nospaces.txt

# Q20: Rename Files to Lowercase
aditya-forge@LAPTOP-184D82EB:~/OS/scripts$ sh p20.sh SAMPLE_FILE.TXT Test_Document.Doc
Renamed: SAMPLE_FILE.TXT -> sample_file.txt
Renamed: Test_Document.Doc -> test_document.doc
```

---

## Week 04 Scripts Matrix

| # | Script File | Description | Execution Example |
|---|---|---|---|
| **Q11** | [p11.sh](p11.sh) | Validate String Length (>=5) | `sh p11.sh hello@123` -> `Valid string` |
| **Q12** | [p12.sh](p12.sh) | Echo String Length | `sh p12.sh "hello"` -> `Length of 'hello' is 5` |
| **Q13** | [p13.sh](p13.sh) | Delete matching files between 2 dirs | `sh p13.sh dir1 dir2` |
| **Q14** | [p14.sh](p14.sh) | Monitor system processes periodically | `sh p14.sh` -> displays `ps -ef` snapshot |
| **Q15** | [p15.sh](p15.sh) | Display last modification time | `sh p15.sh p15.sh` -> `Last modification time of p15.sh` |
| **Q16** | [p16.sh](p16.sh) | Spell check a text document | `sh p16.sh doc.txt` -> checks misspellings |
| **Q17** | [p17.sh](p17.sh) | Encrypt text file (AES-256-CBC) | `sh p17.sh secret.txt` -> creates `secret.txt.enc` |
| **Q18** | [p18.sh](p18.sh) | Extract a unique wordlist (>4 chars) | `sh p18.sh doc.txt list.txt` -> `Total words: 26` |
| **Q19** | [p19.sh](p19.sh) | Remove all blank spaces from file | `sh p19.sh in.txt out.txt` -> `Spaces removed.` |
| **Q20** | [p20.sh](p20.sh) | Rename passed files to lowercase | `sh p20.sh FILE.TXT` -> `Renamed: FILE.TXT -> file.txt` |