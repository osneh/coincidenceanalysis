#!/usr/bin/python
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import os
import math
import random
import time
import sys

inFile = sys.argv[1]
print(inFile)

thresholds = [20]

def generate_random_numbers(n):
    random_numbers = [random.randint(-2, 2) for _ in range(n)]
    return random_numbers

def count_true_false(lst):
    true_count = lst.count(True)
    false_count = lst.count(False)
    return true_count, false_count

def distance(p1,p2):
    x1,y1=p1
    x2,y2=p2
    return math.sqrt( math.pow(x2-x1,2) + math.pow(y2-y1,2))

tcolor = ['red','blue','gold','green','magenta','black','orange','tan','peru','cyan','pink','olive','purple','brown','lime','grey']  
mymarker = ['X','o','s','h','H','D','d','1','3','4','8','+', 'v', '^', '<', '>', 'p', '*', 'P','x','|','_','2','.']

color_marker = []
for i in mymarker :
    for j in tcolor :
        color_marker.append([j,i])


##dataPicmic = pd.read_csv('/home/habreu/WORK/dataImad_23Jul2024/Picmic0_noSingleStripEvents.csv')
dataPicmic = pd.read_csv(inFile)


ifile= 'inter.csv'
cfile='clusters.csv'
mypathI= os.path.isfile(ifile)
mypathC= os.path.isfile(cfile)

xyList = []
wList = [] ##list(data.track)
xCent = []
yCent = []
pt0List=[]
pt1List=[]
idxs = []

XC = -999999
YC = -999999

for i in range(dataPicmic.index.stop):
##for i in [1641,1642,1643,1644]:
#
# for i in [1641,1642,1643,1644]:
##for i in [1,2,3,4]:
    
    tmpPt0 = []
    tmpPt1 = []
    tmpXY = []
    
    idxs.append(i)                                                                  # Filling events from picmic 
    pixels = int(dataPicmic.nbPixels[i])
    if (pixels>1):
        print('----------------------   Event ',i,'  ---------------------------')
        print(str(dataPicmic.nbPixels[i])+" "+dataPicmic.listPixelsString[i])
        cdm =str('./clustering/xypicmic.exe ')+str(thresholds[0])+str(' ')+str(dataPicmic.nbPixels[i])+str(' ')+str(dataPicmic.listPixelsString[i])
        print(cdm)
        os.system(cdm)
        ## ## ---------------------------------------------------------------------------------------------------------------- ## ##
        ## ## ---------------------------------------------------------------------------------------------------------------- ## ##
        ##''''
        ##data = pd.read_csv("clustering/xlines.csv",delimiter=";")
        data = pd.read_csv("xlines.csv",delimiter=";")
        wList.append(list(data.track))
        

        ##if (mypathI):
        if os.path.getsize(ifile) > 0:
            ##print('HERE 1')
            df= pd.read_csv(ifile, delimiter=";")

        if (mypathC):
            df_clus = pd.read_csv(cfile,delimiter=';')
        
        for idx in range(data.index.stop) :
            color='black'
            way=data.track[idx]
    
            if way[0]=='B' :
                tcolor='blue'
            elif way[0]=='R':
                tcolor='red'
            elif way[0]=='Y':
                tcolor='gold'
        
            line0 = (eval(data.pt0[idx]),eval(data.pt1[idx]))
            xs,ys = zip(*line0)
            #wList.append(way)
            #print('way:',way,'pt0(',xs[0],',',ys[0],'), pt1(',xs[1],',',ys[1],')')
            ##print('way:',way,'pt0(',xs[0],',',ys[0],'), pt1(',xs[1],',',ys[1],')')
            tmpPt0.append([xs[0],ys[0]])
            tmpPt1.append([xs[1],ys[1]])
            ##pt0List.append([xs[0],ys[0]])
            ##pt1List.append([xs[1],ys[1]])
            plt.plot(xs,ys,'--', markersize=0, color=tcolor, linewidth=.3)
        pt0List.append(tmpPt0)
        pt1List.append(tmpPt1)
        
    #if (mypathI):
    if os.path.getsize(ifile) > 0:
        ##print('HERE 11')
        dim = df.index.stop
        ##print('dim:',dim)
        fillCounter = -1
        count = 0 
    
        XC = 0
        YC = 0
    
        for j in range(dim):
            px = df.x[j]
            py  = df.y[j]
            XC+=px
            YC+=py
            ##xyList.append([px,py])
            tmpXY.append([px,py])
        ##plt.scatter(px,py,2,color='black',marker='x')
        ##plt.text(px+generate_random_numbers(1), py+generate_random_numbers(1), str(i), fontsize=8, ha='right', va='bottom')
        xyList.append(tmpXY)
        XC =XC/dim
        YC = YC/dim
        xCent.append(round(XC,2))
        yCent.append(round(YC,2))
        print('X,Y=',XC,YC)
    else : 
        XC = -999999
        YC = -999999
        xyList.append([-999999,-999999])
        xCent.append(round(-999999))
        yCent.append(round(-999999))

    ##print('dim from list:',len(xyList))
    ##print(xyList)
    ##print(30*'-')

    if XC!=-999999 and YC!=-999999 :
        ##print('Hola')
        print('Counter:',i,',Centroid:[',XC,',',YC,']')
        plt.plot(XC, YC, marker="*", markersize=7, markeredgecolor="black", markerfacecolor="lime",linestyle='None',label='('+str(round(XC,2))+','+str(round(YC,2))+')')
    plt.title('PICMIC$0$ Intersections, Centroid -- #inter:'+str(dim)+' ,counter='+str(i))

    plt.xlabel('X-axis [$\mu$m]')
    plt.ylabel('Y-axis [$\mu$m]')
    #plt.title('PICMIC$0$ Intersections, Centroid -- #inter:'+str(i))
    plt.legend()
    plt.savefig('lines_evt'+str(i)+'.png')
    ##plt.show()
    plt.close()


## ------------------------------------------------------------------------- ##
## -------------            Save Information        ------------------------ ##
## ------------------------------------------------------------------------- ##
print(40*'-')

allVars = ['idxPicmic','tracks','pt0','pt1','intersections','xCentroid','yCentroid']
megaList = []
megaList.append(idxs)
megaList.append(wList)
megaList.append(pt0List)
megaList.append(pt1List)
megaList.append(xyList)
megaList.append(xCent)
megaList.append(yCent)

mydict = dict(zip(allVars,megaList))
print(mydict)
print(40*'--')
##print(wList)
##print('--')
##print(pt0List)
##print()
##print(xCent)

dfFromDict = pd.DataFrame.from_dict(mydict)
dfFromDict.to_csv('data/processedIntersectionsPicmic.csv',index=False)

exit()
