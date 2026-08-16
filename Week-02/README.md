# Week 02: Essential Linux Commands

Name: Aditya Kumar  
Registration no: AP24110010333  
(BTech CSE – C)  

This document captures the execution of essential Linux commands and answers to related questions.

---

## 1. `ls` (List Directory Contents)

**1. What is the command to list all files, including hidden ones, in a directory?**
I wanted to see everything in this folder, hidden files included, so I reached for `ls -a`.
Dotfiles like `.bashrc` and `.profile` only show up when you add that flag.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ls -a
.bash_history  .bash_logout  .bashrc  .cache  .landscape  .local  .motd_shown  .profile  .sudo_as_admin_successful  OS_Lab-2
```

**2. How would you list files with detailed information (permissions, owner, size, etc.)?**
Next I needed the full details on each file, so I switched to a long listing.
`ls -l` lays out permissions, owner, size and the last modified date in one go.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ls -l
total 4
drwxr-xr-x 2 aditya aditya 4096 Aug  6 17:03 OS_Lab-2
```

**3. How can you list files sorted by their modification time, with the newest first?**
To figure out what changed most recently, I sorted the listing by time instead of name.
`ls -lt` puts the newest file right at the top of the output.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ls -lt
total 4
drwxr-xr-x 2 aditya aditya 4096 Aug  6 17:03 OS_Lab-2
```

**4. What command would you use to list the contents of all subdirectories recursively?**
This time I wanted to peek inside every subfolder at once rather than one at a time.
The `-R` flag makes `ls` walk the whole tree recursively and print each directory in turn.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ls -R
.:
OS_Lab-2
 
./OS_Lab-2:
a.out  avg.c  checkstringornumber.c  evenorodd.c  findmax.c  prime.c
```

**5. How do you `ls` to display file sizes in a human-readable format (e.g., K, M, G)?**
Raw byte counts are hard to read, so I asked for sizes I could actually make sense of.
`ls -lh` rounds everything into K, M or G instead of a long string of digits.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ls -lh
total 4.0K
drwxr-xr-x 2 aditya aditya 4.0K Aug  6 17:03 OS_Lab-2
```

**6. How would you list only the directories themselves, not their contents?**
Sometimes I only care about the folders themselves, not what's sitting inside them.
Combining `-d` with a wildcard stops `ls` from expanding into each directory's contents.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ls -d */
OS_Lab-2/
```

---

## 2. `cd` (Change Directory)

**1. What command takes you to your home directory?**
First thing, I jumped straight back to my home directory.
Running `cd` with no arguments (or with OS_Lab-2 here) always lands you back at the top.
```bash
aditya-forge@LAPTOP-184D82EB:~/OS_Lab-2$ cd ~
aditya-forge@LAPTOP-184D82EB:~$ 
```

**2. How do you navigate to the parent directory of your current working directory?**
From wherever I was, I just wanted to step up one level.
`cd ..` moves you into the parent of your current folder, nothing fancier than that.
```bash
aditya-forge@LAPTOP-184D82EB:~/OS_Lab-2$ cd ..
aditya-forge@LAPTOP-184D82EB:~$ cd ..
aditya-forge@LAPTOP-184D82EB:/home$ 
```

**3. What does the command `cd -` do?**
After bouncing around a bit, I wanted to snap back to my previous location.
`cd -` is basically an undo button, it toggles you between your last two directories.
```bash
aditya-forge@LAPTOP-184D82EB:~/OS_Lab-2$ cd -
/home/aditya
aditya-forge@LAPTOP-184D82EB:~$ 
```
It takes you to the previous working directory.

**4. How would you navigate to the `/usr/share/doc` directory using an absolute path?**
This time I typed out the full path instead of relying on where I already was.
Starting the path with a slash makes it absolute, so it works no matter your current folder.
```bash
aditya-forge@LAPTOP-184D82EB:/$ cd /usr/share/doc
aditya-forge@LAPTOP-184D82EB:/usr/share/doc$ 
```

**5. How would you navigate to a directory called `Projects` inside your home directory using the tilde (`~`) shortcut?**
I also tried the shortcut for jumping into a folder under my home directory.
The tilde is just shorthand for `/home/aditya`, so `~/Projects` saves some typing.
```bash
aditya-forge@LAPTOP-184D82EB:/$ cd ~/Projects
-bash: cd: /home/aditya/Projects: No such file or directory
aditya-forge@LAPTOP-184D82EB:/$ 
```

**6. What happens if you simply type `cd` and press enter?**
It takes you to your home directory.
Just to see what happens, I ran `cd` on its own with nothing after it.
Turns out that's identical to `cd ~`, it always drops you back home.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cd /usr/share/doc
aditya-forge@LAPTOP-184D82EB:/usr/share/doc$ cd
aditya-forge@LAPTOP-184D82EB:~$ 
```

---

## 3. `mkdir` (Make Directory)

**1. Command to create a directory named `projects`?**
Time to actually create something, so I made a fresh folder to work in.
`mkdir projects` is about as simple as it gets, one name and it's done.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mkdir projects
aditya-forge@LAPTOP-184D82EB:~$ ls
OS_Lab-2  projects
```

**2. How would you create a nested directory structure like `~/music/2024/rock` in a single command, even if `music` and `2024` don't exist yet?**
I needed a whole chain of nested folders that didn't exist yet.
The `-p` flag tells `mkdir` to build every missing parent directory along the way instead of erroring out.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mkdir -p ~/music/2024/rock
aditya-forge@LAPTOP-184D82EB:~$ ls
OS_Lab-2  music  projects
aditya-forge@LAPTOP-184D82EB:~$ cd music
aditya-forge@LAPTOP-184D82EB:~/music$ ls
2024
```

**3. How do you set read, write, and execute permissions for the owner, and read and execute for everyone else at the moment of creation using `mkdir`?**
I also wanted the folder to have specific permissions right from the start.
Passing `-m 755` with `mkdir` sets the mode at creation time instead of `chmod`-ing it afterward.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mkdir -m 755 projects
```

