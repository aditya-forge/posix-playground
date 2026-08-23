# Week 03: Shell Scripting — Bash Script Execution & Automation

This directory documents **Week 03** of Operating Systems coursework, focusing on **Bash Shell Scripting** — writing, permissioning (`chmod +x`), and executing `.sh` scripts directly in the Ubuntu terminal.

---

## Shell Scripting Concepts Demonstrated

```text
+-----------------------------------------------------------------------------------+
|                        Bash Shell Scripting Action Summary                       |
+-----------------------------------------------------------------------------------+
| 1. Shebang Line        : #!/bin/bash (Tells the kernel which interpreter to use) |
| 2. Script Permissions   : chmod +x script.sh (Makes script executable)            |
| 3. Script Execution     : ./script.sh (Runs script in current directory)          |
| 4. Command-Line Args    : $1, $2, $3, $# (Positional parameters & arg count)     |
| 5. User Input           : read variable (Reads stdin into a variable)             |
| 6. Arithmetic           : $((expression)) (Integer arithmetic expansion)          |
| 7. Conditionals         : if [ condition ]; then ... fi (Test expressions)        |
| 8. Loops                : for (( i=1; i<=n; i++ )) / while [ condition ]          |
| 9. Regex Matching       : [[ $var =~ ^pattern$ ]] (Extended regex test)           |
| 10. Piping & Utilities  : echo | bc, echo | rev, wc -w (Chaining commands)       |
+-----------------------------------------------------------------------------------+
```

---

## Terminal Session Record

```bash
# Q1: Greatest of Three Numbers (Command-Line Parameters)
aditya-forge@LAPTOP-184D82EB:~$ chmod +x greatest3.sh
aditya-forge@LAPTOP-184D82EB:~$ ./greatest3.sh 12 45 7
Greatest number is: 45
aditya-forge@LAPTOP-184D82EB:~$ ./greatest3.sh 90 15 33
Greatest number is: 90

# Q2: Even or Odd Check
aditya-forge@LAPTOP-184D82EB:~$ chmod +x evenodd.sh
aditya-forge@LAPTOP-184D82EB:~$ ./evenodd.sh
Enter a number:
18
18 is Even
aditya-forge@LAPTOP-184D82EB:~$ ./evenodd.sh
Enter a number:
7
7 is Odd

# Q3: Average of N Numbers
aditya-forge@LAPTOP-184D82EB:~$ chmod +x average.sh
aditya-forge@LAPTOP-184D82EB:~$ ./average.sh
Enter how many numbers:
4
Enter number 1:
10
Enter number 2:
20
Enter number 3:
30
Enter number 4:
40
Sum = 100
Average = 25.00

# Q4: Prime Number Check
aditya-forge@LAPTOP-184D82EB:~$ chmod +x prime.sh
aditya-forge@LAPTOP-184D82EB:~$ ./prime.sh
Enter a number:
29
29 is Prime
aditya-forge@LAPTOP-184D82EB:~$ ./prime.sh
Enter a number:
15
15 is not Prime

# Q5: Number or String Detection
aditya-forge@LAPTOP-184D82EB:~$ chmod +x numorstr.sh
aditya-forge@LAPTOP-184D82EB:~$ ./numorstr.sh
Enter input:
12345
12345 is a Number
aditya-forge@LAPTOP-184D82EB:~$ ./numorstr.sh
Enter input:
hello
hello is a String

# Q6: Character & Word Counter (File Processing)
aditya-forge@LAPTOP-184D82EB:~$ cat sample.txt
This is line one
Shell scripting is fun
Linux commands are powerful
aditya-forge@LAPTOP-184D82EB:~$ chmod +x charword.sh
aditya-forge@LAPTOP-184D82EB:~$ ./charword.sh sample.txt
Line 1: Characters = 16, Words = 4
Line 2: Characters = 22, Words = 4
Line 3: Characters = 27, Words = 4

# Q7: Fibonacci Series Generator
aditya-forge@LAPTOP-184D82EB:~$ chmod +x fibonacci.sh
aditya-forge@LAPTOP-184D82EB:~$ ./fibonacci.sh
Enter number of terms:
10
Fibonacci series:
0 1 1 2 3 5 8 13 21 34

# Q8: Factorial Calculator
aditya-forge@LAPTOP-184D82EB:~$ chmod +x factorial.sh
aditya-forge@LAPTOP-184D82EB:~$ ./factorial.sh
Enter a number:
5
Factorial of 5 = 120
aditya-forge@LAPTOP-184D82EB:~$ ./factorial.sh
Enter a number:
7
Factorial of 7 = 5040

# Q9: Sum of Digits
aditya-forge@LAPTOP-184D82EB:~$ chmod +x sumdigits.sh
aditya-forge@LAPTOP-184D82EB:~$ ./sumdigits.sh
Enter a number:
9425
Sum of digits of 9425 = 20

# Q10: Palindrome String Check
aditya-forge@LAPTOP-184D82EB:~$ chmod +x palindrome.sh
aditya-forge@LAPTOP-184D82EB:~$ ./palindrome.sh
Enter a string:
madam
madam is a Palindrome
aditya-forge@LAPTOP-184D82EB:~$ ./palindrome.sh
Enter a string:
hello
hello is not a Palindrome
```

