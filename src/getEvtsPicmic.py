#!/usr/bin/python
import pandas as pd
import numpy as np

dataPicmic = pd.read_csv('Picmic0_noSingleStripEvents.csv')

for i in range(dataPicmic.index.stop):
    print(str(dataPicmic.nbPixels[i])+" "+dataPicmic.listPixelsString[i])
