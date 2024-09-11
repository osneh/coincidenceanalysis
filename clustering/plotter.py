#!/usr/bin/python
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#from pathlib import Path
import os
import math
import random

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

data = pd.read_csv("xlines.csv",delimiter=";")

mypathI= os.path.getsize('inter.csv')
#mypathC= os.path.getsize('centroid.csv')

if (mypathI!=0):
    df= pd.read_csv("inter.csv", delimiter=";")
    #print(df)
#if (mypathC!=0):
#    df_clus = pd.read_csv("centroid.csv",delimiter=';')

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
    plt.plot(xs,ys,'--', markersize=0, color=tcolor, linewidth=.3)

if (mypathI!=0):
    
    dim = df.index.stop;
    cluster = [ [-99999,-99999] for i in range(dim) ]
    num_clus = -1
    is_clustered = [False for i in range(dim)]
    num_cluster = [-1 for j in range(dim)]
    fillCounter = -1
    count = 0 
    
    #xs = [0.0 for i in range(dim)]
    #ys =[0.0 for i in range(dim)]
    #labs=[str(i) for i in range(dim)]
    #print(xs)
    #print(ys)
    #print(labs)
    
    #for i in range(dim):
    #    plt.text(xs[i],ys[i],labs[i],fontsize=5)
    #plt.show()
    #exit()
    
    for i in range(dim):
        px = df.x[i]
        py  = df.y[i]
        #xs[i]=px
        #ys[i]=py
        #labs[i]=str(i)
        plt.scatter(px,py,2,color='black',marker='x')
        plt.text(px+generate_random_numbers(1), py+generate_random_numbers(1), str(i), fontsize=8, ha='right', va='bottom')
        #plt.text(xs,ys,labs,fontsize=5)
        #print('======================>>>',i,py,py)
    
    for idx in range(dim):
        if (is_clustered[idx]==False) :    
            point_idx = [df.x[idx],df.y[idx]]
            is_clustered[idx]=True
            
            if (fillCounter<0):
                num_clus+=1
                #cluster.append(point_idx)
                cluster[idx] = point_idx
                num_cluster[idx]=num_clus
            #elif (fillCounter>0 and num_clus==-1) :
            #    num_clus+=1
            #    cluster.append(point_idx)
            #    num_cluster[idx]=num_clus
                
            print('#Cluster=',num_clus)
            no_count = 0
            #for jdx in range(idx+1,dim):
            for jdx in range(dim):
                if (jdx == idx) :
                    continue
                
                point_jdx=[df.x[jdx],df.y[jdx]]
                dR = distance(point_idx,point_jdx)
                 
                if (dR<20) :
                    if (is_clustered[jdx]==False  and is_clustered[idx]==True):
                        if (num_cluster[idx]!=-1):
                            #cluster.append(point_jdx)
                            cluster[jdx]=point_jdx
                            is_clustered[jdx]=True
                            num_cluster[jdx]=num_clus
                            #print("===>> IF 00 <<======")
                        else :
                            num_clus+=1
                            num_cluster[idx] = num_clus
                            num_cluster[jdx] = num_cluster[idx]
                            is_clustered[idx] = True
                            is_clustered[jdx] = True
                            #cluster.append(point_idx)
                            #cluster.append(point_jdx)
                            cluster[idx] =point_idx
                            cluster[jdx] = point_jdx
                            #print("===>> IF 11 <<======")
                        
                    elif (is_clustered[jdx]==True ) :
                        num_cluster[idx]=num_cluster[jdx]
                        is_clustered[idx] = True
                        #cluster.append(point_idx)
                        cluster[idx] = point_idx
                        #print("===>> IF 22 <<======")
                        print('~~~ '+str(idx)+'-'+str(jdx)+':'+str(point_idx)+
                        str({is_clustered[idx]})+ '#'+str(num_cluster[idx])+'--'+
                        str(point_jdx)+str({is_clustered[jdx]})+'#'+str(num_cluster[jdx])+
                        ',dR='+format( distance(point_idx,point_jdx),'2.2f'))
                        break
                    
                    #elif(is_clustered[idx]==False and is_clustered[jdx]==False):
                    #    print("I'M HERE, i=",idx,',j=',jdx)
                    #    print("===>> IF 02 <<======")
                    #    num_clus+=1
                    #    num_cluster[idx] = num_clus
                    #    num_cluster[jdx] = num_cluster[idx]
                    #    is_clustered[idx] = True
                    #    is_clustered[jdx] = True
                    #    cluster.append(point_idx)
                    #    cluster.append(point_jdx)
                        

                
                
                    print('~~~ '+str(idx)+'-'+str(jdx)+':'+str(point_idx)+
                      str({is_clustered[idx]})+ '#'+str(num_cluster[idx])+'--'+
                      str(point_jdx)+str({is_clustered[jdx]})+'#'+str(num_cluster[jdx])+
                      ',dR='+format( distance(point_idx,point_jdx),'2.2f'))
                      #',dR='+str(f"{2f}"    distance(point_idx,point_jdx)  ))
                    #break
                else :
                    no_count+=1
                
            if ( no_count==(dim-1)) :
                if ( idx!=0) :
                    num_clus+=1
                #num_clus+=1    
                    
                num_cluster[idx] = num_clus
                is_clustered[idx] = True
                cluster[idx] = point_idx
                
                print('~~~ '+str(idx)+':'+str(point_idx)+
                str({is_clustered[idx]})+ '#'+str(num_cluster[idx]))
            
                
            toprint = [ str(idx)+':'+str(i) for idx,i in enumerate(is_clustered)]
            toprint2 = [ str(jdx)+':'+str(j) for jdx,j in enumerate(num_cluster)]
            print('')
            print('is clustered:')
            print(toprint)
            true_count, false_count = count_true_false(is_clustered)
            print('--------------- Number of True:',true_count)
            print('--------------- Number of False:',false_count)
            
            print('cluster number:')
            print(toprint2)
            print(list(set(num_cluster)))
            print("----------------------------------------------------------------------")
            print("  NO COUNT=",no_count)
            print(30*'@')
            fillCounter +=1
    print("############################################")
    print('counter=',count)
    print('FIll Counter = ', fillCounter)
                