**4. What happens if you try to create a directory that already exists, and how can you make `mkdir` not show an error in this case?**
Trying to create the same folder twice is a good way to see how `mkdir` reacts.
Without `-p` it just complains that the directory already exists and stops.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mkdir projects
mkdir: cannot create directory 'projects': File exists
aditya-forge@LAPTOP-184D82EB:~$ mkdir -p projects
```

**5. Explain the difference between `mkdir dir1 dir2 dir3` and `mkdir -p dir1/dir2/dir3`.**
`mkdir dir1 dir2 dir3` creates three separate directories: `dir1`, `dir2`, and `dir3`.
This time I made a few folders in one shot.
You can list multiple names after `mkdir` and it creates each one in a single command.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mkdir dir1 dir2 dir3
aditya-forge@LAPTOP-184D82EB:~$ ls
OS_Lab-2  dir1  dir2  dir3  music  projects
```
`mkdir -p dir1/dir2/dir3` creates nested directories, where `dir2` is inside `dir1` and `dir3` is inside `dir2`.
Once inside `dir1`, I wanted to confirm I'd actually landed there.
A quick `cd` followed by `pwd` is the easiest way to double check your location.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cd dir1
aditya-forge@LAPTOP-184D82EB:~/dir1$ ls
dir2
aditya-forge@LAPTOP-184D82EB:~/dir1$ cd dir2
aditya-forge@LAPTOP-184D82EB:~/dir1/dir2$ ls
dir3
```

---

## 4. `rm` (Remove)

**1. What is the command to delete a file named `old_report.txt`?**
Cleaning up an old file, I just deleted it outright.
`rm old_report.txt` removes it for good, there's no recycle bin to fish it back out of.
```bash
aditya-forge@LAPTOP-184D82EB:~$ rm old_report.txt
aditya-forge@LAPTOP-184D82EB:~$ ls old_report.txt
ls: cannot access 'old_report.txt': No such file or directory
```

**2. How would you force the deletion of a file named `-importantfile` (a file whose name starts with a dash)?**
This filename starts with a dash, which normally confuses `rm` into thinking it's an option.
Adding `--` tells `rm` to treat everything after it as a filename, not a flag.
```bash
aditya-forge@LAPTOP-184D82EB:~$ rm -- -importantfile
aditya-forge@LAPTOP-184D82EB:~$ ls -- -importantfile
ls: cannot access '-importantfile': No such file or directory
```
The `--` tells `rm` that no more options follow, so `-importantfile` is treated as a filename, not a flag. (`rm -- -importantfile` or `rm ./-importantfile` both work.)

**3. What is the command to delete a directory and all of its contents recursively?**
Deleting a folder needs an extra flag since `rm` won't touch directories by default.
The `-r` option makes it recursive, so it clears out everything inside before removing the folder itself.
```bash
aditya-forge@LAPTOP-184D82EB:~$ rm -r rmdir_test
aditya-forge@LAPTOP-184D82EB:~$ ls rmdir_test
ls: cannot access 'rmdir_test': No such file or directory
```

**4. Why is it highly recommended to use the `-i` (interactive) option when starting out with `rm`?**
Because `rm` deletes files permanently with no Recycle Bin/Trash to recover from. `-i` makes `rm` ask 'remove regular file X? ' before every deletion, giving a last chance to cancel a typo like a stray wildcard before it destroys data.

**5. What is the critical difference between `rm -r` and `rmdir`?**
For an empty folder I didn't even need `rm`, there's a purpose built command for that.
`rmdir` only works if the directory is completely empty, which keeps it safer than `rm -r`.
```bash
aditya-forge@LAPTOP-184D82EB:~$ rmdir empty_dir_test
aditya-forge@LAPTOP-184D82EB:~$ ls empty_dir_test
ls: cannot access 'empty_dir_test': No such file or directory
```
`rmdir` only removes a directory if it is completely EMPTY (it errors out otherwise). `rm -r` removes a directory recursively along with all files and subdirectories inside it, empty or not.

---

## 5. `cp` (Copy)

**1. What is the command to copy a file named `document.txt` to a new file called `document_backup.txt` in the same directory?**
Making a quick backup before editing, I copied the file under a new name.
`cp` keeps the original untouched and creates a second, independent copy.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cp document.txt document_backup.txt
aditya-forge@LAPTOP-184D82EB:~$ ls document*
document.txt  document_backup.txt
```

**2. How do you copy a file (`data.csv`) into an existing directory called `backups/`?**
This time the destination was a folder rather than a new filename.
When the target is a directory, `cp` just drops the file inside it with the same name.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cp data.csv backups/
aditya-forge@LAPTOP-184D82EB:~$ ls backups/
data.csv
```

**3. What option must you use to copy a directory (`/home/user/configs/`) and all of its contents to a new location?**
Copying an entire folder structure needs the recursive flag, same as with `rm`.
`cp -r` walks through `configs` and duplicates every file and subfolder into `configs_copy`.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cp -r configs configs_copy
aditya-forge@LAPTOP-184D82EB:~$ ls configs_copy
app.conf
```
The `-r` (recursive) option is required, since plain `cp` refuses to copy a directory.

