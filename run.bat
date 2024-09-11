#!/bin/bash

# Check if at least one parameter is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <parameter>"
  exit 1
fi

# Store parameters
FILE=$1
OUTDIR=$2

FILE0="$OUTDIR"Picmic0.csv""
FILE1="$OUTDIR"Picmic0_noSingleStripEvents.csv""

##python src/ascii_readDataPicmic_bin2ascii_improved.py -f data/sampic_run27/picmic_data/picmic_data_20240721_23_44_19.txt -o /home/habreu/WORK/coincidenceanalysis/data/

python src/ascii_readDataPicmic_bin2ascii_improved.py -f $FILE -o $OUTDIR

cat $FILE0 | grep -v '^1,' > $FILE1