---

## Detailed Shell Scripting Concepts

### 1. Shebang (`#!/bin/bash`)
- The first line of every script tells the Linux kernel to use `/bin/bash` as the interpreter.

### 2. Making Scripts Executable (`chmod +x`)
- **Command**: `chmod +x script.sh`
- **Explanation**: Adds the execute (`x`) permission bit to the file so it can be run as `./script.sh`.

### 3. Command-Line Arguments (`$1`, `$2`, `$#`)
- **`$1`, `$2`, `$3`**: Positional parameters (first, second, third argument).
- **`$#`**: Total number of arguments passed.
- **`$0`**: The script name itself.

### 4. Reading User Input (`read`)
- **Command**: `read variable`
- **Explanation**: Reads a line from standard input (stdin) and stores it in `variable`.

### 5. Integer Arithmetic (`$((...))`)
- **Command**: `$((num % 2))`
- **Explanation**: Bash arithmetic expansion for integer math (modulo, addition, multiplication, etc.).

### 6. Floating-Point Arithmetic (`bc`)
- **Command**: `echo "scale=2; $sum / $n" | bc`
- **Explanation**: Bash only supports integer math natively. For decimals, pipe the expression into `bc` (basic calculator).

### 7. Conditional Tests (`[ ]` and `[[ ]]`)
- **`[ $a -ge $b ]`**: POSIX test — integer comparison (`-ge` = greater or equal).
- **`[[ $input =~ ^regex$ ]]`**: Extended bash test — regex pattern matching with `=~`.

### 8. Looping Constructs
- **C-style for loop**: `for (( i=1; i<=n; i++ ))` — familiar iteration syntax.
- **While loop**: `while [ $temp -gt 0 ]` — loop until condition becomes false.

### 9. String Reversal (`rev`)
- **Command**: `echo "$str" | rev`
- **Explanation**: The `rev` utility reverses each line of input character-by-character.

### 10. Word Counting (`wc -w`)
- **Command**: `echo "$line" | wc -w`
- **Explanation**: `wc -w` counts the number of words in the input stream.

---

## Week 03 Scripts Matrix

| # | Script File | Description | Execution Example |
|---|---|---|---|
| **Q1** | [greatest3.sh](greatest3.sh) | Greatest of 3 Numbers (CLI args) | `./greatest3.sh 12 45 7` -> `Greatest number is: 45` |
| **Q2** | [evenodd.sh](evenodd.sh) | Even / Odd Check | `./evenodd.sh` -> `18 is Even` |
| **Q3** | [average.sh](average.sh) | Average of N Numbers | `./average.sh` -> `Average = 25.00` |
| **Q4** | [prime.sh](prime.sh) | Prime Number Verification | `./prime.sh` -> `29 is Prime` |
| **Q5** | [numorstr.sh](numorstr.sh) | Number vs String Detection | `./numorstr.sh` -> `12345 is a Number` |
| **Q6** | [charword.sh](charword.sh) | File Character & Word Counter | `./charword.sh sample.txt` -> `Line 1: Characters = 16, Words = 4` |
| **Q7** | [fibonacci.sh](fibonacci.sh) | Fibonacci Series Generator | `./fibonacci.sh` -> `0 1 1 2 3 5 8 13 21 34` |
| **Q8** | [factorial.sh](factorial.sh) | Factorial Calculator | `./factorial.sh` -> `Factorial of 5 = 120` |
| **Q9** | [sumdigits.sh](sumdigits.sh) | Sum of Digits Calculator | `./sumdigits.sh` -> `Sum of digits of 9425 = 20` |
| **Q10** | [palindrome.sh](palindrome.sh) | Palindrome String Checker | `./palindrome.sh` -> `madam is a Palindrome` |