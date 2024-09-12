#!/usr/bin/env python3
'''
Author :       Henso ABREU
Description:    Script to dump Octupus(Sampic) and Picmic ascii data produced by PICMIC0/SAMPIC to an ASCII file 
                "version 1"
'''
import pandas as pd
import numpy as np
import sys
import os
import argparse
import re
import struct
from datetime import date,time
#import picmic_modules as prepro
import csv
from termcolor import colored
import glob
from statistics import mean 
from statistics import median
from statistics import fmean
from decimal import Decimal

headers = ["nbPixels","timeStamp1","timeStamp2","listPixels"]
CDW = os.getcwd()   # Actual directory
##ascii_files = './data_ascii'

##########################################
def read_lines_in_bunches(file_path, skip_lines=7, bunches=3):
    with open(file_path, 'r') as file:
        # Skip the first Num Lines
        for _ in range(skip_lines):
            next(file)
        
        while True:
            lines = [file.readline().strip() for _ in range(bunches)]
            if not any(lines):
                break
            yield lines

##########################################
def dumpData(list1, list2, list3, list4) :
    myList = []
    myList.append(list1)
    myList.append(list2)
    myList.append(list3)
    myList.append(list4)
    return myList

##########################################
def hex_block_to_decimal(hex_block):
    #hex_digits = hex_block.replace('.', '')  # Remove the point if present
    # Extract the first 2 and last 2 digits
    last_two_digits = hex_block[:2]
    first_two_digits = hex_block[-2:]
    # Convert the digits to integers
    first_decimal = int(first_two_digits, 16)
    last_decimal = int(last_two_digits, 16)
    return [first_decimal, last_decimal]
###########################################
def listOfList2String(bigList):
    result_string = []
    for sublist in bigList:
        result_string.extend(sublist)
           
    stringsave =''
    dimension = len(result_string);
    for i,idx in enumerate(result_string):
        if (i < dimension):
            stringsave+= str(idx)+" "
        else :
            stringsave+=str(idx)
    
    del(result_string)
    return stringsave.strip()
###########################################
def create_string_array(input_string):
    # Split the input string by spaces
    number_pairs = input_string.split()
    
    # Ensure that the list has an even number of elements
    if len(number_pairs) % 2 != 0:
        raise ValueError("The input string must contain an even number of numbers.")
    
    # Convert the list into pairs of strings
    string_array = [number_pairs[i]+' '+ number_pairs[i + 1] for i in range(0, len(number_pairs), 2)]
    
    return string_array
###########################################
def compare_and_add_elements(l1, l2):
    # Use a set for l2 for faster lookup
    set_l2 = set(l2)
    
    # Iterate through l1 and check if any element is not in l2
    for element in l1:
        if element not in set_l2:
            # If element is not found in l2, add it to l2
            l2.append(element)
            set_l2.add(element)  # Update the set to include the new element
    
    return l2
############################################
##def cleanSampicFile(flist) :
##    for file in flist :
    

