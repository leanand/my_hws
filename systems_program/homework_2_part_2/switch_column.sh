#!/bin/bash
FILENAME=$1
awk -v COLS1=$2 -v COLS2=$3 'BEGIN{
  print "\n==="
  }
  {
  if (NF >= COLS1 && NF >= COLS2){
    TEMP = $COLS1
    $COLS1 = $COLS2
    $COLS2 = TEMP
    print $0
  }
  else
    print $0
}' $FILENAME  >> $FILENAME