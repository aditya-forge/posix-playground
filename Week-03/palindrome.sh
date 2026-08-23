#!/bin/bash
echo "Enter a string:"
read str
rev=$(echo "$str" | rev)
if [ "$str" == "$rev" ]; then
  echo "$str is a Palindrome"
else
  echo "$str is not a Palindrome"
fi