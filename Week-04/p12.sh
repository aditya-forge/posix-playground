#!/bin/bash 
if [ $# -eq 0 ]; then 
    echo "Please provide a string argument." 
    echo "Usage: $0 <string>" 
    exit 1 
fi 
  
str="$1" 
echo "Length of '$str' is ${#str}"