#!/bin/bash 
if [ -z "$1" ]; then 
    echo -n "Enter file path: " 
    read file 
else 
    file="$1" 
fi 
  
if [ ! -e "$file" ]; then 
    echo "File does not exist!" 
    exit 1 
fi 
  
echo "Last modification time of $file:" 
date -r "$file"