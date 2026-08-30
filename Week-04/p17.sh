#!/bin/bash 
if [ -z "$1" ]; then 
    echo -n "Enter file name to encrypt: " 
    read file 
else 
    file="$1" 
fi 
  
if [ ! -f "$file" ]; then 
    echo "File $file does not exist!" 
    exit 1 
fi 
  
outfile="$file.enc" 
  
echo "Encrypting $file to $outfile..." 
openssl enc -aes-256-cbc -pbkdf2 -in "$file" -out "$outfile" 
  
if [ $? -eq 0 ]; then 
    echo "File encrypted successfully as $outfile" 
else 
    echo "Encryption failed!" 
fi