def main():
    
    parser = argparse.ArgumentParser()
    parser.add_argument("-fsampic", "--file_sampic",help="provide the octupus input file produced by OCTUPUS/SAMPIC")
    parser.add_argument("-o", "--outDir", help="provide the output folder to save the processed ASCII data")

    ##parser.add_argument("-fpicmic", "--file_picmic",help="provide the picmic input file produced by PICMIC0/SAMPIC")
    ##parser.add_argument("PARAMS", nargs='+')
    ##parser.add_argument("-o", "--outDir", help="provide the output folder to save the processed ASCII data")
    ##args, unknown = parser.parse_known_args()
    args = parser.parse_args()
    
    if ( (str(args.file_sampic)=='None') ) : #& (str(args.file_picmic)=='None')) :
        print("----------------------- >>>>>>>>>>>>>>>> Input files -- Mandatory   <<<<<<<<<<<<<<<<<<<-------------------------")
        print('Script not executed')
        exit()

    
    ##if not sys.stdin.isatty():
    ##    args.PARAMS.extend(sys.stdin.read().splitlines())
     
    
    #filenames = glob.glob(args.file_sampic)
    ##print(args.file_sampic) 
    
    ##print('------------------------')
    #print(filenames)
    ##file = open(args.file_sampic,"rb")
    '''
    with open (args.file_sampic) as file:
        #head=file.readline()[6:]; ## line1
        reader = file.readlines()
        for line in reader[7:] :
            print(line)
    
        file.close()
        exit()
    '''
    headers = ['Hit','UnixTime','Channel','Cell0Time','RawTOTValue','TOTValue','Time','Baseline','RawPeak','Amplitude','DataSample']
    
    file_path = args.file_sampic
    hit=[]
    unixtime = []
    ch =[]
    cell0time = []
    rawtotvalue = []
    totvalue = []
    time = []
    baseline = []
    rawpeak = []
    amplitude = []
    ##size = []
    datasample = []
    for bunch in read_lines_in_bunches(file_path):
        '''
        print(bunch)
        print('Hit:', bunch[0].split(' ')[1])
        print('Unix Time:', bunch[0].split(' ')[-1])
        print('Channel:',bunch[1].split(' ')[1])
        print('Cell0Time:',bunch[1].split(' ')[3])
        print('RawTOTValue:',bunch[1].split(' ')[5])
        print('TOTValue:',bunch[1].split(' ')[7])
        print('Time:',bunch[1].split(' ')[9])
        print('Baseline:',bunch[1].split(' ')[-7])
        print('RawPeak:',bunch[1].split(' ')[-5])
        print('Amplitude:',bunch[1].split(' ')[-3])
        ##print('Size:',bunch[1].split(' ')[-1])
        '''
        
        print('Cell0Time: ',bunch[1].split(' ')[3])
        print('Cell0Time:-',Decimal(bunch[1].split(' ')[3]))
        
        ##print('RawTOTValue:',bunch[1].split(' ')[5])
        hit.append(int(bunch[0].split(' ')[1]))
        unixtime.append(float(bunch[0].split(' ')[-1]))
        ch.append(int(bunch[1].split(' ')[1]))
        ##cell0time.append(float(bunch[1].split(' ')[3]))
        cell0time.append(Decimal(bunch[1].split(' ')[3]))
        rawtotvalue.append(float(bunch[1].split(' ')[5]))
        totvalue.append(float(bunch[1].split(' ')[7]))
        time.append(float(bunch[1].split(' ')[9]))
        baseline.append(float(bunch[1].split(' ')[-7]))
        rawpeak.append(float(bunch[1].split(' ')[-5]))
        amplitude.append(float(bunch[1].split(' ')[-3]))
        this_data = [float(string) for string in bunch[2].split()[1:]]
        datasample.append(this_data)
        #datasample.append(np.float_(this_data))
        #datasample = eval(datasam)
    
    myDict = dict(zip(headers,[hit,unixtime,ch,cell0time,rawtotvalue,totvalue,time,baseline,rawpeak,amplitude,datasample]))
    
    ##print(myDict)
    
    df2Csv = pd.DataFrame.from_dict(myDict)
    print(df2Csv)
    
    df2Csv.to_csv(args.outDir+'SampicData.csv', index=False)
    exit()
    
    '''
        
    
        
        this_data = [float(string) for string in bunch[2].split()[1:]]
        print('mean=',mean(this_data))
        print('median=',median(this_data))
        print('fmean=',fmean(this_data))
        print('maximum',max(this_data))
        print('minimum',min(this_data))
        #print(this_data[1:])
        ##print(this_data)
        ##for idx in bunch :
        ##print(bunch)
        print(20*'-')
        
    
    ##with open (args.file_sampic) as file:
    ##    while True:
    ##        lines = [file.readline().strip() for _ in range(3)]
    ##        if not any(lines):
    ##            break
    ##        hell0 = yield lines
    ##        print(hell0)
            
     
    # loading the tailed file
    #for f in args.PARAMS :
    ##for f in args.file_sampic :
        # variable defintions
    ##    dump =1
    ##    mat=[]
    ##    numPixelsList = []
    ##    allPixelsList = []
    ##    timeStampList = []
    ##    timeStampList2 = []:q!
    ##    totalEvts=0
        
    ##    testList = []
        
        ##inFileName = f.split('/')[0]
        ##outFileName = inFileName.split('.')[0]+'.csv'
    
        # inFile
        ##print('f:',f)
        ###file = open(f,"rb")
    
        ## Reading information from the file comments
        ###head=file.readline(); ## line1
        ###print(head,'----------------------- 1')
        ###infoFromComments  = str(head).split("==")[2].split("=")[1:]
        ###runInfo = [i.split(' ')[1] for i in infoFromComments]
        ##print('infoFromComments')
        ##print(infoFromComments)
        
    ''' ##print(head,'----------------------- 2') '''
    ''' freq = int(str(head).split("==")[-2].split(' ')[4]) '''

    ''' ## lines 3  '''
    ''' head=file.readline() # 3 #=== DATA STRUCTURE PER FRAME=== '''
    ''' ##print(head,'----------------------- 3') '''
    ''' newVarValues = [int(i.split(' ')[1]) for i in str(head).split(':')[2:] ] '''
    ''' newVarNames = [ j.split(' ')[-1].strip() for j in str(head).split(':')[1:]] '''
    ''' dictNewVars = dict(zip(newVarNames,newVarValues)) '''

    ''' head=file.readline() # line 4 # === NB_OF_PIXELS_IN_FRAMES (2 bytes) RAW_TIMESTAMP (in fe_clock_periods) (5 Bytes), PIXEL_COLUMN (1 byte), PIXEL ROW ( 1 byte) == '''
    ''' ##print(head,'----------------------- 4') '''
    ''' head=file.readline() # line 5 # '''
    ''' ##print(head,'----------------------- 5') '''
    ''' head=file.readline() # line 6 # '''
    ''' ##print(head,'----------------------- 6') '''
    
    ''' count = 0 '''
      
    '''  '''
    '''  '''
    ''' while dump :  '''
    '''     count +=1 '''

    '''     line=file.readline() # line 7 # '''
    '''     #print(line,'----------------------- 7') '''
    '''      '''
    '''     if not line: '''
    '''         dump = False '''
    '''         break '''
    '''      '''
    '''     nFrames = int(str(line[:18]) .split(' ')[-2].strip()) '''
    '''     #print('nFrames=',nFrames) '''
    '''      '''
    '''     unixTime = float(str(line).split(" ")[-2]) '''
    '''     #print('unixTime=',unixTime) '''
    '''      '''
    '''     lastTime= [] '''
    '''     dtime = 0 '''
    '''     mlist0 = None '''
    '''     mlistx = None '''
    '''     for jdx in range(nFrames): '''
    '''         #print('--> Frame=',jdx) '''
    '''      '''
    '''         #unixTime = float(str(line).split(" ")[-2]) '''
    '''         #print('unixTime=',unixTime) '''
    '''  '''
    '''         line=file.readline() # line 8 # '''
    '''          '''
    '''         #print(line,'----------------------- 8') '''
    '''         timeStamp=int(str(line).split(" ")[-2]) '''
    '''         #print('TIME-STAMP:',timeStamp) '''
    '''         lastTime.append(timeStamp)  '''
    '''          '''
    '''         if ( nFrames>1 ): '''
    '''             dtime=int(lastTime[jdx])-int(lastTime[jdx-1]) '''
    '''             #print('DeltaTime:',dtime,', jdx',lastTime[jdx], ', jdx -1',lastTime[jdx-1], 'real jdx', jdx) '''
    '''          '''
    '''         #dtime = abs(timeStamp - dtime) '''
    '''         nbPixels = int(str(line).split(" ")[-4]) '''
    '''      '''
    '''         line = str(file.readline()).replace('.','') '''
    '''     ##print(head,'----------------------- 9') '''
    '''      '''
    '''         RCs = line[2:-3].split(" ") '''
    '''         mat = [hex_block_to_decimal(i) for i in RCs]  '''
    '''      '''
    '''         spixels = listOfList2String(mat)    '''
    '''             ##print(str(nbPixels),spixels)   '''
    '''             if (jdx == 0) : '''
    '''                 mlist0 = create_string_array(spixels) '''
    '''                 #print('mlist0:',mlist0) '''
    '''           else : '''
    '''             mlistx = create_string_array(spixels) '''
    '''             #print('mlistx:',mlistx) '''
    '''             if (dtime<6400): '''
    '''                 compare_and_add_elements(mlistx, mlist0) '''
    '''         #print(result) '''
    '''         #print('mlist0:',mlist0) '''
    '''         #print('~ ~ ~ ~ ~ ~ ~ ~ ') '''
    '''          '''
    '''         ##spixels = listOfList2String(mat) '''
    '''         toprint = '' '''
    '''         for i in mlist0: '''
    '''             toprint=i+' '+toprint '''
    '''         print(len(mlist0),toprint.strip()) '''

    '''          '''
    ##            
    ##        ##print('---------------------------------------------------------')
    ##    
    ##    ##file.close()
    ##exit()
    
##########################################    
if __name__ == "__main__":
    main()