print(num_cluster)
print(cluster)

'''
if (mypathC!=0):
    #colormap = plt.cm.gist_ncar #nipy_spectral, Set1,Paired   
    #colors = [colormap(i) for i in np.linspace(0, 10,500)]
    for kdx in range(df_clus.index.stop):
        ##flagColor = df_clus.centroidFlag[kdx]
        ##if (flagColor==7) :
        cx = df_clus.x[kdx]
        cy = df_clus.y[kdx]
        ll = str(cx)+','+str(cy)
        #plt.scatter(cx,cy,facecolors='none',s= 100,label=ll,color=colors[kdx])
        #plt.scatter(cx,cy,s= 100,color='dimgrey')
'''
plt.ylim(-3000,3000)
plt.xlim(-4000,4000)
#plt.ylim(-35,35)
#plt.xlim(-35,35)
plt.xlabel('X-axis [$\mu$m]')
plt.ylabel('Y-axis [$\mu$m]')
plt.title('PICMIC$0$ Intersections, Centroids and Clusters')
plt.grid()
#if (mypathC!=0):
    #plt.legend( fontsize=9,bbox_to_anchor=(0.5, 0., 0.5, 0.5) )
#plt.axis('off')
plt.savefig('plot.png')
#plt.show()

#exit()


'''
 for i in range(dim):
     px = df.x[i]
     py  = df.y[i]
     #xs[i]=px
     #ys[i]=py
     #labs[i]=str(i)
     plt.scatter(px,py,2,color='black',marker='x')
     plt.text(px+generate_random_numbers(1), py+generate_random_numbers(1), str(i), fontsize=8, ha='right', va='bottom')
'''
plt.figure()


unique_nclus = list(set(num_cluster))
print('HERE:')
print(unique_nclus)
tcolor = ['red','blue','gold','green','magenta','black','orange','tan','peru','cyan','pink','olive','purple','brown','lime','grey']
#colormap = plt.cm.gist_ncar #nipy_spectral, Set1,Paired  
#tcolor = [colormap(i) for i in np.linspace(0, 4,1000)]

mymarker = ['X','o','s','h','H','D','d','1','3','4','8','+', 'v', '^', '<', '>', 'p', '*', 'P','x','|','_','2','.']

color_marker = []
for i in mymarker :
    for j in tcolor :
        color_marker.append([j,i])

#print(color_marker)

#tcolor = 'black';
print(unique_nclus)
for nclus in unique_nclus:
    if ( nclus>-1 ) :
        for idx, point in enumerate(cluster):
            if (num_cluster[idx]==nclus):
            #print(idx,point)
                #if (point[0]>-99999 and point[1]>-99999) :
            #print('index=',idx,', nclus=',num_cluster[idx])
    #if ( unique_nclus[idx] == nclus  ) :
        #print('nclus=',0, ',  px=',p)
#            plt.scatter(point[0],point[1],10,color=tcolor[nclus],marker='x')
                plt.scatter(point[0],point[1],facecolors='none',s=20,color=color_marker[nclus][0],marker=color_marker[nclus][1])

plt.xlabel('X-axis [$\mu$m]')
plt.ylabel('Y-axis [$\mu$m]')
#plt.ylim(-3000,3000)
#plt.xlim(-4000,4000)
#plt.ylim(-35,35)
#plt.xlim(-35,35)
plt.title('PICMIC$0$ Intersections, Centroids and After-Clusterization')
plt.grid()
plt.show()    
#plt.savefig('Cluster-plot.png')



print(60*'@')
print(cluster)
print(50*'-')
print(num_cluster)
print(60*'@')

#exit()

# --------------------------------------------------------------------------------------------------------------------------------------------------------------
num_clus = num_cluster.copy()
num_unique = list(set(num_clus))
merged_cluster = [ False for i in range(len(num_unique))]
merged_cluster[0] =True
print(num_unique)

