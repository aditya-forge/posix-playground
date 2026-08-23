#!/bin/bash
echo "Enter input:"
read input
if [[ $input =~ ^-?[0-9]+$ ]]; then
  echo "$input is a Number"
else
  echo "$input is a String"
fi