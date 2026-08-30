#!/bin/bash 
if [ $# -eq 0 ]; then 
    echo "Usage: $0 file1 file2 ..." 
    exit 1 
fi 
  
for file in "$@" 
do 
    if [ -f "$file" ]; then 
        dir=$(dirname "$file") 
        base=$(basename "$file") 
        lower=$(echo "$base" | tr '[:upper:]' '[:lower:]') 
  
        if [ "$base" != "$lower" ]; then 
            mv "$file" "$dir/$lower" 
            echo "Renamed: $base -> $lower" 
        else 
            echo "Already lowercase: $base" 
        fi 
    fi 
done