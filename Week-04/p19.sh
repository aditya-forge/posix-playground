#!/bin/bash 
if [ $# -lt 2 ]; then 
    echo "Usage: $0 <input_file> <output_file>" 
    exit 1 
fi 
  
infile="$1" 
outfile="$2" 
  
if [ ! -f "$infile" ]; then 
    echo "Input file $infile not found!" 
    exit 1 
fi 
  
tr -d ' ' < "$infile" > "$outfile" 
  
echo "Spaces removed. Output written to $outfile"