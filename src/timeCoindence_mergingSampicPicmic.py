import pandas as pd
import numpy as np
import ast
import matplotlib.pyplot as plt

BASELINE=0.780000
SAMPLING_FRECUENCY=4252

def convert_to_float_array(string):
    # Safely evaluate the string to a list
    float_list = ast.literal_eval(string)
    # Convert to numpy array or keep as list of floats
    return np.array(float_list)

def convert_string_to_list_of_lists(input_string):
    # Use ast.literal_eval to safely evaluate the string as a Python expression
    list_of_lists = ast.literal_eval(input_string)
    return list_of_lists

# retrieve Picmic processed data
df = pd.read_csv('data/Picmic0_noSingleStripEvents.csv')
df.listPixels = df.listPixels.apply(convert_string_to_list_of_lists)

picmic_unixtime_list = df['Time'].tolist()
##print(len(picmic_unixtime_list))

# retrieve Sampic processed data
df_octupus = pd.read_csv('data/SampicData.csv')
octopus_unixtime_list = df_octupus.Cell0Time.tolist()
octupus_ch_list = df_octupus.Channel.tolist()

# coincidence and keep it inside a dictionary 
lkey =[]
lval =[]
count2 = 0
for ipicmic, picmic_time in enumerate(picmic_unixtime_list):
    tmin = 999999.
    index_picmic = -1
    index_octopus =-1
    val_picmic = -999.
    val_octopus = -999.
    #for ioctopus,octopus_time in enumerate(octopus_unixtime_list) :
    for ioctopus_index in range(0,len(octopus_unixtime_list),4):
        #print('ipicimic:',ipicmic,',icoctopus:',ioctopus_index)
        myocttime = octopus_unixtime_list[ioctopus_index]
        dmin = abs(picmic_time-myocttime)
        ##print('dmin:',dmin,'-ipicmic:',ipicmic,',picmic_time:',picmic_time,'-ioctopus:',ioctopus_index,',octopus_tim:',octopus_time)
        if dmin < tmin :
            tmin = dmin
            index_picmic = ipicmic
            index_octopus = ioctopus_index
            val_picmic=picmic_time
            val_octopus=myocttime
            #print('ioctopus=',ioctopus_index)
    if (index_picmic>=0):
        count2+=1
        lkey.append(index_picmic)
        lval.append(index_octopus)
        ##print('picmic--> index:',index_picmic,', value min:',tmin,',index octopus:',index_octopus)
        ##print('picmic_unix-time:',val_picmic,',octopus_unix-tim:',val_octopus)
        ##print('picmic_Time:',df['Time'].at[index_picmic],',octopus_Time:',df_octupus['Time'].at[index_octopus], ', diff=', df['Time'].at[index_picmic] - df_octupus['Time'].at[index_octopus] )
    
    ##print('--------------------------------')
print('Total Counter Coincidence=',count2)
coincidenceDict=dict(zip(lkey,lval))
##print(coincidenceDict)

# process sampic data
df_octupus['CorrTime'] = -df_octupus['Cell0Time']+df_octupus['Time']

# ####################################
# preparing sampic data to be merged
# ####################################
myChannel = []
ch01 = []
ch02 = []
ch03 = []
ch12 = []
ch13 = []
ch23 = []

cell0Ch0 =[]
cell0Ch1 =[]
cell0Ch2 =[]
cell0Ch3 =[]

timeCh0 = []
timeCh1 = []
timeCh2 = []
timeCh3 = []

ch_temp0=[]
ch_temp1=[]
ch_temp2=[]
ch_temp3=[]

ampCh0 = []
ampCh1 = []
ampCh2 = []
ampCh3 = []

variables = ['ipicmic','ioctopus','diffTime01','diffTime02','diffTime03','diffTime12','diffTime13','diffTime23',
             'corrTime0','corrTime1','corrTime2','corrTime3','Amplitude0','Amplitude1','Amplitude2','Amplitude3',
             'cell0Time0','cell0Time1','cell0Time2','cell0_Time']
##listOfList = []
##for i in range(6) :
##    listOfList.append()


