#!/bin/bash 
if [ -z "$1" ]; then 
    echo "Usage: $0 <input_file> [output_file]" 
    exit 1 
fi 
  
infile="$1" 
outfile="${2:-wordlist.txt}" 
  
tr '[:upper:]' '[:lower:]' < "$infile" | tr -cs 'a-zA-Z' '\n' | awk 'length($0) >= 5' | sort -u > "$outfile" 
  
echo "Wordlist extracted and saved to $outfile" 
echo "Total words: $(wc -l < "$outfile")"