import csv 
import os

#thresholds = [10,30,50,100,250,500,800,1000,1500]
thresholds = [20]
counter = 0 
#with open('tmp10lines.csvBACKUP') as file_obj:     
with open('tmp.csv') as file_obj:     
    reader_obj = csv.reader(file_obj)     
    for row in reader_obj:
        pixels = int(row[0].split(' ')[0])
        for i in thresholds:
            if (pixels>1):
                cdm =str('./xypicmic.exe ')+str(i)+str(' ')+str(row[0])
                os.system(cdm)
                os.system('python plotter.py '+str(counter))
        counter+=1

