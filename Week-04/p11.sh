#!/bin/bash 
if [ -n "$1" ]; then 
    str="$1" 
else 
    echo -n "Enter a string: " 
    read str 
fi 
  
len=${#str} 
  
if [ $len -lt 5 ]; then 
    echo "String '$str' is too short! It must have at least 5 characters (current length: $len)." 
else 
    echo "Valid string '$str' (length: $len)." 
fi