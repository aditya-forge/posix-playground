#!/bin/bash
echo "Enter how many numbers:"
read n
sum=0
for (( i=1; i<=n; i++ ))
do
  echo "Enter number $i:"
  read num
  sum=$((sum + num))
done
avg=$(echo "scale=2; $sum / $n" | bc)
echo "Sum = $sum"
echo "Average = $avg"