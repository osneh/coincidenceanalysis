#!/bin/bash

# Check if at least one parameter is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <parameter>"
  exit 1
fi

# Store parameters
FILE=$1
OUTDIR=$2

# define outfiles
FILE0="$OUTDIR"Picmic0.csv""
FILE1="$OUTDIR"Picmic0_noSingleStripEvents.csv""
FILE3="$OUTDIR"processedIntersectionsPicmic.csv""

##python src/ascii_readDataPicmic_bin2ascii_improved.py -f data/sampic_run27/picmic_data/picmic_data_20240721_23_44_19.txt -o /home/habreu/WORK/coincidenceanalysis/data/
## Running scripts to decode Picmic data
python src/ascii_readDataPicmic_bin2ascii_improved.py -f $FILE -o $OUTDIR

## filtering single Strip Events
cat $FILE0 | grep -v '^1,' > $FILE1

## computing intersections , lines and centroid
python src/analysePicmicSampicV1.py $FILE1

## merging plots 
ls -ltrh *png | sed 's|.* lines|lines|g' > temp.txt
convert $(cat temp.txt) myPlots.pdf
rm -rf temp.txt xlines.csv inter.csv
rm -rf *png

##mv myPlots.pdf $OUTDIR

