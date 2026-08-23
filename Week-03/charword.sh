#!/bin/bash
if [ $# -ne 1 ]; then
  echo "Usage: $0 filename"
  exit 1
fi
file=$1
line_no=1
while IFS= read -r line
do
  chars=${#line}
  words=$(echo "$line" | wc -w)
  echo "Line $line_no: Characters = $chars, Words = $words"
  line_no=$((line_no + 1))
done < "$file"