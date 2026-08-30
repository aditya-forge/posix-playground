#!/bin/bash 
if [ $# -ne 2 ]; then 
    echo "Usage: $0 <dir1> <dir2>" 
    exit 1 
fi 
  
dir1="$1" 
dir2="$2" 
  
for file in "$dir1"/*; do 
    if [ -f "$file" ]; then 
        fname=$(basename "$file") 
        target="$dir2/$fname" 
  
        if [ -f "$target" ]; then 
            if cmp -s "$file" "$target"; then 
                echo "Deleting $file (matches $target)" 
                rm "$file" 
            fi 
        fi 
    fi 
done