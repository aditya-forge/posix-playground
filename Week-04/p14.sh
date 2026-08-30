#!/bin/bash 
for i in 1 2 3 
do 
    echo "--- Process Snapshot #$i ---" 
    date 
    ps -ef 2>/dev/null | head -n 15 
    echo "" 
  
    if [ $i -lt 3 ]; then 
        echo "Waiting 30 seconds..." 
        sleep 30 
    fi 
done