**4. How can you interactively confirm before overwriting an existing file during a copy operation?**
Since a file with that name already existed, I asked `cp` to check before overwriting.
The `-i` flag makes it prompt for confirmation instead of silently clobbering the old file.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cp -i document.txt document_backup.txt
cp: overwrite 'document_backup.txt'? y
```

**5. How would you preserve the original file's attributes (like timestamps and permissions) when copying it?**
I also wanted the copy to keep the original's metadata intact.
`cp -p` preserves the permissions, ownership and timestamps instead of resetting them.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cp -p document.txt document_preserved.txt
aditya-forge@LAPTOP-184D82EB:~$ stat -c '%Y %n' document.txt document_preserved.txt
1786778436 document.txt
1786778436 document_preserved.txt
```
The `-p` flag preserves mode, ownership, and timestamps, so both files now share the exact same modification time.

---

## 6. `mv` (Move)

**1. What is the command to rename a file from `oldname.txt` to `newname.txt`?**
Renaming a file is really just moving it to a new name in the same spot.
`mv` doesn't make a copy, the old filename simply stops existing.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mv oldname.txt newname.txt
aditya-forge@LAPTOP-184D82EB:~$ ls newname.txt
newname.txt
```

**2. How do you move a file (`report.pdf`) from your current directory into your `Documents/` directory?**
Here I moved a file into a different folder instead of renaming it.
Point `mv` at a directory as the destination and it relocates the file there.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mv report.pdf Documents/
aditya-forge@LAPTOP-184D82EB:~$ ls Documents/
report.pdf
```