for idx_oct in coincidenceDict.values():
    ch_temp = []
    #ch_diff = []
    
    for z in range(4):
    ##    print(':-:-:-:-:-:-:-:-:-:-:-:-:-:--:-:-:-:-:-:-:-:-:-:-:-:-:-:')
    ##    ##print('index:',myJdx,',channel=',df_octupus.Channel[myJdx])
        zz  = z + idx_oct
        temp_time_ch = df_octupus.Channel[zz]
        
        if temp_time_ch == 0 :
            timeCh0.append(df_octupus.CorrTime.at[zz])
            ampCh0.append(df_octupus.Amplitude.at[zz])
            cell0Ch0.append(df_octupus.Cell0Time.at[zz])
        elif temp_time_ch == 1 :
            timeCh1.append(df_octupus.CorrTime.at[zz])
            ampCh1.append(df_octupus.Amplitude.at[zz])
            cell0Ch1.append(df_octupus.Cell0Time.at[zz])
        elif temp_time_ch == 2 :
            timeCh2.append(df_octupus.CorrTime.at[zz])
            ampCh2.append(df_octupus.Amplitude.at[zz])
            cell0Ch2.append(df_octupus.Cell0Time.at[zz])
        elif temp_time_ch == 3 :
            timeCh3.append(df_octupus.CorrTime.at[zz])
            ampCh3.append(df_octupus.Amplitude.at[zz])
            cell0Ch3.append(df_octupus.Cell0Time.at[zz])
    ##    print('index:',zz,',channel=',df_octupus.Channel[zz])
 
    count = 0 
    for jdx in range(3):
        myJdx = jdx+idx_oct
        #ch_temp=[]
   
        for kdx in range(jdx+1,4):
            #count+=1
            myKdx = kdx+idx_oct
            #print(myJdx,'-',myKdx)
            
            valJdxCh = df_octupus.Channel.at[myJdx]
            valKdxCh = df_octupus.Channel.at[myKdx]
            
            ch_temp.append(str(myJdx)+'ch='+str(valJdxCh)+'-'+str(myKdx)+' ch='+str(valKdxCh))
            ##valJdx = df_octupus['Time'].at[myJdx]
            ##valKdx = df_octupus['Time'].at[myKdx]
            valJdx = df_octupus['CorrTime'].at[myJdx]
            valKdx = df_octupus['CorrTime'].at[myKdx]

            #ch_diff.append(valJdx-valKdx)
            print(myJdx,'-',myKdx,'-->',valJdx,'~',valKdx,'-->',valJdx-valKdx,'-->',valJdxCh,'~~',valKdxCh)
            ##print(count)
            if ((valJdxCh == 0)&(valKdxCh == 1)) or ((valJdxCh == 1)&(valKdxCh == 0))  :
                ch01.append(valJdx-valKdx)
            elif ( (valJdxCh == 0)&(valKdxCh == 2) ) or ( (valJdxCh == 2)&(valKdxCh == 0) ) :
                ch02.append(valJdx-valKdx)
            elif ( (valJdxCh == 0)&(valKdxCh == 3) ) or ( (valJdxCh == 3)&(valKdxCh == 0) ) :
                ch03.append(valJdx-valKdx)
            elif ( (valJdxCh == 1)&(valKdxCh == 2) ) or ( (valJdxCh == 2)&(valKdxCh == 1) ) :
                ch12.append(valJdx-valKdx)
            elif ( (valJdxCh == 1)&(valKdxCh == 3) ) or ( (valJdxCh == 3)&(valKdxCh == 1) ) :
                ch13.append(valJdx-valKdx)
            elif ( (valJdxCh == 2)&(valKdxCh == 3) ) or ( (valJdxCh == 3)&(valKdxCh == 2) ) :
                ch23.append(valJdx-valKdx)

            #listOfList[count].append(str(myJdx)+'-'+str(myKdx))
            count+=1
    myChannel.append(ch_temp)
    
dict2save = dict(zip(variables,[list(coincidenceDict.keys()),list(coincidenceDict.values()),ch01,ch02,ch03,ch12,ch13,ch23,timeCh0,timeCh1,timeCh2,timeCh3,ampCh0,ampCh1,ampCh2,ampCh3,cell0Ch0,cell0Ch1,cell0Ch2,cell0Ch3]))
dfFromDict = pd.DataFrame.from_dict(dict2save)
print(dfFromDict)
mylist = dfFromDict.ipicmic.tolist()



# retrieved processed info picmic (with intersections, centroid etc...)
dfxypos = pd.read_csv('data/processedIntersectionsPicmic.csv')
newdf = dfxypos[dfxypos['idxPicmic'].isin(mylist)]
print('----------------------------------------')
print(newdf)


#adding data to coincidence data
dfFromDict['xCentroid'] =newdf.xCentroid.tolist() 
dfFromDict['yCentroid'] =newdf.yCentroid.tolist() 
dfFromDict['intersections'] =newdf.intersections.tolist() 
dfFromDict['tracks'] =newdf.tracks.tolist()
dfFromDict['pt0'] = newdf.pt0.tolist()
dfFromDict['pt1'] = newdf.pt1.tolist()
dfFromDict.to_csv('coincidenceTimeDiffCentroidTracks.csv',index=False)

##print(len(mylist))

exit()    