clusters = cluster.copy()
clone_num_clus = num_clus.copy()
clone_num_unique = num_unique.copy()


for i in range(len(num_unique)-1):
    nclus_i = num_unique[i]
    #merged_j = False
        
    print('############################################')
    for j in range(i+1,len(num_unique)):
        nclus_j = num_unique[j]
        
        print('Nclus_i['+str(i)+']=='+str(nclus_i)+' ~~ Nclus_j['+str(j)+']=='+str(nclus_j))
        
        print('merged CLUSTER:',merged_cluster)
        print('Init.  CLUSTER:',num_clus)
        print('cloned CLUSTER:',clone_num_clus)
        print('numClus UNIQUE:',num_unique) 
        print('cloned UNIQUE :',clone_num_unique)
        #print('-----------------------------------')
        
        ##if (merged_cluster[i]==True and merged_cluster[j]==True ) # original
        if (merged_cluster[i]==True and merged_cluster[j]==True and (clone_num_unique[i]==clone_num_unique[j]) ) :
            continue
        #print('---00---')
        merged_j = False
        for k in range( len(num_clus)) :
            for z in range(len(num_clus)) :
                
                if ( (k != z) and (num_clus[k]==nclus_i) and (num_clus[z]==nclus_j)  ) :
                    #print('---01---','merged_j:',merged_j,',k:',k,',z:',z)
                    if (merged_j):
                        break
                    point_k = clusters[k]
                    point_z = clusters[z]
                    #print('---02---')
                    dist=distance(point_k,point_z)
                    if (dist<80):
                        merged_j=True
                        if (i!=0) :
                            merged_cluster[i] = merged_j        # PAY ATTENTION
                    
                    print('=====>>>>> k:',k,',z:',z,',p['+str(k)+']:',point_k,'-- p['+str(z)+']:',point_z,
                    'dR:',format(dist,'.2f'),',isMerged_j:',merged_j)
                    #if (merged_j):
                    merged_cluster[j] = merged_j
                    print('=====>>>>> merge:',merged_cluster)        
        
                    if (merged_j) :
                        clone_n_clus_i = clone_num_unique[nclus_i]
                        clone_n_clus_j = clone_num_unique[nclus_j]
                        print('-->clone_n_clus_i:',clone_n_clus_i)
                        print('-->clone_n_clus_j:',clone_n_clus_j)
                        
                        
                        temp_n = nclus_j
                        if ( (clone_n_clus_j - clone_n_clus_i)<0 ) :    # pay attention
                            clone_n_clus_i = clone_n_clus_j# = clone_n_clus_i    # pay attention
                            temp_n=nclus_i
                            clone_num_unique[i] = clone_n_clus_i ### EXPERIMENTAL
                        #    print('HERE: ')


                        
                        #print('-->clone_n_clus_i after:',clone_n_clus_i)  #pay attention
                        
                        #if (clone_n_clus_j - clone_n_clus_i>0):
                        else :
                            clone_num_unique[j] = clone_n_clus_i 
                        #clone_num_unique[j]
                        print('clone_num_unique[j]',clone_num_unique[j])
                        
                        for idx in range(len(clone_num_clus)) :
                            ##temp_n = nclus_j
                            ##if ( (clone_n_clus_j - clone_n_clus_i)<0  ):
                            ##    temp_n = nclus_i
                            if (clone_num_clus[idx]==temp_n) :
                            ###if (clone_num_clus[idx]==nclus_j) :
                                #clone_num_clus[idx]=nclus_i
                                clone_num_clus[idx]=clone_n_clus_i
                                
                                
                                
        print('after-CLUSTER: ',clone_num_clus)
        print('after-UNIQUE : ',clone_num_unique)
        print('-----------------------------------')
    print('======================================================================================================')
    
    '''
    plt.figure()
    for idx, point in enumerate(clusters):
        plt.scatter(point[0],point[1],facecolors='none',s=20,color=color_marker[clone_num_clus[idx]][0],marker=color_marker[clone_num_clus[idx]][1])

    plt.xlabel('X-axis [$\mu$m]')
    plt.ylabel('Y-axis [$\mu$m]')
    plt.title('PICMIC$0$ Intersections, Centroids and Re-Clusterization'+str(i))
    plt.grid()
    plt.show()    
    '''

plt.figure()
for idx, point in enumerate(clusters):
    plt.scatter(point[0],point[1],facecolors='none',s=20,color=color_marker[clone_num_clus[idx]][0],marker=color_marker[clone_num_clus[idx]][1])#,label=str(clone_num_clus[idx]))

plt.xlabel('X-axis [$\mu$m]')
plt.ylabel('Y-axis [$\mu$m]')
plt.title('PICMIC$0$ Intersections, Centroids and SRe-Clusterization')
plt.grid()
plt.show()    

# --------------------------------------------------------------------------------------------------------------------------------------------------------------


exit()