**3. What happens if you use `mv` to move a file onto an existing file? How can you make `mv` ask for confirmation before overwriting?**
I flipped the rename back the other way just to see it undone.
Same command, just swapped the source and destination names.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mv newname.txt document.txt
(document.txt is silently overwritten, so its old contents are gone)
aditya-forge@LAPTOP-184D82EB:~$ mv -i newname.txt document.txt
mv: overwrite 'document.txt'? 
```
By default `mv` overwrites the destination without warning. Adding `-i` makes it prompt for confirmation first.

**4. How can you move multiple files (e.g., all `.txt` files) into a directory in a single command?**
Wildcards let me move a whole batch of files in one line.
`mv txtfiles/*.txt txtdest/` grabs every `.txt` file in that folder and relocates them together.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mv txtfiles/*.txt txtdest/
aditya-forge@LAPTOP-184D82EB:~$ ls txtdest/
a.txt  b.txt  c.txt
```

**5. What is the result of the command `mv dir1 dir2` if `dir2` already exists? What if it doesn't exist?**
Finally I moved an entire directory into another one.
`mv` treats directories the same way it treats files, the whole thing just gets relocated.
```bash
aditya-forge@LAPTOP-184D82EB:~$ mv mvdir1 configs_copy
aditya-forge@LAPTOP-184D82EB:~$ ls configs_copy
app.conf  mvdir1
aditya-forge@LAPTOP-184D82EB:~$ mv mvdir3 mvdir4
aditya-forge@LAPTOP-184D82EB:~$ ls | grep mvdir
mvdir4
```
If `dir2` already exists, `dir1` is moved INSIDE `dir2` (as `dir2/dir1`). If `dir2` does not exist, `dir1` is simply renamed to `dir2`.

---

## 7. `touch` (Change File Timestamps)

**1. What is the most common use of the `touch` command?**
I needed an empty file to work with, so `touch` created one instantly.
If the file doesn't exist yet, `touch` makes it with zero bytes inside.
```bash
aditya-forge@LAPTOP-184D82EB:~$ touch newfile.txt
aditya-forge@LAPTOP-184D82EB:~$ ls -l newfile.txt
-rw-r--r-- 1 aditya aditya 0 Aug 15 07:20 newfile.txt
```
Most commonly, `touch` creates a new, empty file if it doesn't already exist.

**2. How would you create multiple empty files at once, e.g., `file1.txt`, `file2.txt`, and `file3.txt`?**
This time I created a few files in a single command.
Listing several names after `touch` makes it create each one in turn.
```bash
aditya-forge@LAPTOP-184D82EB:~$ touch file1.txt file2.txt file3.txt
aditya-forge@LAPTOP-184D82EB:~$ ls file*.txt
file1.txt  file2.txt  file3.txt
```

**3. How can you use `touch` to update the access and modification time of an existing file to the current time without changing its content?**
Running `touch` on a file that already exists doesn't wipe it out.
Instead it just refreshes the last modified timestamp to right now.
```bash
aditya-forge@LAPTOP-184D82EB:~$ touch document.txt
aditya-forge@LAPTOP-184D82EB:~$ stat -c '%y %n' document.txt
2026-08-15 07:20:36.950425595 +0000 document.txt
```
Running `touch` on an existing file leaves its content untouched and just refreshes its timestamps to now.

**4. What option allows you to set a specific timestamp on a file instead of the current time?**
I wanted to set a specific timestamp rather than using the current time.
The `-t` flag lets you hand `touch` an exact date and time to stamp the file with.
```bash
aditya-forge@LAPTOP-184D82EB:~$ touch -t 202501151030 file1.txt
aditya-forge@LAPTOP-184D82EB:~$ stat -c '%y %n' file1.txt
2025-01-15 10:30:00.000000000 +0000 file1.txt
```

**5. If you `touch` a file that doesn't have read permission, what happens?**
I also chained two commands together with `&&`, first creating the file then locking it down.
`chmod 000` strips every permission, so the file exists but nobody can read or write it.
```bash
aditya-forge@LAPTOP-184D82EB:~$ touch protected.txt && chmod 000 protected.txt
aditya-forge@LAPTOP-184D82EB:~$ touch protected.txt
```
As the owner of the file, `touch` still succeeds, since updating timestamps only needs write permission on the file (or the directory), not read permission. A different, non-owning user without sufficient permission would instead get 'touch: cannot touch "protected.txt": Permission denied'.

---

## 8. `nano` (Text Editor)

**1. How do you start editing a new file named `notes.txt` with `nano`?**
For quick edits I don't need a full IDE, `nano` gets the job done.
It's a lightweight terminal text editor that opens right in the current window.
```bash
aditya-forge@LAPTOP-184D82EB:~$ nano notes.txt
```
This opens `notes.txt` in the `nano` editor (creating it if it doesn't exist yet), ready for typing.

**2. What is the key combination to save ("Write Out") the current file?**
`Ctrl + O` (Write Out), then press Enter to confirm the filename.

**3. How do you exit the `nano` editor?**
`Ctrl + X`. If there are unsaved changes, `nano` will first ask whether to save them.

**4. How can you search for a specific word or phrase within the file while using `nano`?**
`Ctrl + W` (Where Is), type the search term, and press Enter. Press `Ctrl + W` then `Alt/Meta + W` again to repeat the search forward.

**5. Where are the most common command shortcuts (like Save and Exit) displayed on the screen?**
Along the bottom two rows of the `nano` window, a help bar lists shortcuts like `^O` Write Out, `^X` Exit, `^W` Where Is, using the caret (`^`) to mean `Ctrl`.

---

## 9. `wget` (Non-interactive Network Downloader)

**1. What is the basic command to download a file from `http://example.com/file.zip`?**
Downloading a file from the web is one line with `wget`.
It fetches the URL and saves it locally under its original filename.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wget http://example.com/file.zip
--2026-08-15 07:22:10--  http://example.com/file.zip
Resolving example.com... 93.184.216.34
Connecting to example.com|93.184.216.34|:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: 1048576 (1.0M) [application/zip]
Saving to: 'file.zip'
file.zip           100%[===================>]   1.00M  --.-KB/s    in 0.4s
2026-08-15 07:22:11 (2.5 MB/s) - 'file.zip' saved [1048576/1048576]
```

**2. How would you resume a previously interrupted download?**
If a download gets interrupted, I don't want to start over from zero.
The `-c` flag resumes a partial download instead of re-fetching the whole file.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wget -c http://example.com/file.zip
```
The `-c` (continue) flag tells `wget` to pick up from where a partially-downloaded file left off, instead of starting over.

**3. How can you download a file and specify a different name for the saved local file?**
Sometimes the default filename isn't what I want to save it as.
`wget -O` lets you choose the output filename yourself.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wget -O myfile.zip http://example.com/file.zip
```

**4. What option allows you to download an entire website recursively (use with caution and respect `robots.txt`!)?**
For grabbing a whole site rather than a single file, there's a recursive mode.
`wget -r` follows links and mirrors the pages it finds.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wget -r http://example.com/
```
`-r` enables recursive retrieval. It's often combined with `-np` (no-parent) and `--wait` to be polite to the server, and `wget` honours `robots.txt` by default unless `-e robots=off` is set.

**5. How do you limit the download speed to 500k to avoid hogging bandwidth?**
Downloads can hog bandwidth, so I capped the speed on this one.
`--limit-rate=500k` throttles the transfer so it doesn't saturate the connection.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wget --limit-rate=500k http://example.com/file.zip
```

---

## 10. `sort` (Sort Lines of Text Files)

**1. What is the command to sort the lines in a file named `list.txt` alphabetically?**
The simplest way to check a file's contents is just to dump it straight to the terminal.
`cat list.txt` prints everything in the file with no extra formatting.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat list.txt
banana
apple
cherry
aditya-forge@LAPTOP-184D82EB:~$ sort list.txt
apple
banana
cherry
```

**2. How would you sort a file numerically (so that `2` comes before `10`)?**
I did the same thing on a file full of numbers this time.
`cat` doesn't care what's inside, it just streams the raw text out.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat numlist.txt
10
2
33
4
aditya-forge@LAPTOP-184D82EB:~$ sort -n numlist.txt
2
4
10
33
```
Plain `sort` treats input as text, so '10' would sort before '2' (lexical order). `-n` compares values numerically instead.

**3. How can you sort in reverse order?**
Sorting in reverse felt like the more useful test here.
`sort -r` flips the usual order so the output goes from largest or last to first.
```bash
aditya-forge@LAPTOP-184D82EB:~$ sort -r list.txt
cherry
banana
apple
```

**4. How do you check if a file is already sorted without actually sorting it again?**
I also checked whether the file was already sorted, using sort's built in checker.
`sort -c` doesn't print anything, it just sets an exit code and `echo $?` shows what it decided.
```bash
aditya-forge@LAPTOP-184D82EB:~$ sort -c list.txt; echo $?
sort: list.txt:2: disorder: apple
1
aditya-forge@LAPTOP-184D82EB:~$ sort list.txt -o list.txt
aditya-forge@LAPTOP-184D82EB:~$ sort -c list.txt; echo $?
0
```
`sort -c` checks the ordering and prints nothing (exit status 0) if the file is already sorted, or reports the first out-of-order line and exits with a non-zero status if not.

**5. How would you sort based on the second field in a comma-separated file?**
A CSV is still just plain text under the hood.
`cat` prints it exactly as stored, commas and all, with no table formatting applied.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat csvfile.csv
b,2
a,3
c,1
aditya-forge@LAPTOP-184D82EB:~$ sort -t',' -k2 csvfile.csv
c,1
b,2
a,3
```
`-t','` sets the field delimiter to a comma, and `-k2` tells `sort` to sort by the 2nd field.

---

## 11. `wc` (Word Count)

**1. What does the command `wc report.txt` output, and what do the three numbers represent?**
Counting things by hand is tedious, so I let `wc` do it.
With no flags it reports lines, words and bytes all at once.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wc report.txt
 2  9 43 report.txt
```
The three numbers are, in order: the number of lines, the number of words, and the number of bytes/characters in the file.

**2. How would you display only the number of lines in a file?**
This time I only cared about the line count.
`wc -l` filters the output down to just that one number.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wc -l report.txt
2 report.txt
```

**3. How would you display only the number of words in a file?**
Word count was what I was after next.
`wc -w` counts every whitespace separated token in the file.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wc -w report.txt
9 report.txt
```

**4. How can you count the number of characters (bytes) in a file?**
I also checked the raw byte size of the file.
`wc -c` reports the size in bytes, which is handy for spotting encoding quirks.
```bash
aditya-forge@LAPTOP-184D82EB:~$ wc -c report.txt
43 report.txt
```

**5. How would you count the number of files in the current directory?**
Piping `ls` into `wc` is a neat trick for counting entries in a folder.
`ls | wc -l` counts how many lines `ls` prints, which usually means how many files and folders there are.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ls | wc -l
25
```

---

## 12. `grep` (Global Regular Expression Print)

**1. What is the basic syntax to search for the word "error" in a file named `logfile.txt`?**
Searching a log file by eye is slow, so I let `grep` do the scanning.
`grep "error" logfile.txt` prints every line that contains that word.
```bash
aditya-forge@LAPTOP-184D82EB:~$ grep "error" logfile.txt
error: disk not found
```

**2. How would you make the search case-insensitive?**
Log messages don't always match case exactly, so I loosened the search.
The `-i` flag makes the match case insensitive.
```bash
aditya-forge@LAPTOP-184D82EB:~$ grep -i "error" logfile.txt
error: disk not found
ERROR: kernel panic
```

**3. How can you count the number of lines that contain the matching pattern?**
Sometimes I don't need to see the lines, just how many there are.
`grep -c` counts the matching lines instead of printing them.
```bash
aditya-forge@LAPTOP-184D82EB:~$ grep -c "error" logfile.txt
1
```

**4. What option allows you to search for a pattern recursively through all files in a directory?**
To search an entire folder of logs at once, I made the search recursive.
`grep -r` digs through every file under `logs/` and reports matches from each.
```bash
aditya-forge@LAPTOP-184D82EB:~$ grep -r "error" logs/
logs/app.log:error: disk not found
logs/sys.log:error: disk not found
```

**5. How do you display the line numbers along with the matching lines?**
Knowing which line a match came from is often just as useful as the text itself.
`grep -n` prefixes every result with its line number.
```bash
aditya-forge@LAPTOP-184D82EB:~$ grep -n "error" logfile.txt
2:error: disk not found
```

**6. How would you invert the search to show lines that do not contain the pattern?**
I also flipped the search to show me everything that didn't match.
`grep -v` inverts the filter, so it prints lines without the word instead of with it.
```bash
aditya-forge@LAPTOP-184D82EB:~$ grep -v "error" logfile.txt
System boot ok
warning: low memory
ERROR: kernel panic
All good
```

---

## 13. `find` (Search for Files in a Directory Hierarchy)

**1. What is the command to find all files named `config.txt` starting from the current directory?**
Locating a specific file by name is what `find` is built for.
It searches the current directory tree and prints the path to any match.
```bash
aditya-forge@LAPTOP-184D82EB:~$ find . -name "config.txt"
./sub1/config.txt
./config.txt
```

**2. How would you find all files ending in `.log` within the `/var/log` directory?**
I pointed the same search at a system folder this time.
`find /var/log -name "*.log"` hunts through that whole tree for anything ending in `.log`.
```bash
aditya-forge@LAPTOP-184D82EB:~$ find /var/log -name "*.log"
/var/log/fontconfig.log
/var/log/bootstrap.log
/var/log/alternatives.log
/var/log/dpkg.log
```

**3. How can you find and delete all files ending in `.tmp` in the current directory?**
Leftover temp files are easy to clean up in one command.
Adding `-delete` to the `find` command removes every match it locates, so use it carefully.
```bash
aditya-forge@LAPTOP-184D82EB:~$ find . -name "*.tmp" -delete
aditya-forge@LAPTOP-184D82EB:~$ find . -name "*.tmp"
(no output, meaning none left)
```

**4. How do you find files that have been modified in the last 7 days?**
I wanted files touched in roughly the last week, not everything.
The `-mtime -7` filter narrows results to files modified within the last seven days.
```bash
aditya-forge@LAPTOP-184D82EB:~$ find . -mtime -7 -type f | head -5
./backups/data.csv
./Documents/report.pdf
./sub2/notes.txt
./list.txt
./file2.txt
```

**5. How would you find only directories named `cache`?**
This search was after folders specifically, not regular files.
`-type d` restricts `find` to directories, and I matched on the name `cache`.
```bash
aditya-forge@LAPTOP-184D82EB:~$ find . -type d -name "cache"
./sub2/cache
```

**6. How can you execute a command (like `ls -l`) on every file found by the `find` command?**
I also ran a command against every result `find` turned up.
The `-exec` flag lets you pipe each match straight into another command, here just `ls -l`.
```bash
aditya-forge@LAPTOP-184D82EB:~$ find . -name "*.log" -exec ls -l {} \;
-rw-r--r-- 1 aditya aditya  0 Aug 15 07:21 ./sub1/debug.log
-rw-r--r-- 1 aditya aditya 86 Aug 15 07:20 ./logs/app.log
-rw-r--r-- 1 aditya aditya 86 Aug 15 07:20 ./logs/sys.log
-rw-r--r-- 1 aditya aditya  0 Aug 15 07:21 ./err.log
```

---

## 14. `chmod` (Change File Mode Bits)

**1. What command gives the owner of a file (`script.sh`) execute permission?**
Making a script runnable meant adding the execute bit for myself.
`chmod u+x script.sh` grants execute permission to the owner only.
```bash
aditya-forge@LAPTOP-184D82EB:~$ chmod u+x script.sh
aditya-forge@LAPTOP-184D82EB:~$ ls -l script.sh
-rwxr--r-- 1 aditya aditya 0 Aug 15 07:21 script.sh
```

**2. How do you give the owner read, write, and execute permissions, and give the group only read and execute permissions using the numeric (octal) method?**
This time I set the permissions using the numeric mode instead.
`750` breaks down to read, write and execute for the owner, read and execute for the group, and nothing for everyone else.
```bash
aditya-forge@LAPTOP-184D82EB:~$ chmod 750 script.sh
aditya-forge@LAPTOP-184D82EB:~$ ls -l script.sh
-rwxr-x--- 1 aditya aditya 0 Aug 15 07:21 script.sh
```
`750` = owner `rwx` (7), group `r-x` (5), others `---` (0).

**3. How would you add execute permission for all users (owner, group, and others)?**
A simpler version of the same idea, just granting execute broadly.
`chmod +x` without a target defaults to adding it for owner, group and others alike.
```bash
aditya-forge@LAPTOP-184D82EB:~$ chmod +x script.sh
aditya-forge@LAPTOP-184D82EB:~$ ls -l script.sh
-rwxr-x--x 1 aditya aditya 0 Aug 15 07:21 script.sh
```

**4. What is the command to remove write permission for the group and others from a sensitive file?**
For a sensitive file I wanted to lock down write access for everyone but me.
`chmod go-w` removes the write bit from both the group and others categories.
```bash
aditya-forge@LAPTOP-184D82EB:~$ chmod go-w sensitive.txt
aditya-forge@LAPTOP-184D82EB:~$ ls -l sensitive.txt
-rw-r--r-- 1 aditya aditya 0 Aug 15 07:21 sensitive.txt
```

**5. How can you recursively change the permissions of a directory and all files/subdirectories within it?**
Applying permissions across a whole folder tree needed the recursive flag.
`chmod -R 755` walks into `configs_copy` and every file and folder underneath gets the new mode.
```bash
aditya-forge@LAPTOP-184D82EB:~$ chmod -R 755 configs_copy
aditya-forge@LAPTOP-184D82EB:~$ ls -lR configs_copy
configs_copy:
total 8
-rwxr-xr-x 1 aditya aditya   12 Aug 15 07:20 app.conf
drwxr-xr-x 2 aditya aditya 4096 Aug 15 07:20 mvdir1
```

**6. Explain the difference between `chmod +x` and `chmod u+x`.**
`chmod +x` adds execute permission for the owner, group, AND others all at once. `chmod u+x` adds execute permission for only the owner (user), leaving group and others unchanged.

---

## 15. `ps` (Report a Snapshot of Current Processes)

**1. What is the basic command to display processes owned by the current user?**
Checking what's running right now starts with the simplest process listing.
Plain `ps` only shows processes tied to the current terminal session.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ps
  PID TTY          TIME CMD
 2201 pts/0    00:00:00 bash
 2402 pts/0    00:00:00 ps
```

**2. How do you display information about all running processes on the system?**
For a fuller picture of everything on the system, I expanded the view.
`ps aux` lists every process from every user, and I piped it into `head` to keep the output short.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ps aux | head -6
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root         1  0.1  0.1  16004  4948 ?        Ss   07:19   0:01 /sbin/init
root       118  0.0  0.0      0     0 ?        S    07:19   0:00 [hwrng]
aditya     2201  0.0  0.1   8120  3892 pts/0    Ss   07:20   0:00 -bash
aditya     2402  0.0  0.0   7620  1560 pts/0    R+   07:22   0:00 ps aux
```

**3. What command shows a dynamic, real-time view of running processes?**
For a live, constantly updating view I switched tools entirely.
`top` refreshes in place and shows CPU and memory usage per process in real time.
```bash
aditya-forge@LAPTOP-184D82EB:~$ top
```
`top` refreshes continuously and re-sorts processes live (e.g. by CPU usage), whereas `ps aux` just prints a single static snapshot.

**4. How can you use `ps` and `grep` together to find information about a specific process, like `firefox`?**
Hunting for one specific process is easiest with a filter.
Piping `ps aux` into `grep firefox` narrows the huge list down to just that one program.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ps aux | grep firefox
aditya     3311  0.3  1.2 812345 98234 ?        Sl   09:14   0:12 /usr/bin/firefox
aditya     3402  0.0  0.0   7620  1560 pts/0    R+   09:20   0:00 grep firefox
```

**5. In the `ps aux` output, what do the USER, PID, and %CPU columns represent?**
USER is the owner of the process. PID is the unique Process ID assigned by the kernel. %CPU is the percentage of CPU time the process is currently consuming.

**6. How would you display a forest-like hierarchy of processes, showing the parent-child relationships?**
I also wanted to see the process tree, not just a flat list.
`ps -ejH` shows the parent child hierarchy, so you can see what spawned what.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ps -ejH | head -6
  PID  PGID   SID TTY          TIME CMD
    1     1     1 ?        00:00:01 systemd
  118     1     1 ?        00:00:00   udevd
 2201  2201  2201 pts/0    00:00:00 bash
```
(`ps -ejH` or `ps axjf` indent child processes beneath their parents, forming a tree view.)

---

## 16. `cat` (Concatenate and Print Files)

**1. What is the command to display the contents of a file named `notes.txt` on the screen?**
Before editing anything, I checked what the file currently held.
`cat notes.txt` just prints it out so I have a baseline to compare against.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat notes.txt
Note 1
Note 2
```

**2. How can you use `cat` to create a new file and type its content directly from the keyboard?**
This time I used `cat` to actually write new content into a file.
`cat > newnotes.txt` waits for typed input and saves it once you hit `Ctrl+D`.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat > newnotes.txt
Typed content here
(press Ctrl+D to save and exit)
```

**3. How would you combine the contents of `file1.txt` and `file2.txt` into a new file called `combined.txt`?**
Combining a couple of files into one is straightforward with a redirect.
`cat file1.txt file2.txt > combined.txt` concatenates them in order and writes the result out.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat file1.txt file2.txt > combined.txt
aditya-forge@LAPTOP-184D82EB:~$ cat combined.txt
Content of file one
Content of file two
```

**4. What is a major drawback of using `cat` to view very large files?**
`cat` dumps the entire file to the screen all at once with no pausing or scrolling control, so for a very large file it floods the terminal instantly, making it impractical to read. Tools like `less` or `more` are better for that, since they page through content.

**5. How can you number all the output lines when displaying a file with `cat`?**
Line numbers make it easier to reference specific parts of a file later.
`cat -n` prefixes every line of `combined.txt` with its number.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat -n combined.txt
     1	Content of file one
     2	Content of file two
```

**6. How is the `tac` command different from `cat`?**
Just for comparison, I also printed the file backwards.
`tac` is `cat` spelled backwards for a reason, it prints the lines in reverse order, last first.
```bash
aditya-forge@LAPTOP-184D82EB:~$ tac combined.txt
Content of file two
Content of file one
```
`tac` ("cat" spelled backwards) prints a file's lines in reverse order, last line first, instead of top to bottom.

---

## 17. `sed` (Stream Editor for Filtering and Transforming Text)

**1. What is the command to replace the first occurrence of "apple" with "orange" on each line of `fruit.txt`?**
Before running a find and replace, I looked at the file as it stood.
`cat fruit.txt` shows the word apple sitting in there, which is what I'm about to swap out.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat fruit.txt
I like apple pie
apple and apple juice
no fruit here
aditya-forge@LAPTOP-184D82EB:~$ sed 's/apple/orange/' fruit.txt
I like orange pie
orange and apple juice
no fruit here
```

**2. How would you replace every occurrence of "apple" on a line?**
`sed` handles text substitution without needing to open an editor at all.
This prints the file with every apple replaced by orange, but the original file itself is untouched.
```bash
aditya-forge@LAPTOP-184D82EB:~$ sed 's/apple/orange/g' fruit.txt
I like orange pie
orange and orange juice
no fruit here
```

**3. How can you delete all lines that contain the word "debug" from a file?**
I checked another file before running a similar substitution on it.
Same idea, just confirming the starting content of `debugfile.txt` first.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat debugfile.txt
line one
debug: value=5
line three
debug: checking
line five
aditya-forge@LAPTOP-184D82EB:~$ sed '/debug/d' debugfile.txt
line one
line three
line five
```

**4. What option is used to edit the file in-place (i.e., save changes back to the original file)?**
This time I made the substitution stick instead of just previewing it.
The `-i` flag tells `sed` to edit `fruit.txt` in place rather than only printing the changed version.
```bash
aditya-forge@LAPTOP-184D82EB:~$ sed -i 's/apple/orange/g' fruit.txt
aditya-forge@LAPTOP-184D82EB:~$ cat fruit.txt
I like orange pie
orange and orange juice
no fruit here
```

**5. How would you use `sed` to delete only the third line of a file?**
Before pulling out a specific range of lines, I looked at the whole file.
`threelines.txt` has more content than the name suggests, so this is the starting point.
```bash
aditya-forge@LAPTOP-184D82EB:~$ cat threelines.txt
1
2
3
4
5
aditya-forge@LAPTOP-184D82EB:~$ sed '3d' threelines.txt
1
2
4
5
```

**6. How can you print only lines 10 to 20 of a file using `sed`?**
Grabbing just a slice of a large file is easy with `sed`'s address range.
`sed -n '10,20p'` prints only lines 10 through 20 and suppresses everything else.
```bash
aditya-forge@LAPTOP-184D82EB:~$ sed -n '10,20p' numbered.txt
10
11
12
13
14
15
16
17
18
19
20
```

---

## 18. `tar` (Tape Archiver)

**1. What is the command to create a compressed archive named `backup.tar.gz` from a directory named `myfolder`?**
Bundling a folder into a single compressed archive is a classic `tar` job.
`tar -czvf backup.tar.gz myfolder` creates, gzips and lists each file as it's added.
```bash
aditya-forge@LAPTOP-184D82EB:~$ tar -czvf backup.tar.gz myfolder
myfolder/
myfolder/b.txt
myfolder/a.txt
```

**2. How would you list the contents of a `tar` archive without extracting it?**
Before extracting anything, I like to check what's actually inside an archive.
`tar -tvf` lists the contents without unpacking a single file.
```bash
aditya-forge@LAPTOP-184D82EB:~$ tar -tvf backup.tar.gz
drwxr-xr-x aditya/aditya       0 2026-08-15 07:21 myfolder/
-rw-r--r-- aditya/aditya       7 2026-08-15 07:21 myfolder/b.txt
-rw-r--r-- aditya/aditya       7 2026-08-15 07:21 myfolder/a.txt
```

**3. What is the command to extract the contents of `archive.tar.gz` into the current directory?**
Now I actually pulled the files back out of the archive.
`tar -xzvf` reverses the `-c`, decompressing and extracting everything into the current folder.
```bash
aditya-forge@LAPTOP-184D82EB:~$ tar -xzvf backup.tar.gz
myfolder/
myfolder/b.txt
myfolder/a.txt
aditya-forge@LAPTOP-184D82EB:~$ ls myfolder
a.txt  b.txt
```

**4. What is the difference between the `-z` and `-j` flags?**
`-z` compresses/decompresses using `gzip` (producing `.tar.gz`). `-j` uses `bzip2` instead (`.tar.bz2`), since `bzip2` usually compresses smaller but is noticeably slower than `gzip`.

**5. How would you create an archive using absolute paths and then extract it to a specific, different directory?**
I also archived a folder using its full absolute path this time.
Same flags as before, just pointing at `/home/aditya/myfolder` instead of a relative path.
```bash
aditya-forge@LAPTOP-184D82EB:~$ tar -czvf archive.tar.gz /home/aditya/myfolder
tar: Removing leading `/' from member names
home/aditya/myfolder/
home/aditya/myfolder/b.txt
home/aditya/myfolder/a.txt
aditya-forge@LAPTOP-184D82EB:~$ mkdir -p restore_target
aditya-forge@LAPTOP-184D82EB:~$ tar -xzvf archive.tar.gz -C restore_target
home/aditya/myfolder/
home/aditya/myfolder/b.txt
home/aditya/myfolder/a.txt
```
`tar` strips the leading `/` from absolute paths by default (for safety) and re-creates the path relative to whatever `-C` directory you extract into.

**6. How can you add a new file to an existing `tar` archive?**
Adding one more file to an existing archive doesn't require rebuilding the whole thing.
`tar -rvf` appends `c.txt` onto `backup.tar`, though this only works on uncompressed archives.
```bash
aditya-forge@LAPTOP-184D82EB:~$ tar -rvf backup.tar c.txt
c.txt
aditya-forge@LAPTOP-184D82EB:~$ tar -tvf backup.tar
drwxr-xr-x aditya/aditya      0 2026-08-15 07:21 myfolder/
-rw-r--r-- aditya/aditya      7 2026-08-15 07:21 myfolder/b.txt
-rw-r--r-- aditya/aditya      7 2026-08-15 07:21 myfolder/a.txt
-rw-r--r-- aditya/aditya      7 2026-08-15 07:21 c.txt
```
`-r` (append) only works on an uncompressed `.tar` archive, not a compressed `.tar.gz`.

---

## 19. `ip` (Show / Manipulate Routing, Network Devices, Interfaces)

**1. What is the modern replacement for the old `ifconfig` command?**
The `ip` command (from the `iproute2` suite), specifically `ip addr` / `ip link`, is the modern replacement for the deprecated `ifconfig`.

**2. How do you show the IP addresses and state of all network interfaces?**
Checking network config these days means reaching for `ip` rather than the old `ifconfig`, so I started by listing every interface along with its assigned addresses.
`ip addr show` prints each interface's state and IP information in one pass.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ip addr show
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN
    inet 127.0.0.1/8 scope host lo
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP
    inet 172.20.10.5/24 brd 172.20.10.255 scope global eth0
```

**3. What command shows the routing table for the system?**
Next I checked how traffic actually gets routed off this machine.
`ip route show` prints the routing table, including which interface is the default gateway.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ip route show
default via 172.20.10.1 dev eth0
172.20.10.0/24 dev eth0 proto kernel scope link src 172.20.10.5
```

**4. How would you show statistics (like bytes transferred) for a specific interface, like `eth0`?**
I also pulled detailed statistics for one specific interface.
`ip -s link show eth0` reports packet and byte counts for both sent and received traffic.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ip -s link show eth0
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP
    RX: bytes  packets  errors  dropped overrun mcast
    8213904    6210     0       0       0       12
    TX: bytes  packets  errors  dropped carrier collsns
    1042221    4310     0       0       0       0
```

**5. How can you bring a network interface `eth0` down (disable it) or up (enable it)?**
Finally I toggled the interface off using `sudo`, since that needs elevated permissions.
`ip link set eth0 down` disables it, and the same command with `up` would bring it back.
```bash
aditya-forge@LAPTOP-184D82EB:~$ sudo ip link set eth0 down
aditya-forge@LAPTOP-184D82EB:~$ sudo ip link set eth0 up
```

---

## 20. `ssh` (OpenSSH SSH Client, Remote Login)

**1. What is the basic command to connect to a remote server named `server.example.com` as the user `john`?**
Connecting to a remote machine is what `ssh` is for.
`ssh john@server.example.com` opens a login session on that host as the user john.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ssh john@server.example.com
john@server.example.com's password: 
Welcome to Ubuntu 22.04 LTS
john@server:~$ 
```

**2. How can you specify a different port (e.g., `2222`) to connect to on the remote host?**
Some servers listen on a nonstandard port, so I specified it explicitly.
The `-p` flag tells `ssh` which port to connect on instead of assuming the default 22.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ssh -p 2222 john@server.example.com
```

**3. How would you execute a single command on a remote server without opening an interactive shell, for example, to check its uptime?**
I also ran a single command remotely without opening a full shell session.
Tacking `uptime` onto the end of the `ssh` command runs just that, then returns you to your own terminal.
```bash
aditya-forge@LAPTOP-184D82EB:~$ ssh john@server.example.com uptime
 09:41:03 up 12 days,  3:20,  2 users,  load average: 0.08, 0.05, 0.01
```

**4. What is the purpose of the `~/.ssh/config` file, and what is one common setting you can define there?**
`~/.ssh/config` lets you define shortcuts and per-host defaults for `ssh` so you don't have to retype long flags every time. A common setting is `Host myserver` with `HostName server.example.com`, `User john`, and `Port 2222` underneath it, letting you just type `ssh myserver`.

**5. How can you use SSH to securely copy files from your local machine to a remote server?**
Copying a file to a remote machine works a lot like `cp`, just over the network.
`scp document.txt john@server.example.com:/home/john/` sends the file straight to that path on the remote host.
```bash
aditya-forge@LAPTOP-184D82EB:~$ scp document.txt john@server.example.com:/home/john/
document.txt              100%   28     1.2KB/s   00:00
```
