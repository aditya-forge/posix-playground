#!/bin/bash 
if [ $# -eq 0 ]; then 
    echo "Usage: $0 <filename>" 
    exit 1 
fi 
  
file="$1" 
  
if [ ! -f "$file" ]; then 
    echo "File $file not found!" 
    exit 1 
fi 
  
echo "Checking spellings in $file..." 
if command -v aspell > /dev/null; then 
    aspell list < "$file" | sort -u 
elif command -v spell > /dev/null; then 
    spell "$file" 
else 
    echo "No spell utility found. Words in document:" 
    tr -cs 'a-zA-Z' '\n' < "$file" | tr '[:upper:]' '[:lower:]' | sort -u 
fi