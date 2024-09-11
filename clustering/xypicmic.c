#include "xypicmic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void printIntersectionPoint(IntersectionPoint *item, int numIP) {
  //   printf("Printing %d persons:\n", numIP);
    for (int i = 0; i < numIP; ++i) {
printf("[%2d],x,y,bool,flag=", i+1);
printf("%f,%f,\t%d, %d:\n", item[i].x, item[i].y, item[i].intersects, item[i].flag);
    }
}

// surcharge pour val par defaut imprime tout
void printIntersectionPoint0(IntersectionPoint *item) {
  //     int numIP=sizeof(item) / sizeof(*item);
  int numIP=sizeof(item) / sizeof(item[0]); //<= marche pas?
     printf("numIP %d, %d, %d\n", numIP, sizeof(item), sizeof(item[0]));
/*   int numIP=0; */
/* while ((item + numIP)->x[0] != '\0') { */
/*         numIP++; */
/*     } */
    printIntersectionPoint(item, numIP);
}

void replaceBackslashes(char *str) {
    while (*str) {
        if (*str == '\\') *str = '/';
        str++;
    }
}

void extractRYBi(const char *name, char *RYBi) {
    const char *start = strchr(name, '<');
    const char *end = strchr(name, '>');
    if (start != NULL && end != NULL && start < end) {
        int length = end - start - 1;
        if (length < MAX_NAME_LENGTH - 2) {
            RYBi[0] = name[0];
            strncpy(&RYBi[1], start + 1, length);
            RYBi[length + 1] = '\0';
        }
    }
}

double distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

LineCoordinates calculateLineCoordinates(char lineType, int value) {
    LineCoordinates coords;
    
    double Ymax = 852*7.5*0.5;
    double tang60 = sqrt(3);
    double Xmax = (Ymax*2)/sqrt(3);
    double deltax = Xmax*(2./852);
    coords.type = lineType;
    coords.val = value;

    if (lineType == 'Y') {
        coords.y_start = coords.y_end = (value-426) * 7.5;
        coords.x_start = -Xmax; 
        coords.x_end = Xmax;
    } else if (lineType == 'R') {
        coords.y_start = -Ymax; coords.y_end = Ymax;
        coords.x_start =  Ymax/tang60 - deltax*value;
        coords.x_end =   Xmax + Ymax/tang60- deltax*value;
    } else if (lineType == 'B') {
        coords.y_start = Ymax; coords.y_end = -Ymax;
        coords.x_start =   -Xmax-Ymax/tang60 +deltax*(value);
        coords.x_end =    -Ymax/tang60 +deltax*(value);
    }

    return coords;
}

IntersectionPoint calculateIntersection(LineCoordinates line1, LineCoordinates line2) {
    IntersectionPoint result = {INFINITY, INFINITY, false, 1};
    double denominator = (line1.x_start - line1.x_end) * (line2.y_start - line2.y_end) -
                         (line1.y_start - line1.y_end) * (line2.x_start - line2.x_end);

    if (fabs(denominator) < 1e-6) {
        return result; // Les lignes sont parallèles ou coïncidentes.
    }

    double x = ((line1.x_start * line1.y_end - line1.y_start * line1.x_end) * (line2.x_start - line2.x_end) -
                (line1.x_start - line1.x_end) * (line2.x_start * line2.y_end - line2.y_start * line2.x_end)) / denominator;
    double y = ((line1.x_start * line1.y_end - line1.y_start * line1.x_end) * (line2.y_start - line2.y_end) -
                (line1.y_start - line1.y_end) * (line2.x_start * line2.y_end - line2.y_start * line2.x_end)) / denominator;

    result.x = x;
    result.y = y;
    result.intersects = true;
    result.flag = colorFlag(line1.type,line2.type);

    return result;
}

IntersectionPoint calculateCentroid(IntersectionPoint *cluster, int size) {
    IntersectionPoint centroid = {0, 0, false, 0,-1};
    unsigned char results=0;

    for (int i = 0; i < size; i++) {
        centroid.x += cluster[i].x;
        centroid.y += cluster[i].y;
        results = fill_bits(results,cluster[i].flag);
        centroid.num =cluster[i].num;
    }
    centroid.flag = results;


    if ( centroid.flag== 7 ){
        centroid.intersects = true;
    }

    if (size!=0){    
        centroid.x /= size;
        centroid.y /= size;
    }
    else {
        centroid.x = INFINITY;
        centroid.y = INFINITY;
    }

    return centroid;
}

void splitLineColor(LineCoordinates *Items, int nItems, LineCoordinates *ly, LineCoordinates *lr, LineCoordinates *lb) {
    int unsigned temp_y = 0; unsigned int temp_r=0; unsigned int temp_b=0;
    for ( int j = 0 ; j < nItems; ++j){
        char ltype =  Items[j].type ;
    
        if ( ltype == 'Y') ly[temp_y++] = Items[j];
        else if ( ltype == 'R' ) lr[temp_r++] = Items[j];
        else if ( ltype == 'B' ) lb[temp_b++] = Items[j];
    }
}

void xLines(IntersectionPoint *intersecs, int nIntersecs,LineCoordinates *yellow, int y_size, LineCoordinates *red, int r_size, LineCoordinates * blue, int b_size, int * counter){
        int iCount =0;
        //printf("----------------------------------\n");
        if (y_size>0){
            for (int idx = 0 ; idx<y_size; idx++){              // Yellow lines Loop
                for (int jdx = 0 ; jdx<r_size; jdx++){          // Red lines Loop 
                    IntersectionPoint intersection = calculateIntersection(yellow[idx],red[jdx]);
                    intersecs[iCount++] = intersection;
                    //printf("%c%d-%c%d;  xy(%0.2f,%0.2f); flag=%d\n",yellow[idx].type,yellow[idx].val,red[jdx].type,red[jdx].val,intersection.x,intersection.y,intersection.flag );
                }

                for (int kdx = 0 ; kdx<b_size; kdx++){          // Blue lines Loop 
                    IntersectionPoint intersection = calculateIntersection(yellow[idx],blue[kdx]);
                    intersecs[iCount++] = intersection;
                }
            }
        }

        if (r_size>0 && b_size > 0 ) {              // Red and Blue lines Loop
            for (int idx = 0; idx<r_size; idx++){
                for (int jdx = 0 ; jdx<b_size; jdx++){
                    IntersectionPoint intersection = calculateIntersection(red[idx],blue[jdx]);
                    intersecs[iCount++] = intersection; 
                }
            }
        }
    *counter=iCount;
}

void fillCentroids(int cut, IntersectionPoint *myIntersections, int myDimIntersections,IntersectionPoint * arrayCentroid, int nCentroid ){

    int max_interactions = myDimIntersections;
    int myclustered[max_interactions];
    memset(myclustered,0,max_interactions*sizeof(int));
    int fillCounter = -1;

    for (int i = 0; i < myDimIntersections-1; i++) { 
        int numeroCluster = fillCounter;
        //double min_distance = INFINITY;
        //fillCounter++;
        if (!myclustered[i]) {
            IntersectionPoint cluster[myDimIntersections];      // Tableau temporaire pour stocker un cluster de points.
            numeroCluster++;
            int clusterSize = 0;                                // Taille du cluster.
            cluster[clusterSize] = myIntersections[i]; 
            cluster[clusterSize++].num = numeroCluster;
            myclustered[i] = 1;                                 // Marquage du point comme regroupé.
            // Recherche d'autres points à inclure dans le cluster.
            for (int j = i + 1; j < myDimIntersections; j++) {
                if (!myclustered[j]) {
                    double dist = distance(myIntersections[i].x, myIntersections[i].y, myIntersections[j].x, myIntersections[j].y);
                    //double dist = distance(myIntersections[i].x, myIntersections[i].y, cluster[j].x, cluster[j].y);
                    //double dist_min = INFINITY;
                    if (dist <  cut) {
                    //if (dist <  min_distance) {
                        cluster[clusterSize] = myIntersections[j]; // Ajout du point au cluster.
                        cluster[clusterSize++].num = numeroCluster;
                        myclustered[j] = 1; 
                        //min_distance = dist;                          // Marquage du point comme regroupé.
                    }
                }
            }
            IntersectionPoint centroid = calculateCentroid(cluster, clusterSize);
            fillCounter++;
            //printf("INSIDE Centroid --> x=%0.2f ,\t y=%0.2f \t , flag=%d, is3Colors=%d, fillCounterValue=%d ,  numClusters=%d\n",centroid.x,centroid.y,centroid.flag,centroid.intersects,fillCounter,centroid.num);
            //if (fillCounter>-1){
            //    for (int t = 0 ; t<clusterSize; t++ ){
                   // printf("%d;%0.2f;%0.2f \n",fillCounter,cluster[t].x,cluster[t].y);
                    //fprintf(csvFile3,"%d;%0.2f;%0.2f\n",fillCounter,cluster[t].x,cluster[t].y);
            //    }
           // }


            if (fillCounter>-1){
	    	arrayCentroid[fillCounter] = centroid;
	        }
            
        }
    }

}

void clustering(int cut, IntersectionPoint *myIntersections, int myDimIntersections, int *nclusters){

    int max_interactions = myDimIntersections;
    int is_clustered[max_interactions];
    int num_cluster[max_interactions];

    memset(is_clustered,0,max_interactions*sizeof(int));
    memset(num_cluster,-1,max_interactions*sizeof(int));
    int fillCounter = -1;
    int numeroCluster = -1;

    IntersectionPoint clusters[myDimIntersections];

    for (int idx = 0; idx < myDimIntersections; idx++) { 
    
        if (!is_clustered[idx]) {
            is_clustered[idx]=1;

            if (fillCounter<0){
                numeroCluster++;
                clusters[idx] = myIntersections[idx];
                num_cluster[idx]=numeroCluster;
                clusters[idx].num = num_cluster[idx];
            }
            //printf("#Cluster=%d\n",numeroCluster);
            int no_count = 0 ; // to define mono-point clusters
            for (int jdx =  0 ; jdx < myDimIntersections; jdx++) {
                if (jdx == idx) 
                    continue;

                double dist = distance(myIntersections[idx].x, myIntersections[idx].y, myIntersections[jdx].x, myIntersections[jdx].y);
                if (dist<cut){
                    if (is_clustered[jdx]==0 && is_clustered[idx]==1){
                        if (num_cluster[idx] != -1){
                            clusters[jdx] = myIntersections[jdx];
                            clusters[jdx].num = numeroCluster;
                            is_clustered[jdx] = 1;
                            num_cluster[jdx] = numeroCluster;
                        }
                        else{
                            numeroCluster++;
                            num_cluster[idx] = numeroCluster;
                            num_cluster[jdx] = num_cluster[idx];
                            is_clustered[idx] = 1;
                            is_clustered[jdx] = 1;
                            clusters[idx] = myIntersections[idx];
                            clusters[jdx] = myIntersections[jdx];
                            clusters[idx].num = num_cluster[idx];
                            clusters[jdx].num = num_cluster[jdx];
                        }
                    }
                    else if ( is_clustered[jdx]==1){ 
                            num_cluster[idx] = num_cluster[jdx];
                            is_clustered[idx] = 1;
                            clusters[idx] =  myIntersections[idx];
                            clusters[idx].num = num_cluster[idx];

                            /*printf("~~ %d - %d :  xi: %2.2f, yi: %2.2f {%d} #%d --xj: %2.2f, yj: %2.2f {%d} #%d -- dR:%2.2f\n"
                            ,idx,jdx,myIntersections[idx].x,myIntersections[idx].y, is_clustered[idx],num_cluster[idx],
                            myIntersections[jdx].x,myIntersections[jdx].y , is_clustered[jdx],num_cluster[jdx],dist);*/

                            break;

                    }

                     /*printf("~~ %d - %d :  xi: %2.2f, yi: %2.2f {%d} #%d --xj: %2.2f, yj: %2.2f {%d} #%d -- dR:%2.2f\n"
                    ,idx,jdx,myIntersections[idx].x,myIntersections[idx].y, is_clustered[idx],num_cluster[idx],
                    myIntersections[jdx].x,myIntersections[jdx].y , is_clustered[jdx],num_cluster[jdx],dist);*/
                }
                else {
                    no_count++;
                }

                if (no_count==(max_interactions-1)){
                    if (idx!=0){
                        numeroCluster++;
                    }
                    num_cluster[idx] = numeroCluster;
                    is_clustered[idx] = 1;
                    clusters[idx] = myIntersections[idx];
                    clusters[idx].num = num_cluster[idx];

                    /* printf("~~ %d - %d :  xi: %2.2f, yi: %2.2f {%d} #%d \n"
                    ,idx,jdx,myIntersections[idx].x,myIntersections[idx].y, is_clustered[idx],num_cluster[idx]);*/
                }
            }
            fillCounter++;
        }
    }
    
    printf("----------- inside clustering &1st layer ----------------------------------\n");
    for (int k = 0 ; k<max_interactions;k++){
        //printf("z:%d \t , clusNumber:%d, \t x: %2.2f \t , y: %2.2f \n",k,myIntersections[k].num,myIntersections[k].x,myIntersections[k].y);
         printf("intersection index:%d \t , clusNumber:%d, \t x: %2.2f \t , y: %2.2f \n",k,clusters[k].num,clusters[k].x,clusters[k].y);
    }
    //myIntersections[z] = clusters[k];
    //printf("\n");

   // Determine if more than 1 cluster was found
   int dim_unique;
   int *unique = get_unique_values(num_cluster, max_interactions, &dim_unique);
   *nclusters = dim_unique;

   printf("====Layer 1 , num Clusters = %d \n", *nclusters);
   for (int i = 0; i < *nclusters; i++)
    printf("idx=%d , num_clus=%d\n",i,unique[i]);

    // Only if more than 1 cluster
    if (dim_unique > 1) {
      clustersMerger(cut,4,clusters, myDimIntersections, unique, dim_unique,nclusters);  
    }

     printf("----------- inside clustering &2st layer ----------------------------------\n");
    // clone array of intersections 
    for (int z = 0 ; z<max_interactions;z++){ 
        printf("i:%d \t , clusNumber:%d, \t x: %2.2f \t , y: %2.2f \n",z,clusters[z].num,clusters[z].x,clusters[z].y);
        myIntersections[z] = clusters[z];
    }

    printf("====Layer 2 , num Clusters = %d \n", *nclusters);

    //free(is_clustered);
    //free(num_cluster);
    //free(clusters);

}

void clustersMerger(int cut, int factor,  IntersectionPoint *m_clusters, int myDimIntersections, int *num_unique, int dim_unique, int *nclus){

    int merged_cluster[myDimIntersections];
    int clone_num_cluster[myDimIntersections];
    int clone_num_unique[dim_unique];
    int num_clus[myDimIntersections];

    memset(merged_cluster,0,myDimIntersections*sizeof(int));
    memset(clone_num_cluster,-1,myDimIntersections*sizeof(int));
    memset(clone_num_unique,-1,dim_unique*sizeof(int));
    memset(num_clus,-1,myDimIntersections*sizeof(int));

    // array initilizations
    for (int indx = 0 ; indx<myDimIntersections; indx++){
        clone_num_cluster[indx] = m_clusters[indx].num;
        num_clus[indx] = m_clusters[indx].num;
    }

    for (int jndx = 0; jndx<dim_unique; jndx++)    
        clone_num_unique[jndx] = num_unique[jndx];


    // merging clusters
    merged_cluster[0] = 1;
    for (int i = 0; i< dim_unique -1; i++){
        int nclus_i = num_unique[i];
      
        for (int j = i+1; j < dim_unique; j++){
            int nclus_j = num_unique[j];

            if ( (merged_cluster[i]==1) && (merged_cluster[j]==1) && (clone_num_unique[i]==clone_num_unique[j]) ) 
                continue;

            bool merged_j = false;
            for ( int k=0 ; k<myDimIntersections; k++){
                for ( int z = 0; z<myDimIntersections; z++){

                    if ( (k != z) && ( num_clus[k]==nclus_i) && (num_clus[z]==nclus_j) ) {
                        if (merged_j) 
                            break;

                        double dist = distance(m_clusters[k].x, m_clusters[k].y, m_clusters[z].x, m_clusters[z].y);
                        if ( dist<cut*factor) {
                            merged_j = true;
                            if (i!=0){
                                merged_cluster[i] = merged_j;
                            }
                        }
                        merged_cluster[j] = merged_j;

                        if (merged_j){
                            int clone_n_clus_i = clone_num_unique[nclus_i];
                            int clone_n_clus_j = clone_num_unique[nclus_j];

                            int temp_n = nclus_j;

                            if ( (clone_n_clus_j - clone_n_clus_i)<0 ){
                                clone_n_clus_i = clone_n_clus_j;
                                temp_n=nclus_i;
                                clone_num_unique[i] = clone_n_clus_i;
                            }
                            else{
                                clone_num_unique[j] = clone_n_clus_i;
                            }

                            for (int ndx = 0 ; ndx< myDimIntersections; ndx++){
                                if (clone_num_cluster[ndx]==temp_n){
                                    clone_num_cluster[ndx] = clone_n_clus_i;
                                }
                            }

                        }

                    }

                }
            }
          
        }
    }

    // modify the array with the new cluster numbers after merge
    for (int kdx = 0 ; kdx < myDimIntersections; kdx++){
        m_clusters[kdx].num = clone_num_cluster[kdx];
        //printf("kdx:%d \t , clusNumber:%d, \t x: %2.2f \t , y: %2.2f \n",kdx,m_clusters[kdx].num,m_clusters[kdx].x,m_clusters[kdx].y);
    }

    int new_dim_unique;
    int *new_unique = get_unique_values(clone_num_cluster, myDimIntersections, &new_dim_unique);
    *nclus = new_dim_unique;
    //printf("====INSIDE2 , num Clus = %d \n", nclus);
}


void getCentroids( IntersectionPoint * m_centroids, int size_centroids, IntersectionPoint *m_clusters, int size_inter ){

    int array_num_cluster[size_inter];
    memset(array_num_cluster,-1,size_inter*sizeof(int));

    for (int j = 0; j < size_inter; j++)
            array_num_cluster[j] = m_clusters[j].num;

    int dim_unique_clus;
    int *unique_clus = get_unique_values(array_num_cluster, size_inter, &dim_unique_clus);
          
        for (int idx = 0; idx < size_centroids; idx++){
            IntersectionPoint centro = {0,0,false,0,-1};
            m_centroids[idx] = centro;
            unsigned char results=0;
    
            int count = 0;
            for (int jdx = 0 ; jdx < size_inter; jdx++){

                if ( unique_clus[idx] == m_clusters[jdx].num ){
                    m_centroids[idx].x += m_clusters[jdx].x;
                    m_centroids[idx].y += m_clusters[jdx].y;
                    results = fill_bits(results,m_clusters[jdx].flag);
                    m_centroids[idx].num = m_clusters[jdx].num;
                    count++;
                }
            }
            m_centroids[idx].flag = results;
            if ( m_centroids[idx].flag==7){
                m_centroids[idx].intersects = true;
            }
            if (count!=0){
                m_centroids[idx].x /=count;
                m_centroids[idx].y /=count;
            }
            else {
                m_centroids[idx].x = INFINITY;
                m_centroids[idx].y = INFINITY;
            }
        }
    

}


// Function to check if a value is already present in the array
int is_value_present(int *arr, int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return 1; // Value found
        }
    }
    return 0; // Value not found
}

// Function to create an array with unique values from another array
int* get_unique_values(int *arr, int size, int *new_size) {
    int *unique_arr = malloc(size * sizeof(int)); // Allocate memory for the worst-case scenario
    if (unique_arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    int unique_count = 0; // Counter for unique values

    for (int i = 0; i < size; i++) {
        // Check if the value is already present in the unique array
        if (!is_value_present(unique_arr, unique_count, arr[i])) {
            unique_arr[unique_count] = arr[i];
            unique_count++;
        }
    }

    // Resize the array to the exact number of unique values
    unique_arr = realloc(unique_arr, unique_count * sizeof(int));
    if (unique_arr == NULL) {
        fprintf(stderr, "Memory reallocation failed\n");
        exit(EXIT_FAILURE);
    }

    *new_size = unique_count; // Update the new size
    return unique_arr;
}

void fillLines(char *arguments[], LineCoordinates *allLines, int nLines, int *yellowSize , int *redSize, int * blueSize){
     int temp_y= 0; int temp_r=0; int temp_b=0; 

     for (int i = 0; i < nLines; i++) {
        int inputRow = atoi(arguments[3 + i * 2]); 
        int inputCol = atoi(arguments[4 + i * 2]); 
        if (inputRow >= 0 && inputRow < ROWS && inputCol >= 0 && inputCol < COLS) {
            char *value = arr[inputRow][inputCol];
            char lineType = value[0]; 

            if (lineType == 'D') {
                printf("For Row %d, Column %d: This is a dummy cell.\n", inputRow, inputCol);
            } 
            else {
                int lineValue = atoi(&value[1]); // Conversion de la valeur de la ligne en entier.
                //printf("----------------> correct value =%d\n",lineValue);
                // filling integer for a posterior allocation in array of struct
                if (lineType == 'Y') temp_y+=1;
                else if (lineType == 'R') temp_r+=1;
                else if (lineType == 'B') temp_b+=1;
            
		printf("Line Type %c\n",lineType);
 
                LineCoordinates coords = calculateLineCoordinates(lineType, lineValue); // Calcul des coordonnées de la ligne.
                allLines[i] = coords;
            }
        } 
        else {
            printf("Invalid row or column for element %d. Please enter values within the range.\n", i + 1);
        }
    }
    *yellowSize = temp_y;
    *redSize = temp_r;
    *blueSize = temp_b;
}

int assign_number(char c) {
    switch(c) {
        case 'Y':
            return 0;
        case 'R':
            return 1;
        case 'B':
            return 2;
        default:
            return -1; // Indicates invalid character
    }
}

int colorFlag(char color1, char color2){
    int num1 = assign_number(color1);
    int num2 = assign_number(color2);


    if ((num1 == 0 && num2 == 1) || (num1 == 1 && num2 == 0))
        return COMBINATION_YR;
    else if ((num1 == 0 && num2 == 2) || (num1 == 2 && num2 == 0))
        return COMBINATION_YB;
    else if ((num1 == 1 && num2 == 2) || (num1 == 2 && num2 == 1))
        return COMBINATION_RB;
    else
        return -1; // Indicates invalid combination

}

void init_array(IntersectionPoint *array, int dim){
    for (int k = 0 ; k < dim; k++){
        array[k].x = 0;
        array[k].y = 0;
        array[k].intersects= false;
        array[k].flag= 0; 
        array[k].num = -1 ;
    }
}

unsigned char fill_bits(unsigned char byte, int num) {
    // Set bit 0, 1, and 2 based on the provided number
    if (num == COMBINATION_YR) {
        byte |= (1 << 0);
    }
    if (num == COMBINATION_YB) {
        byte |= (1 << 1);
    }
    if (num == COMBINATION_RB) {
        byte |= (1 << 2);
    }
    return byte;
}


int selThreshold(int nlines){
  int selthre = 1500;
  if (nlines < 15) {
	  selthre = 100;
  } else if (nlines>=15 && nlines<20){
  	selthre = 250;
  } else if (nlines>=20 && nlines<30){
  	selthre = 500;
  } else if(nlines>=30){
  	selthre = 1500;
  }
  return selthre;
}

// ---------------------------------
double distance_p(IntersectionPoint p1, IntersectionPoint p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Function to initialize a cluster
/*
Cluster *init_cluster(int num_points) {
    Cluster *cluster = malloc(sizeof(Cluster));
    if (cluster == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    cluster->array_points = malloc(num_points * sizeof(IntersectionPoint));
    if (cluster->array_points == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    cluster->num_points = num_points;
    return cluster;
}
*/
/*
void free_cluster(Cluster *cluster) {
    free(cluster->array_points);
    free(cluster);
}
*/
/*
// Function to find the nearest cluster for a point
int find_nearest_cluster(IntersectionPoint point, Cluster *clusters, int num_clusters) {
    int nearest_cluster = -1;
    double min_distance = INFINITY;

    for (int i = 0; i < num_clusters; i++) {
        double d = distance_p(point, clusters[i].centroid);
        if (d < min_distance) {
            min_distance = d;
            nearest_cluster = i;
        }
    }

    return nearest_cluster;
}
*/
// Function to perform hierarchical clustering
/*
void hierarchical_clustering(Point points[], int num_points, int max_clusters) {
    struct Cluster clusters[MAX_CLUSTERS];
    int num_clusters = 0;

    for (int i = 0; i < num_points; i++) {
        int nearest_cluster = find_nearest_cluster(points[i], clusters, num_clusters);
        if (nearest_cluster == -1 || clusters[nearest_cluster].num_points >= max_clusters) {
            // Create a new cluster
            clusters[num_clusters].centroid = points[i];
            clusters[num_clusters].num_points = 1;
            clusters[num_clusters].points[0] = points[i];
            num_clusters++;
        } else {
            // Add the point to the nearest cluster
            clusters[nearest_cluster].points[clusters[nearest_cluster].num_points] = points[i];
            clusters[nearest_cluster].num_points++;
            // Update the centroid of the cluster
            clusters[nearest_cluster].centroid.x = (clusters[nearest_cluster].centroid.x * (clusters[nearest_cluster].num_points - 1) + points[i].x) / clusters[nearest_cluster].num_points;
            clusters[nearest_cluster].centroid.y = (clusters[nearest_cluster].centroid.y * (clusters[nearest_cluster].num_points - 1) + points[i].y) / clusters[nearest_cluster].num_points;
        }
    }

    // Output the clusters
    for (int i = 0; i < num_clusters; i++) {
        printf("Cluster %d centroid: (%.2f, %.2f), Number of points: %d\n", i + 1, clusters[i].centroid.x, clusters[i].centroid.y, clusters[i].num_points);
    }
}
*/
// --------------------------------
char arr[ROWS][COLS][MAX_NAME_LENGTH]= {
{{"D0"},{"D128"},{"Y1"},{"D384"},{"D512"},{"Y2"},{"D768"},{"D896"},{"Y3"},{"D1152"},
{"D1280"},{"Y4"},{"D1536"},{"D1664"},{"Y5"},{"D1920"},{"D2048"},{"Y6"},{"D2304"},{"D2432"},
{"D2560"},{"D2688"},{"D2816"},{"D2944"},{"D3072"},{"D3200"},{"D3328"},{"D3456"},{"D3584"},{"D3712"},
{"D3840"},{"D3968"},{"D4096"},{"D4224"},{"D4352"},{"D4480"},{"D4608"},{"D4736"},{"D4864"},{"D4992"},
{"D5120"},{"D5248"},{"D5376"},{"D5504"},{"D5632"},{"D5760"},{"D5888"},{"D6016"},{"D6144"},{"D6272"},
{"D6400"},{"D6528"},{"D6656"},{"D6784"}},
{{"D1"},{"D129"},{"D257"},{"D385"},{"D513"},{"D641"},
{"D769"},{"D897"},{"D1025"},{"D1153"},{"D1281"},{"D1409"},{"D1537"},{"D1665"},{"R422"},{"B30"},
{"D2049"},{"R374"},{"B78"},{"Y7"},{"R328"},{"B124"},{"Y8"},{"D2945"},{"B170"},{"Y9"},
{"D3329"},{"B224"},{"Y10"},{"D3713"},{"D3841"},{"Y11"},{"D4097"},{"B324"},{"Y12"},{"D4481"},
{"D4609"},{"Y13"},{"D4865"},{"D4993"},{"D5121"},{"D5249"},{"D5377"},{"D5505"},{"D5633"},{"D5761"},
{"D5889"},{"D6017"},{"D6145"},{"D6273"},{"D6401"},{"D6529"},{"D6657"},{"D6785"}},
{{"D2"},{"D130"},
{"D258"},{"Y19"},{"D514"},{"D642"},{"Y20"},{"D898"},{"D1026"},{"D1154"},{"D1282"},{"D1410"},
{"D1538"},{"D1666"},{"D1794"},{"D1922"},{"D2050"},{"B61"},{"D2306"},{"D2434"},{"B109"},{"D2690"},
{"D2818"},{"D2946"},{"D3074"},{"D3202"},{"D3330"},{"D3458"},{"R204"},{"B254"},{"D3842"},{"R152"},
{"B304"},{"D4226"},{"R99"},{"B351"},{"D4610"},{"D4738"},{"D4866"},{"Y14"},{"R5"},{"D5250"},
{"Y15"},{"D5506"},{"D5634"},{"Y16"},{"D5890"},{"D6018"},{"Y17"},{"D6274"},{"D6402"},{"Y18"},
{"D6658"},{"D6786"}},
{{"D3"},{"D131"},{"D259"},{"D387"},{"D515"},{"D643"},{"D771"},{"D899"},
{"Y21"},{"D1155"},{"D1283"},{"Y22"},{"D1539"},{"D1667"},{"Y23"},{"R412"},{"B54"},{"Y24"},
{"D2307"},{"B96"},{"Y25"},{"D2691"},{"D2819"},{"Y26"},{"R267"},{"B199"},{"D3331"},{"R216"},
{"B248"},{"D3715"},{"D3843"},{"D3971"},{"D4099"},{"R125"},{"B341"},{"D4483"},{"R72"},{"D4739"},
{"D4867"},{"R29"},{"D5123"},{"D5251"},{"D5379"},{"D5507"},{"D5635"},{"D5763"},{"D5891"},{"D6019"},
{"D6147"},{"D6275"},{"D6403"},{"D6531"},{"D6659"},{"D6787"}},
{{"D4"},{"D132"},{"D260"},{"D388"},
{"D516"},{"D644"},{"D772"},{"D900"},{"D1028"},{"D1156"},{"D1284"},{"D1412"},{"D1540"},{"D1668"},
{"D1796"},{"D1924"},{"D2052"},{"D2180"},{"B91"},{"D2436"},{"R332"},{"B137"},{"D2820"},{"R284"},
{"B188"},{"Y27"},{"D3332"},{"B231"},{"Y28"},{"R187"},{"D3844"},{"Y29"},{"R139"},{"D4228"},
{"Y30"},{"D4484"},{"B374"},{"Y31"},{"R46"},{"B424"},{"Y32"},{"D5252"},{"D5380"},{"Y33"},
{"D5636"},{"D5764"},{"D5892"},{"D6020"},{"D6148"},{"D6276"},{"D6404"},{"D6532"},{"D6660"},{"D6788"}
},
{{"D5"},{"D133"},{"D261"},{"Y37"},{"D517"},{"D645"},{"Y38"},{"D901"},{"D1029"},{"Y39"},
{"D1285"},{"D1413"},{"Y40"},{"D1669"},{"B24"},{"D1925"},{"D2053"},{"B74"},{"D2309"},{"D2437"},
{"B125"},{"D2693"},{"D2821"},{"D2949"},{"D3077"},{"D3205"},{"B216"},{"D3461"},{"R212"},{"D3717"},
{"D3845"},{"R161"},{"D4101"},{"D4229"},{"D4357"},{"D4485"},{"D4613"},{"R60"},{"B417"},{"D4997"},
{"R17"},{"B464"},{"D5381"},{"D5509"},{"D5637"},{"Y34"},{"D5893"},{"D6021"},{"Y35"},{"D6277"},
{"D6405"},{"Y36"},{"D6661"},{"D6789"}},
{{"D6"},{"D134"},{"D262"},{"D390"},{"D518"},{"D646"},
{"D774"},{"D902"},{"D1030"},{"D1158"},{"D1286"},{"D1414"},{"D1542"},{"B16"},{"Y41"},{"R426"},
{"D2054"},{"Y42"},{"D2310"},{"D2438"},{"Y43"},{"D2694"},{"D2822"},{"Y44"},{"R275"},{"D3206"},
{"Y45"},{"D3462"},{"D3590"},{"Y46"},{"R184"},{"B306"},{"D4102"},{"D4230"},{"D4358"},{"D4486"},
{"R80"},{"D4742"},{"D4870"},{"R32"},{"B451"},{"D5254"},{"D5382"},{"D5510"},{"D5638"},{"D5766"},
{"D5894"},{"D6022"},{"D6150"},{"D6278"},{"D6406"},{"D6534"},{"D6662"},{"D6790"}},
{{"D7"},{"D135"},
{"D263"},{"D391"},{"D519"},{"D647"},{"D775"},{"D903"},{"D1031"},{"D1159"},{"D1287"},{"D1415"},
{"D1543"},{"D1671"},{"D1799"},{"B51"},{"D2055"},{"D2183"},{"D2311"},{"D2439"},{"D2567"},{"D2695"},
{"D2823"},{"D2951"},{"D3079"},{"D3207"},{"R251"},{"D3463"},{"D3591"},{"D3719"},{"B295"},{"Y47"},
{"R148"},{"B336"},{"Y48"},{"R101"},{"D4615"},{"Y49"},{"R54"},{"D4999"},{"Y50"},{"R3"},
{"D5383"},{"Y51"},{"D5639"},{"D5767"},{"Y52"},{"D6023"},{"D6151"},{"Y53"},{"D6407"},{"D6535"},
{"D6663"},{"D6791"}},
{{"D8"},{"D136"},{"D264"},{"Y55"},{"D520"},{"D648"},{"Y56"},{"D904"},
{"D1032"},{"Y57"},{"D1288"},{"D1416"},{"Y58"},{"R460"},{"B34"},{"Y59"},{"D2056"},{"B80"},
{"Y60"},{"R364"},{"B131"},{"D2696"},{"R316"},{"D2952"},{"D3080"},{"D3208"},{"D3336"},{"D3464"},
{"R224"},{"D3720"},{"D3848"},{"D3976"},{"D4104"},{"D4232"},{"D4360"},{"D4488"},{"D4616"},{"R78"},
{"D4872"},{"D5000"},{"R24"},{"B471"},{"D5384"},{"D5512"},{"D5640"},{"D5768"},{"D5896"},{"D6024"},
{"D6152"},{"D6280"},{"D6408"},{"Y54"},{"D6664"},{"D6792"}},
{{"D9"},{"D137"},{"D265"},{"D393"},
{"D521"},{"D649"},{"D777"},{"D905"},{"D1033"},{"D1161"},{"D1289"},{"D1417"},{"R482"},{"B19"},
{"D1801"},{"R431"},{"B67"},{"D2185"},{"R384"},{"D2441"},{"Y61"},{"D2697"},{"D2825"},{"Y62"},
{"R288"},{"B214"},{"Y63"},{"D3465"},{"B262"},{"Y64"},{"D3849"},{"D3977"},{"Y65"},{"D4233"},
{"B360"},{"Y66"},{"R91"},{"D4745"},{"D4873"},{"D5001"},{"D5129"},{"D5257"},{"D5385"},{"D5513"},
{"D5641"},{"D5769"},{"D5897"},{"D6025"},{"D6153"},{"D6281"},{"D6409"},{"D6537"},{"D6665"},{"D6793"}
},
{{"D10"},{"Y72"},{"D266"},{"D394"},{"Y73"},{"D650"},{"D778"},{"D906"},{"D1034"},{"D1162"},
{"D1290"},{"D1418"},{"B6"},{"D1674"},{"R449"},{"B60"},{"D2058"},{"D2186"},{"B108"},{"D2442"},
{"R360"},{"B157"},{"D2826"},{"D2954"},{"B206"},{"D3210"},{"D3338"},{"B250"},{"D3594"},{"D3722"},
{"D3850"},{"D3978"},{"R159"},{"B350"},{"D4362"},{"R110"},{"B395"},{"Y67"},{"D4874"},{"D5002"},
{"Y68"},{"R12"},{"B497"},{"Y69"},{"D5642"},{"D5770"},{"Y70"},{"D6026"},{"D6154"},{"Y71"},
{"D6410"},{"D6538"},{"D6666"},{"D6794"}},
{{"D11"},{"D139"},{"D267"},{"D395"},{"D523"},{"D651"},
{"Y74"},{"D907"},{"D1035"},{"Y75"},{"D1291"},{"D1419"},{"Y76"},{"D1675"},{"B45"},{"Y77"},
{"R420"},{"B94"},{"Y78"},{"D2443"},{"B142"},{"Y79"},{"D2827"},{"D2955"},{"Y80"},{"D3211"},
{"D3339"},{"D3467"},{"D3595"},{"B286"},{"D3851"},{"R179"},{"B339"},{"D4235"},{"D4363"},{"B386"},
{"D4619"},{"R84"},{"B433"},{"D5003"},{"R37"},{"D5259"},{"D5387"},{"D5515"},{"D5643"},{"D5771"},
{"D5899"},{"D6027"},{"D6155"},{"D6283"},{"D6411"},{"D6539"},{"D6667"},{"D6795"}},
{{"D12"},{"D140"},
{"D268"},{"D396"},{"D524"},{"D652"},{"D780"},{"D908"},{"D1036"},{"D1164"},{"D1292"},{"D1420"},
{"R490"},{"D1676"},{"D1804"},{"D1932"},{"B85"},{"D2188"},{"R395"},{"B132"},{"D2572"},{"D2700"},
{"B176"},{"D2956"},{"R294"},{"B226"},{"Y81"},{"R252"},{"D3596"},{"Y82"},{"R197"},{"B322"},
{"Y83"},{"R156"},{"D4364"},{"Y84"},{"D4620"},{"B425"},{"Y85"},{"R53"},{"D5132"},{"Y86"},
{"R7"},{"D5516"},{"D5644"},{"D5772"},{"D5900"},{"D6028"},{"D6156"},{"D6284"},{"D6412"},{"D6540"},
{"D6668"},{"D6796"}},
{{"D13"},{"Y90"},{"D269"},{"D397"},{"Y91"},{"D653"},{"D781"},{"Y92"},
{"D1037"},{"D1165"},{"Y93"},{"D1421"},{"B18"},{"D1677"},{"D1805"},{"B70"},{"D2061"},{"R411"},
{"B118"},{"D2445"},{"D2573"},{"B169"},{"D2829"},{"R314"},{"D3085"},{"D3213"},{"D3341"},{"B264"},
{"D3597"},{"D3725"},{"D3853"},{"D3981"},{"R173"},{"D4237"},{"D4365"},{"R123"},{"D4621"},{"D4749"},
{"D4877"},{"B461"},{"D5133"},{"R28"},{"D5389"},{"Y87"},{"D5645"},{"D5773"},{"Y88"},{"D6029"},
{"D6157"},{"Y89"},{"D6413"},{"D6541"},{"D6669"},{"D6797"}},
{{"D14"},{"D142"},{"D270"},{"D398"},
{"D526"},{"D654"},{"D782"},{"D910"},{"D1038"},{"D1166"},{"D1294"},{"B4"},{"Y94"},{"R480"},
{"B52"},{"Y95"},{"D2062"},{"D2190"},{"Y96"},{"D2446"},{"B150"},{"Y97"},{"D2830"},{"D2958"},
{"Y98"},{"R289"},{"B247"},{"Y99"},{"R242"},{"D3726"},{"Y100"},{"R189"},{"B348"},{"D4238"},
{"D4366"},{"D4494"},{"D4622"},{"R96"},{"B444"},{"D5006"},{"R44"},{"D5262"},{"D5390"},{"D5518"},
{"D5646"},{"D5774"},{"D5902"},{"D6030"},{"D6158"},{"D6286"},{"D6414"},{"D6542"},{"D6670"},{"D6798"}
},
{{"D15"},{"D143"},{"D271"},{"D399"},{"D527"},{"D655"},{"D783"},{"D911"},{"D1039"},{"D1167"},
{"D1295"},{"D1423"},{"R500"},{"B46"},{"D1807"},{"R451"},{"B88"},{"D2191"},{"D2319"},{"D2447"},
{"D2575"},{"D2703"},{"D2831"},{"D2959"},{"D3087"},{"D3215"},{"D3343"},{"R262"},{"B288"},{"D3727"},
{"R215"},{"B333"},{"Y101"},{"D4239"},{"D4367"},{"Y102"},{"D4623"},{"D4751"},{"Y103"},{"R68"},
{"D5135"},{"Y104"},{"R14"},{"D5519"},{"Y105"},{"D5775"},{"D5903"},{"Y106"},{"D6159"},{"D6287"},
{"D6415"},{"D6543"},{"D6671"},{"D6799"}},
{{"D16"},{"Y108"},{"D272"},{"D400"},{"Y109"},{"D656"},
{"D784"},{"Y110"},{"D1040"},{"D1168"},{"Y111"},{"D1424"},{"B27"},{"Y112"},{"D1808"},{"B82"},
{"Y113"},{"D2192"},{"D2320"},{"D2448"},{"D2576"},{"D2704"},{"D2832"},{"R324"},{"D3088"},{"D3216"},
{"D3344"},{"B277"},{"D3600"},{"D3728"},{"D3856"},{"D3984"},{"R182"},{"D4240"},{"D4368"},{"D4496"},
{"B420"},{"D4752"},{"D4880"},{"D5008"},{"D5136"},{"D5264"},{"D5392"},{"D5520"},{"D5648"},{"D5776"},
{"D5904"},{"D6032"},{"D6160"},{"Y107"},{"D6416"},{"D6544"},{"D6672"},{"D6800"}},
{{"D17"},{"D145"},
{"D273"},{"D401"},{"D529"},{"D657"},{"D785"},{"D913"},{"D1041"},{"D1169"},{"D1297"},{"B17"},
{"D1553"},{"D1681"},{"D1809"},{"D1937"},{"R445"},{"D2193"},{"Y114"},{"D2449"},{"B161"},{"Y115"},
{"R345"},{"B209"},{"Y116"},{"R296"},{"D3345"},{"Y117"},{"R250"},{"B309"},{"Y118"},{"R198"},
{"B356"},{"Y119"},{"R149"},{"B403"},{"Y120"},{"D4753"},{"D4881"},{"D5009"},{"D5137"},{"D5265"},
{"D5393"},{"D5521"},{"D5649"},{"D5777"},{"D5905"},{"D6033"},{"D6161"},{"D6289"},{"D6417"},{"D6545"},
{"D6673"},{"D6801"}},
{{"D18"},{"D146"},{"Y126"},{"D402"},{"D530"},{"D658"},{"D786"},{"D914"},
{"D1042"},{"D1170"},{"B2"},{"D1426"},{"D1554"},{"B53"},{"D1810"},{"D1938"},{"D2066"},{"D2194"},
{"D2322"},{"B146"},{"D2578"},{"D2706"},{"D2834"},{"D2962"},{"R319"},{"B245"},{"D3346"},{"D3474"},
{"B293"},{"D3730"},{"D3858"},{"B346"},{"D4114"},{"D4242"},{"B392"},{"D4498"},{"R122"},{"B442"},
{"Y121"},{"R73"},{"D5138"},{"Y122"},{"R27"},{"D5522"},{"Y123"},{"D5778"},{"D5906"},{"Y124"},
{"D6162"},{"D6290"},{"Y125"},{"D6546"},{"D6674"},{"D6802"}},
{{"D19"},{"D147"},{"D275"},{"D403"},
{"Y127"},{"D659"},{"D787"},{"Y128"},{"D1043"},{"D1171"},{"Y129"},{"D1427"},{"B41"},{"Y130"},
{"D1811"},{"B90"},{"Y131"},{"R433"},{"B135"},{"Y132"},{"R389"},{"D2707"},{"Y133"},{"D2963"},
{"D3091"},{"D3219"},{"R287"},{"D3475"},{"D3603"},{"R237"},{"B329"},{"D3987"},{"R192"},{"B383"},
{"D4371"},{"D4499"},{"D4627"},{"D4755"},{"D4883"},{"D5011"},{"D5139"},{"D5267"},{"D5395"},{"D5523"},
{"D5651"},{"D5779"},{"D5907"},{"D6035"},{"D6163"},{"D6291"},{"D6419"},{"D6547"},{"D6675"},{"D6803"}
},
{{"D20"},{"D148"},{"D276"},{"D404"},{"D532"},{"D660"},{"D788"},{"D916"},{"D1044"},{"D1172"},
{"R554"},{"B29"},{"D1556"},{"D1684"},{"B72"},{"D1940"},{"D2068"},{"B127"},{"D2324"},{"R402"},
{"D2580"},{"D2708"},{"D2836"},{"D2964"},{"Y134"},{"R308"},{"D3348"},{"Y135"},{"D3604"},{"D3732"},
{"Y136"},{"R207"},{"B365"},{"Y137"},{"D4372"},{"D4500"},{"Y138"},{"R112"},{"D4884"},{"Y139"},
{"R70"},{"D5268"},{"Y140"},{"R15"},{"D5652"},{"D5780"},{"D5908"},{"D6036"},{"D6164"},{"D6292"},
{"D6420"},{"D6548"},{"D6676"},{"D6804"}},
{{"D21"},{"D149"},{"Y144"},{"D405"},{"D533"},{"Y145"},
{"D789"},{"D917"},{"Y146"},{"D1173"},{"D1301"},{"D1429"},{"D1557"},{"B62"},{"D1813"},{"R472"},
{"B111"},{"D2197"},{"D2325"},{"B159"},{"D2581"},{"D2709"},{"B211"},{"D2965"},{"D3093"},{"D3221"},
{"D3349"},{"R278"},{"D3605"},{"D3733"},{"D3861"},{"D3989"},{"D4117"},{"D4245"},{"D4373"},{"D4501"},
{"R136"},{"D4757"},{"D4885"},{"D5013"},{"D5141"},{"D5269"},{"R39"},{"D5525"},{"Y141"},{"D5781"},
{"D5909"},{"Y142"},{"D6165"},{"D6293"},{"Y143"},{"D6549"},{"D6677"},{"D6805"}},
{{"D22"},{"D150"},
{"D278"},{"D406"},{"D534"},{"D662"},{"D790"},{"D918"},{"D1046"},{"D1174"},{"Y147"},{"R541"},
{"B49"},{"Y148"},{"D1814"},{"D1942"},{"Y149"},{"R440"},{"B143"},{"Y150"},{"D2582"},{"B195"},
{"Y151"},{"D2966"},{"D3094"},{"Y152"},{"D3350"},{"D3478"},{"Y153"},{"D3734"},{"B343"},{"D3990"},
{"R203"},{"B385"},{"D4374"},{"D4502"},{"D4630"},{"D4758"},{"D4886"},{"B490"},{"D5142"},{"R52"},
{"D5398"},{"D5526"},{"D5654"},{"D5782"},{"D5910"},{"D6038"},{"D6166"},{"D6294"},{"D6422"},{"D6550"},
{"D6678"},{"D6806"}},
{{"D23"},{"D151"},{"D279"},{"D407"},{"D535"},{"D663"},{"D791"},{"D919"},
{"D1047"},{"D1175"},{"D1303"},{"B38"},{"D1559"},{"R512"},{"D1815"},{"D1943"},{"D2071"},{"D2199"},
{"D2327"},{"D2455"},{"B186"},{"D2711"},{"D2839"},{"B230"},{"D3095"},{"R315"},{"D3351"},{"D3479"},
{"R268"},{"D3735"},{"Y154"},{"R217"},{"D4119"},{"Y155"},{"R176"},{"D4503"},{"Y156"},{"R126"},
{"D4887"},{"Y157"},{"R77"},{"B526"},{"Y158"},{"R30"},{"D5655"},{"Y159"},{"D5911"},{"D6039"},
{"Y160"},{"D6295"},{"D6423"},{"D6551"},{"D6679"},{"D6807"}},
{{"D24"},{"D152"},{"Y162"},{"D408"},
{"D536"},{"Y163"},{"D792"},{"D920"},{"Y164"},{"R577"},{"B21"},{"Y165"},{"D1560"},{"B71"},
{"Y166"},{"R487"},{"B122"},{"D2200"},{"R435"},{"B172"},{"D2584"},{"R387"},{"D2840"},{"D2968"},
{"R338"},{"D3224"},{"D3352"},{"D3480"},{"B314"},{"D3736"},{"R238"},{"D3992"},{"D4120"},{"D4248"},
{"B412"},{"D4504"},{"R142"},{"D4760"},{"D4888"},{"R93"},{"D5144"},{"D5272"},{"R43"},{"B555"},
{"D5656"},{"R0"},{"D5912"},{"D6040"},{"D6168"},{"D6296"},{"Y161"},{"D6552"},{"D6680"},{"D6808"}
},
{{"D25"},{"D153"},{"D281"},{"D409"},{"D537"},{"D665"},{"D793"},{"D921"},{"D1049"},{"B11"},
{"D1305"},{"R551"},{"D1561"},{"D1689"},{"R505"},{"B113"},{"Y167"},{"R458"},{"D2329"},{"Y168"},
{"R405"},{"B202"},{"Y169"},{"R356"},{"B253"},{"Y170"},{"D3353"},{"D3481"},{"Y171"},{"D3737"},
{"D3865"},{"Y172"},{"D4121"},{"D4249"},{"Y173"},{"R164"},{"B448"},{"D4761"},{"R111"},{"B498"},
{"D5145"},{"D5273"},{"B542"},{"D5529"},{"D5657"},{"D5785"},{"D5913"},{"D6041"},{"D6169"},{"D6297"},
{"D6425"},{"D6553"},{"D6681"},{"D6809"}},
{{"D26"},{"D154"},{"D282"},{"Y180"},{"D538"},{"D666"},
{"D794"},{"D922"},{"D1050"},{"D1178"},{"R571"},{"D1434"},{"D1562"},{"R521"},{"B98"},{"D1946"},
{"D2074"},{"B145"},{"D2330"},{"D2458"},{"D2586"},{"D2714"},{"D2842"},{"D2970"},{"D3098"},{"D3226"},
{"B289"},{"D3482"},{"D3610"},{"D3738"},{"D3866"},{"D3994"},{"B388"},{"D4250"},{"R180"},{"D4506"},
{"Y174"},{"R130"},{"B486"},{"Y175"},{"R86"},{"D5274"},{"Y176"},{"R33"},{"B583"},{"Y177"},
{"D5914"},{"D6042"},{"Y178"},{"D6298"},{"D6426"},{"Y179"},{"D6682"},{"D6810"}},
{{"D27"},{"D155"},
{"D283"},{"D411"},{"D539"},{"Y181"},{"D795"},{"D923"},{"Y182"},{"D1179"},{"D1307"},{"Y183"},
{"D1563"},{"B79"},{"Y184"},{"R489"},{"D2075"},{"Y185"},{"D2331"},{"B179"},{"Y186"},{"D2715"},
{"D2843"},{"D2971"},{"D3099"},{"D3227"},{"D3355"},{"D3483"},{"B323"},{"D3739"},{"D3867"},{"D3995"},
{"D4123"},{"D4251"},{"B419"},{"D4507"},{"R158"},{"B475"},{"D4891"},{"D5019"},{"D5147"},{"D5275"},
{"R61"},{"D5531"},{"D5659"},{"D5787"},{"D5915"},{"D6043"},{"D6171"},{"D6299"},{"D6427"},{"D6555"},
{"D6683"},{"D6811"}},
{{"D28"},{"D156"},{"D284"},{"D412"},{"D540"},{"D668"},{"D796"},{"D924"},
{"D1052"},{"B23"},{"D1308"},{"D1436"},{"B69"},{"D1692"},{"D1820"},{"B116"},{"D2076"},{"R467"},
{"B163"},{"D2460"},{"D2588"},{"B215"},{"Y187"},{"D2972"},{"B260"},{"Y188"},{"D3356"},{"B317"},
{"Y189"},{"D3740"},{"D3868"},{"Y190"},{"R225"},{"B411"},{"Y191"},{"D4508"},{"B457"},{"Y192"},
{"D4892"},{"D5020"},{"Y193"},{"R74"},{"D5404"},{"D5532"},{"R25"},{"D5788"},{"D5916"},{"D6044"},
{"D6172"},{"D6300"},{"D6428"},{"D6556"},{"D6684"},{"D6812"}},
{{"D29"},{"D157"},{"D285"},{"Y198"},
{"D541"},{"D669"},{"Y199"},{"D925"},{"B7"},{"Y200"},{"D1309"},{"B55"},{"D1565"},{"D1693"},
{"D1821"},{"D1949"},{"D2077"},{"B152"},{"D2333"},{"D2461"},{"D2589"},{"D2717"},{"R383"},{"B249"},
{"D3101"},{"D3229"},{"D3357"},{"D3485"},{"D3613"},{"D3741"},{"D3869"},{"D3997"},{"B398"},{"D4253"},
{"D4381"},{"D4509"},{"D4637"},{"D4765"},{"D4893"},{"D5021"},{"R97"},{"B538"},{"Y194"},{"R47"},
{"B587"},{"Y195"},{"D5917"},{"D6045"},{"Y196"},{"D6301"},{"D6429"},{"Y197"},{"D6685"},{"D6813"}
},
{{"D30"},{"D158"},{"D286"},{"D414"},{"D542"},{"D670"},{"D798"},{"D926"},{"D1054"},{"D1182"},
{"B43"},{"Y201"},{"R548"},{"D1694"},{"Y202"},{"R503"},{"D2078"},{"Y203"},{"D2334"},{"B192"},
{"Y204"},{"D2718"},{"B234"},{"Y205"},{"D3102"},{"D3230"},{"Y206"},{"D3486"},{"B332"},{"D3742"},
{"R260"},{"D3998"},{"D4126"},{"R209"},{"D4382"},{"D4510"},{"R160"},{"D4766"},{"D4894"},{"R117"},
{"D5150"},{"D5278"},{"R63"},{"B577"},{"D5662"},{"R18"},{"B623"},{"D6046"},{"D6174"},{"D6302"},
{"D6430"},{"D6558"},{"D6686"},{"D6814"}},
{{"D31"},{"D159"},{"D287"},{"D415"},{"D543"},{"D671"},
{"D799"},{"D927"},{"D1055"},{"B35"},{"D1311"},{"D1439"},{"B76"},{"D1695"},{"R524"},{"B126"},
{"D2079"},{"R470"},{"D2335"},{"D2463"},{"D2591"},{"D2719"},{"D2847"},{"R375"},{"D3103"},{"D3231"},
{"D3359"},{"D3487"},{"Y207"},{"D3743"},{"B367"},{"Y208"},{"D4127"},{"D4255"},{"Y209"},{"D4511"},
{"D4639"},{"Y210"},{"R132"},{"B521"},{"Y211"},{"R82"},{"D5407"},{"Y212"},{"R40"},{"D5791"},
{"Y213"},{"D6047"},{"D6175"},{"D6303"},{"D6431"},{"D6559"},{"D6687"},{"D6815"}},
{{"D32"},{"D160"},
{"D288"},{"Y216"},{"D544"},{"D672"},{"Y217"},{"D928"},{"B20"},{"Y218"},{"D1312"},{"B63"},
{"Y219"},{"D1696"},{"B112"},{"Y220"},{"D2080"},{"D2208"},{"D2336"},{"R447"},{"D2592"},{"D2720"},
{"R398"},{"B259"},{"D3104"},{"R347"},{"D3360"},{"D3488"},{"D3616"},{"D3744"},{"D3872"},{"D4000"},
{"D4128"},{"D4256"},{"D4384"},{"B455"},{"D4640"},{"R150"},{"B503"},{"D5024"},{"D5152"},{"B550"},
{"D5408"},{"R62"},{"B604"},{"D5792"},{"R9"},{"D6048"},{"Y214"},{"D6304"},{"D6432"},{"Y215"},
{"D6688"},{"D6816"}},
{{"D33"},{"D161"},{"D289"},{"D417"},{"D545"},{"D673"},{"D801"},{"B10"},
{"D1057"},{"R610"},{"D1313"},{"D1441"},{"R565"},{"B100"},{"D1825"},{"D1953"},{"B147"},{"Y221"},
{"D2337"},{"B201"},{"Y222"},{"R415"},{"D2849"},{"Y223"},{"D3105"},{"B294"},{"Y224"},{"D3489"},
{"D3617"},{"Y225"},{"R272"},{"B390"},{"Y226"},{"R218"},{"D4385"},{"D4513"},{"R170"},{"D4769"},
{"D4897"},{"D5025"},{"D5153"},{"D5281"},{"D5409"},{"D5537"},{"D5665"},{"R26"},{"D5921"},{"D6049"},
{"D6177"},{"D6305"},{"D6433"},{"D6561"},{"D6689"},{"D6817"}},
{{"D34"},{"Y233"},{"D290"},{"D418"},
{"D546"},{"D674"},{"D802"},{"D930"},{"D1058"},{"B39"},{"D1314"},{"R580"},{"D1570"},{"D1698"},
{"D1826"},{"B136"},{"D2082"},{"D2210"},{"D2338"},{"D2466"},{"D2594"},{"D2722"},{"D2850"},{"D2978"},
{"D3106"},{"D3234"},{"R341"},{"B331"},{"D3618"},{"D3746"},{"B382"},{"D4002"},{"R241"},{"B429"},
{"Y227"},{"R195"},{"B482"},{"Y228"},{"R143"},{"D5026"},{"Y229"},{"D5282"},{"D5410"},{"Y230"},
{"R48"},{"B619"},{"Y231"},{"R4"},{"D6178"},{"Y232"},{"D6434"},{"D6562"},{"D6690"},{"D6818"}
},
{{"D35"},{"D163"},{"D291"},{"Y234"},{"D547"},{"D675"},{"Y235"},{"R645"},{"D1059"},{"Y236"},
{"D1315"},{"D1443"},{"Y237"},{"D1699"},{"B128"},{"Y238"},{"R508"},{"B175"},{"Y239"},{"R457"},
{"B225"},{"Y240"},{"R408"},{"D2979"},{"D3107"},{"D3235"},{"D3363"},{"D3491"},{"R311"},{"D3747"},
{"D3875"},{"R258"},{"D4131"},{"D4259"},{"R213"},{"D4515"},{"D4643"},{"R163"},{"B516"},{"D5027"},
{"R113"},{"D5283"},{"D5411"},{"R64"},{"B607"},{"D5795"},{"R16"},{"D6051"},{"D6179"},{"D6307"},
{"D6435"},{"D6563"},{"D6691"},{"D6819"}},
{{"D36"},{"D164"},{"D292"},{"D420"},{"D548"},{"D676"},
{"D804"},{"B13"},{"D1060"},{"D1188"},{"D1316"},{"D1444"},{"D1572"},{"B110"},{"D1828"},{"R519"},
{"D2084"},{"D2212"},{"D2340"},{"D2468"},{"D2596"},{"R427"},{"D2852"},{"Y241"},{"D3108"},{"D3236"},
{"Y242"},{"D3492"},{"D3620"},{"Y243"},{"R282"},{"B400"},{"Y244"},{"D4260"},{"D4388"},{"Y245"},
{"D4644"},{"D4772"},{"Y246"},{"D5028"},{"B546"},{"D5284"},{"R83"},{"D5540"},{"D5668"},{"R42"},
{"D5924"},{"D6052"},{"D6180"},{"D6308"},{"D6436"},{"D6564"},{"D6692"},{"D6820"}},
{{"D37"},{"Y251"},
{"D293"},{"D421"},{"Y252"},{"D677"},{"D805"},{"Y253"},{"R643"},{"B48"},{"D1317"},{"R590"},
{"D1573"},{"D1701"},{"D1829"},{"D1957"},{"D2085"},{"D2213"},{"B198"},{"D2469"},{"R444"},{"B246"},
{"D2853"},{"D2981"},{"B298"},{"D3237"},{"D3365"},{"D3493"},{"D3621"},{"R305"},{"D3877"},{"D4005"},
{"R253"},{"D4261"},{"D4389"},{"D4517"},{"D4645"},{"D4773"},{"D4901"},{"B536"},{"Y247"},{"R104"},
{"D5413"},{"Y248"},{"D5669"},{"B629"},{"Y249"},{"R8"},{"D6181"},{"Y250"},{"D6437"},{"D6565"},
{"D6693"},{"D6821"}},
{{"D38"},{"D166"},{"D294"},{"D422"},{"D550"},{"D678"},{"D806"},{"R663"},
{"B36"},{"Y254"},{"R607"},{"D1446"},{"Y255"},{"D1702"},{"B133"},{"Y256"},{"R516"},{"D2214"},
{"Y257"},{"R469"},{"D2598"},{"Y258"},{"R419"},{"D2982"},{"Y259"},{"R368"},{"B327"},{"Y260"},
{"D3622"},{"D3750"},{"D3878"},{"D4006"},{"D4134"},{"D4262"},{"R222"},{"D4518"},{"D4646"},{"D4774"},
{"B523"},{"D5030"},{"R128"},{"D5286"},{"D5414"},{"D5542"},{"B625"},{"D5798"},{"D5926"},{"D6054"},
{"D6182"},{"D6310"},{"D6438"},{"D6566"},{"D6694"},{"D6822"}},
{{"D39"},{"D167"},{"D295"},{"D423"},
{"D551"},{"D679"},{"R679"},{"D935"},{"D1063"},{"D1191"},{"D1319"},{"D1447"},{"D1575"},{"B120"},
{"D1831"},{"R532"},{"B171"},{"D2215"},{"R481"},{"D2471"},{"D2599"},{"D2727"},{"D2855"},{"D2983"},
{"R392"},{"D3239"},{"D3367"},{"R340"},{"B361"},{"Y261"},{"D3879"},{"D4007"},{"Y262"},{"R239"},
{"B463"},{"Y263"},{"R194"},{"D4775"},{"Y264"},{"R147"},{"D5159"},{"Y265"},{"D5415"},{"D5543"},
{"Y266"},{"R50"},{"D5927"},{"D6055"},{"D6183"},{"D6311"},{"D6439"},{"D6567"},{"D6695"},{"D6823"}
},
{{"D40"},{"Y269"},{"D296"},{"D424"},{"Y270"},{"D680"},{"B9"},{"Y271"},{"D1064"},{"D1192"},
{"Y272"},{"D1448"},{"B107"},{"Y273"},{"D1832"},{"D1960"},{"D2088"},{"R506"},{"B208"},{"D2472"},
{"R454"},{"B256"},{"D2856"},{"D2984"},{"D3112"},{"D3240"},{"D3368"},{"B349"},{"D3624"},{"D3752"},
{"B399"},{"D4008"},{"D4136"},{"B450"},{"D4392"},{"D4520"},{"D4648"},{"D4776"},{"D4904"},{"D5032"},
{"D5160"},{"R120"},{"D5416"},{"D5544"},{"D5672"},{"B646"},{"Y267"},{"D6056"},{"D6184"},{"Y268"},
{"D6440"},{"D6568"},{"D6696"},{"D6824"}},
{{"D41"},{"D169"},{"D297"},{"D425"},{"D553"},{"D681"},
{"D809"},{"R666"},{"B47"},{"D1193"},{"D1321"},{"B97"},{"D1577"},{"R573"},{"D1833"},{"Y274"},
{"D2089"},{"B191"},{"Y275"},{"D2473"},{"D2601"},{"Y276"},{"D2857"},{"D2985"},{"Y277"},{"R378"},
{"D3369"},{"Y278"},{"R327"},{"B389"},{"Y279"},{"R277"},{"D4137"},{"Y280"},{"D4393"},{"D4521"},
{"D4649"},{"R181"},{"D4905"},{"D5033"},{"D5161"},{"D5289"},{"D5417"},{"R92"},{"B633"},{"D5801"},
{"R35"},{"D6057"},{"D6185"},{"D6313"},{"D6441"},{"D6569"},{"D6697"},{"D6825"}},
{{"D42"},{"D170"},
{"D298"},{"D426"},{"D554"},{"D682"},{"D810"},{"B33"},{"D1066"},{"R644"},{"B84"},{"D1450"},
{"D1578"},{"D1706"},{"D1834"},{"R539"},{"D2090"},{"D2218"},{"D2346"},{"D2474"},{"D2602"},{"R443"},
{"D2858"},{"D2986"},{"R399"},{"D3242"},{"D3370"},{"D3498"},{"B375"},{"D3754"},{"R300"},{"B422"},
{"D4138"},{"D4266"},{"D4394"},{"Y281"},{"R208"},{"B518"},{"Y282"},{"D5034"},{"B567"},{"Y283"},
{"R105"},{"D5546"},{"Y284"},{"R57"},{"D5930"},{"Y285"},{"R10"},{"B717"},{"Y286"},{"D6570"},
{"D6698"},{"D6826"}},
{{"D43"},{"Y287"},{"D299"},{"D427"},{"Y288"},{"R704"},{"B22"},{"Y289"},
{"D1067"},{"B75"},{"Y290"},{"D1451"},{"B119"},{"Y291"},{"D1835"},{"B173"},{"Y292"},{"D2219"},
{"D2347"},{"Y293"},{"D2603"},{"D2731"},{"D2859"},{"R418"},{"B315"},{"D3243"},{"D3371"},{"B362"},
{"D3627"},{"R322"},{"D3883"},{"D4011"},{"D4139"},{"B462"},{"D4395"},{"R220"},{"D4651"},{"D4779"},
{"D4907"},{"D5035"},{"D5163"},{"D5291"},{"D5419"},{"D5547"},{"D5675"},{"D5803"},{"D5931"},{"R31"},
{"B699"},{"D6315"},{"D6443"},{"D6571"},{"D6699"},{"D6827"}},
{{"D44"},{"D172"},{"D300"},{"D428"},
{"D556"},{"B8"},{"D812"},{"D940"},{"D1068"},{"D1196"},{"D1324"},{"B103"},{"D1580"},{"D1708"},
{"B160"},{"D1964"},{"D2092"},{"B207"},{"D2348"},{"D2476"},{"D2604"},{"Y294"},{"R437"},{"D2988"},
{"Y295"},{"D3244"},{"B347"},{"Y296"},{"D3628"},{"B396"},{"Y297"},{"R293"},{"B443"},{"Y298"},
{"D4396"},{"D4524"},{"Y299"},{"D4780"},{"D4908"},{"Y300"},{"R146"},{"B588"},{"D5420"},{"R94"},
{"B641"},{"D5804"},{"D5932"},{"B692"},{"D6188"},{"D6316"},{"D6444"},{"D6572"},{"D6700"},{"D6828"}
},
{{"D45"},{"D173"},{"Y305"},{"D429"},{"D557"},{"Y306"},{"D813"},{"D941"},{"D1069"},{"D1197"},
{"B92"},{"D1453"},{"D1581"},{"D1709"},{"D1837"},{"R552"},{"B187"},{"D2221"},{"D2349"},{"B242"},
{"D2605"},{"D2733"},{"D2861"},{"D2989"},{"R409"},{"D3245"},{"D3373"},{"R357"},{"B384"},{"D3757"},
{"D3885"},{"B430"},{"D4141"},{"R263"},{"D4397"},{"D4525"},{"D4653"},{"B532"},{"D4909"},{"R168"},
{"D5165"},{"Y301"},{"D5421"},{"B624"},{"Y302"},{"R66"},{"B673"},{"Y303"},{"R21"},{"D6317"},
{"Y304"},{"D6573"},{"D6701"},{"D6829"}},
{{"D46"},{"D174"},{"D302"},{"D430"},{"D558"},{"R719"},
{"D814"},{"Y307"},{"D1070"},{"D1198"},{"Y308"},{"D1454"},{"D1582"},{"Y309"},{"D1838"},{"D1966"},
{"Y310"},{"D2222"},{"D2350"},{"Y311"},{"R473"},{"B275"},{"Y312"},{"D2990"},{"D3118"},{"Y313"},
{"D3374"},{"D3502"},{"D3630"},{"R335"},{"D3886"},{"D4014"},{"D4142"},{"D4270"},{"D4398"},{"R229"},
{"D4654"},{"D4782"},{"D4910"},{"B563"},{"D5166"},{"R135"},{"B617"},{"D5550"},{"D5678"},{"B666"},
{"D5934"},{"D6062"},{"B712"},{"D6318"},{"D6446"},{"D6574"},{"D6702"},{"D6830"}},
{{"D47"},{"D175"},
{"D303"},{"D431"},{"D559"},{"D687"},{"D815"},{"R686"},{"D1071"},{"D1199"},{"R638"},{"B117"},
{"D1583"},{"D1711"},{"D1839"},{"D1967"},{"R547"},{"D2223"},{"D2351"},{"R494"},{"D2607"},{"D2735"},
{"R446"},{"D2991"},{"D3119"},{"D3247"},{"B363"},{"Y314"},{"D3631"},{"B410"},{"Y315"},{"R303"},
{"D4143"},{"Y316"},{"D4399"},{"D4527"},{"Y317"},{"R200"},{"D4911"},{"Y318"},{"R153"},{"D5295"},
{"Y319"},{"D5551"},{"B648"},{"Y320"},{"R56"},{"D6063"},{"D6191"},{"R13"},{"D6447"},{"D6575"},
{"D6703"},{"D6831"}},
{{"D48"},{"D176"},{"Y323"},{"D432"},{"B5"},{"Y324"},{"R705"},{"B56"},
{"Y325"},{"D1200"},{"B104"},{"Y326"},{"D1584"},{"B151"},{"D1840"},{"R560"},{"D2096"},{"D2224"},
{"R515"},{"D2480"},{"D2608"},{"D2736"},{"B300"},{"D2992"},{"R416"},{"D3248"},{"D3376"},{"D3504"},
{"D3632"},{"D3760"},{"D3888"},{"B441"},{"D4144"},{"R274"},{"D4400"},{"D4528"},{"D4656"},{"D4784"},
{"D4912"},{"R175"},{"D5168"},{"D5296"},{"D5424"},{"B637"},{"D5680"},{"D5808"},{"D5936"},{"Y321"},
{"D6192"},{"B737"},{"Y322"},{"D6576"},{"D6704"},{"D6832"}},
{{"D49"},{"D177"},{"D305"},{"D433"},
{"D561"},{"R728"},{"B44"},{"D945"},{"R683"},{"D1201"},{"D1329"},{"D1457"},{"D1585"},{"Y327"},
{"R581"},{"B189"},{"Y328"},{"R533"},{"B241"},{"Y329"},{"R483"},{"B283"},{"Y330"},{"D2993"},
{"B335"},{"Y331"},{"R390"},{"D3505"},{"Y332"},{"R343"},{"D3889"},{"Y333"},{"D4145"},{"D4273"},
{"D4401"},{"R244"},{"D4657"},{"D4785"},{"R196"},{"B576"},{"D5169"},{"R145"},{"D5425"},{"D5553"},
{"D5681"},{"B672"},{"D5937"},{"D6065"},{"B724"},{"D6321"},{"R1"},{"D6577"},{"D6705"},{"D6833"}
},
{{"D50"},{"D178"},{"D306"},{"D434"},{"D562"},{"B32"},{"D818"},{"D946"},{"B77"},{"D1202"},
{"R647"},{"D1458"},{"D1586"},{"R599"},{"B178"},{"D1970"},{"R556"},{"B221"},{"D2354"},{"R501"},
{"B269"},{"D2738"},{"D2866"},{"D2994"},{"D3122"},{"D3250"},{"B368"},{"D3506"},{"R361"},{"D3762"},
{"D3890"},{"R310"},{"B467"},{"Y334"},{"D4402"},{"B517"},{"Y335"},{"D4786"},{"B566"},{"Y336"},
{"R169"},{"D5298"},{"Y337"},{"R121"},{"D5682"},{"Y338"},{"R65"},{"D6066"},{"Y339"},{"R23"},
{"B757"},{"Y340"},{"D6706"},{"D6834"}},
{{"D51"},{"D179"},{"Y341"},{"D435"},{"B14"},{"Y342"},
{"R721"},{"B68"},{"Y343"},{"R669"},{"D1331"},{"Y344"},{"D1587"},{"D1715"},{"Y345"},{"R570"},
{"B212"},{"Y346"},{"R528"},{"D2483"},{"D2611"},{"R474"},{"D2867"},{"D2995"},{"D3123"},{"D3251"},
{"D3379"},{"R381"},{"B409"},{"D3763"},{"D3891"},{"B458"},{"D4147"},{"R285"},{"D4403"},{"D4531"},
{"D4659"},{"D4787"},{"D4915"},{"R186"},{"B598"},{"D5299"},{"R138"},{"D5555"},{"D5683"},{"R88"},
{"D5939"},{"D6067"},{"D6195"},{"B742"},{"D6451"},{"D6579"},{"D6707"},{"D6835"}},
{{"D52"},{"D180"},
{"D308"},{"B3"},{"D564"},{"D692"},{"B50"},{"D948"},{"D1076"},{"B101"},{"D1332"},{"R640"},
{"B148"},{"D1716"},{"R593"},{"D1972"},{"D2100"},{"D2228"},{"D2356"},{"Y347"},{"D2612"},{"D2740"},
{"Y348"},{"D2996"},{"D3124"},{"Y349"},{"D3380"},{"D3508"},{"Y350"},{"D3764"},{"D3892"},{"Y351"},
{"R306"},{"D4276"},{"Y352"},{"R249"},{"D4660"},{"Y353"},{"D4916"},{"B582"},{"D5172"},{"R155"},
{"D5428"},{"D5556"},{"R107"},{"D5812"},{"D5940"},{"R55"},{"D6196"},{"D6324"},{"D6452"},{"D6580"},
{"D6708"},{"D6836"}},
{{"D53"},{"D181"},{"D309"},{"Y359"},{"R760"},{"B40"},{"Y360"},{"D949"},
{"B87"},{"D1205"},{"R661"},{"D1461"},{"D1589"},{"D1717"},{"B190"},{"D1973"},{"R562"},{"B233"},
{"D2357"},{"R511"},{"B281"},{"D2741"},{"D2869"},{"B334"},{"D3125"},{"D3253"},{"D3381"},{"D3509"},
{"D3637"},{"B432"},{"D3893"},{"D4021"},{"B478"},{"D4277"},{"D4405"},{"D4533"},{"D4661"},{"D4789"},
{"D4917"},{"Y354"},{"R172"},{"B618"},{"Y355"},{"R124"},{"D5685"},{"Y356"},{"R75"},{"B720"},
{"Y357"},{"R34"},{"B766"},{"Y358"},{"D6709"},{"D6837"}},
{{"D54"},{"D182"},{"D310"},{"D438"},
{"B26"},{"D694"},{"R729"},{"D950"},{"Y361"},{"R678"},{"B121"},{"Y362"},{"R629"},{"D1718"},
{"Y363"},{"D1974"},{"B218"},{"Y364"},{"R537"},{"D2486"},{"Y365"},{"D2742"},{"B318"},{"Y366"},
{"R442"},{"D3254"},{"D3382"},{"R393"},{"B415"},{"D3766"},{"D3894"},{"B465"},{"D4150"},{"R295"},
{"B509"},{"D4534"},{"D4662"},{"D4790"},{"D4918"},{"R191"},{"B610"},{"D5302"},{"R144"},{"B656"},
{"D5686"},{"R103"},{"D5942"},{"D6070"},{"D6198"},{"B752"},{"D6454"},{"D6582"},{"D6710"},{"D6838"}
},
{{"D55"},{"D183"},{"D311"},{"D439"},{"D567"},{"R745"},{"B59"},{"D951"},{"R699"},{"D1207"},
{"D1335"},{"D1463"},{"B158"},{"D1719"},{"R600"},{"D1975"},{"D2103"},{"D2231"},{"B255"},{"D2487"},
{"D2615"},{"B308"},{"D2871"},{"R453"},{"D3127"},{"Y367"},{"D3383"},{"B406"},{"Y368"},{"D3767"},
{"D3895"},{"Y369"},{"D4151"},{"B495"},{"Y370"},{"D4535"},{"B547"},{"Y371"},{"D4919"},{"B597"},
{"Y372"},{"D5303"},{"D5431"},{"Y373"},{"D5687"},{"B689"},{"D5943"},{"D6071"},{"B743"},{"D6327"},
{"R19"},{"D6583"},{"D6711"},{"D6839"}},
{{"D56"},{"D184"},{"D312"},{"Y377"},{"D568"},{"D696"},
{"Y378"},{"R715"},{"B95"},{"Y379"},{"D1336"},{"D1464"},{"Y380"},{"D1720"},{"B194"},{"D1976"},
{"D2104"},{"D2232"},{"D2360"},{"D2488"},{"D2616"},{"D2744"},{"R476"},{"B345"},{"D3128"},{"D3256"},
{"D3384"},{"D3512"},{"R380"},{"B438"},{"D3896"},{"D4024"},{"B484"},{"D4280"},{"R280"},{"D4536"},
{"D4664"},{"D4792"},{"B580"},{"D5048"},{"R185"},{"B634"},{"D5432"},{"R134"},{"B684"},{"Y374"},
{"R85"},{"B726"},{"Y375"},{"R41"},{"B776"},{"Y376"},{"D6712"},{"D6840"}},
{{"D57"},{"D185"},
{"D313"},{"D441"},{"B37"},{"D697"},{"D825"},{"B83"},{"D1081"},{"D1209"},{"B130"},{"D1465"},
{"D1593"},{"D1721"},{"Y381"},{"R597"},{"D2105"},{"Y382"},{"D2361"},{"D2489"},{"Y383"},{"R499"},
{"D2873"},{"Y384"},{"D3129"},{"B381"},{"Y385"},{"R397"},{"B423"},{"Y386"},{"R354"},{"D4025"},
{"D4153"},{"D4281"},{"D4409"},{"D4537"},{"D4665"},{"B575"},{"D4921"},{"R206"},{"D5177"},{"D5305"},
{"D5433"},{"B671"},{"D5689"},{"R108"},{"D5945"},{"D6073"},{"D6201"},{"D6329"},{"D6457"},{"D6585"},
{"D6713"},{"D6841"}},
{{"D58"},{"D186"},{"D314"},{"B25"},{"D570"},{"R755"},{"D826"},{"D954"},
{"D1082"},{"B123"},{"D1338"},{"D1466"},{"B168"},{"D1722"},{"D1850"},{"B222"},{"D2106"},{"R566"},
{"D2362"},{"D2490"},{"D2618"},{"B319"},{"D2874"},{"D3002"},{"D3130"},{"D3258"},{"D3386"},{"B416"},
{"D3642"},{"D3770"},{"D3898"},{"Y387"},{"D4154"},{"D4282"},{"Y388"},{"R269"},{"B554"},{"Y389"},
{"R221"},{"D5050"},{"Y390"},{"D5306"},{"D5434"},{"Y391"},{"D5690"},{"B705"},{"Y392"},{"R79"},
{"B748"},{"Y393"},{"D6458"},{"B799"},{"D6714"},{"D6842"}},
{{"D59"},{"D187"},{"B15"},{"Y395"},
{"R774"},{"B64"},{"Y396"},{"R726"},{"B106"},{"Y397"},{"R682"},{"B156"},{"Y398"},{"R634"},
{"B205"},{"Y399"},{"R582"},{"B252"},{"Y400"},{"D2491"},{"D2619"},{"D2747"},{"D2875"},{"D3003"},
{"D3131"},{"R438"},{"B397"},{"D3515"},{"R386"},{"D3771"},{"D3899"},{"D4027"},{"D4155"},{"D4283"},
{"D4411"},{"B541"},{"D4667"},{"D4795"},{"B596"},{"D5051"},{"R193"},{"B639"},{"D5435"},{"D5563"},
{"B687"},{"D5819"},{"R102"},{"D6075"},{"D6203"},{"R49"},{"B787"},{"Y394"},{"R2"},{"D6843"}
},
{{"D60"},{"D188"},{"D316"},{"D444"},{"D572"},{"D700"},{"R748"},{"D956"},{"D1084"},{"D1212"},
{"B140"},{"D1468"},{"D1596"},{"B197"},{"D1852"},{"D1980"},{"B243"},{"D2236"},{"R555"},{"B291"},
{"Y401"},{"R504"},{"D2876"},{"Y402"},{"D3132"},{"D3260"},{"Y403"},{"R406"},{"B439"},{"Y404"},
{"D3900"},{"B483"},{"Y405"},{"D4284"},{"B531"},{"Y406"},{"R264"},{"D4796"},{"D4924"},{"R211"},
{"D5180"},{"D5308"},{"D5436"},{"B676"},{"D5692"},{"R115"},{"B730"},{"D6076"},{"R69"},{"B774"},
{"D6460"},{"R22"},{"D6716"},{"D6844"}},
{{"D61"},{"Y412"},{"R814"},{"B31"},{"Y413"},{"R766"},
{"B86"},{"D957"},{"R718"},{"D1213"},{"D1341"},{"R675"},{"B180"},{"D1725"},{"R622"},{"B227"},
{"D2109"},{"D2237"},{"B274"},{"D2493"},{"D2621"},{"B328"},{"D2877"},{"R477"},{"B373"},{"D3261"},
{"R430"},{"D3517"},{"D3645"},{"D3773"},{"B472"},{"D4029"},{"D4157"},{"D4285"},{"D4413"},{"D4541"},
{"D4669"},{"Y407"},{"D4925"},{"D5053"},{"Y408"},{"R183"},{"B662"},{"Y409"},{"R141"},{"D5821"},
{"Y410"},{"R90"},{"D6205"},{"Y411"},{"R38"},{"B815"},{"D6717"},{"D6845"}},
{{"D62"},{"R840"},
{"D318"},{"D446"},{"R790"},{"D702"},{"Y414"},{"R739"},{"D1086"},{"Y415"},{"R690"},{"B166"},
{"Y416"},{"R646"},{"B213"},{"Y417"},{"R598"},{"D2238"},{"Y418"},{"R543"},{"B310"},{"Y419"},
{"D2878"},{"D3006"},{"Y420"},{"D3262"},{"D3390"},{"D3518"},{"R401"},{"D3774"},{"D3902"},{"D4030"},
{"D4158"},{"D4286"},{"R302"},{"B551"},{"D4670"},{"D4798"},{"B601"},{"D5054"},{"R202"},{"D5310"},
{"D5438"},{"D5566"},{"D5694"},{"D5822"},{"D5950"},{"B746"},{"D6206"},{"R59"},{"B794"},{"D6590"},
{"R11"},{"B850"}},
{{"D63"},{"B12"},{"D319"},{"R803"},{"B58"},{"D703"},{"D831"},{"B102"},
{"D1087"},{"D1215"},{"B154"},{"D1471"},{"R659"},{"B203"},{"D1855"},{"D1983"},{"D2111"},{"D2239"},
{"R561"},{"D2495"},{"D2623"},{"D2751"},{"D2879"},{"D3007"},{"R465"},{"B401"},{"Y421"},{"D3519"},
{"D3647"},{"Y422"},{"D3903"},{"D4031"},{"Y423"},{"R326"},{"D4415"},{"Y424"},{"D4671"},{"B594"},
{"Y425"},{"D5055"},{"D5183"},{"Y426"},{"R174"},{"D5567"},{"D5695"},{"R131"},{"B732"},{"D6079"},
{"R76"},{"B784"},{"D6463"},{"D6591"},{"B830"},{"D6847"}},
{{"D64"},{"Y430"},{"D320"},{"D448"},
{"Y431"},{"D704"},{"B89"},{"Y432"},{"R727"},{"B138"},{"Y433"},{"D1472"},{"D1600"},{"D1728"},
{"R632"},{"B235"},{"D2112"},{"R585"},{"B285"},{"D2496"},{"D2624"},{"B340"},{"D2880"},{"D3008"},
{"D3136"},{"D3264"},{"D3392"},{"D3520"},{"D3648"},{"D3776"},{"B479"},{"D4032"},{"D4160"},{"B533"},
{"D4416"},{"R292"},{"D4672"},{"D4800"},{"R248"},{"B626"},{"D5184"},{"R201"},{"B674"},{"Y427"},
{"D5696"},{"B721"},{"Y428"},{"R95"},{"B773"},{"Y429"},{"R51"},{"B818"},{"D6720"},{"R6"}
},
{{"D65"},{"R848"},{"B28"},{"D449"},{"R802"},{"B81"},{"D833"},{"D961"},{"D1089"},{"D1217"},
{"R702"},{"D1473"},{"Y434"},{"R652"},{"B229"},{"Y435"},{"D2113"},{"B276"},{"Y436"},{"R553"},
{"D2625"},{"Y437"},{"D2881"},{"D3009"},{"Y438"},{"D3265"},{"D3393"},{"Y439"},{"D3649"},{"D3777"},
{"Y440"},{"R358"},{"D4161"},{"D4289"},{"R309"},{"B561"},{"D4673"},{"D4801"},{"B613"},{"D5057"},
{"D5185"},{"B661"},{"D5441"},{"D5569"},{"D5697"},{"D5825"},{"D5953"},{"B756"},{"D6209"},{"D6337"},
{"B804"},{"D6593"},{"R20"},{"B853"}},
{{"D66"},{"D194"},{"D322"},{"R813"},{"B66"},{"D706"},
{"R769"},{"B115"},{"D1090"},{"R717"},{"D1346"},{"D1474"},{"R668"},{"D1730"},{"D1858"},{"R621"},
{"B257"},{"D2242"},{"R576"},{"D2498"},{"D2626"},{"R523"},{"D2882"},{"D3010"},{"D3138"},{"B405"},
{"D3394"},{"D3522"},{"B459"},{"D3778"},{"D3906"},{"B502"},{"Y441"},{"D4290"},{"D4418"},{"Y442"},
{"R281"},{"B605"},{"Y443"},{"R240"},{"D5186"},{"Y444"},{"R190"},{"B694"},{"Y445"},{"D5826"},
{"D5954"},{"Y446"},{"D6210"},{"D6338"},{"D6466"},{"D6594"},{"B843"},{"D6850"}},
{{"D67"},{"Y448"},
{"R841"},{"B57"},{"Y449"},{"R784"},{"D835"},{"Y450"},{"R738"},{"B153"},{"Y451"},{"R687"},
{"D1603"},{"Y452"},{"D1859"},{"D1987"},{"Y453"},{"D2243"},{"D2371"},{"D2499"},{"R550"},{"B342"},
{"D2883"},{"D3011"},{"D3139"},{"D3267"},{"D3395"},{"B445"},{"D3651"},{"D3779"},{"D3907"},{"D4035"},
{"R348"},{"B535"},{"D4419"},{"D4547"},{"D4675"},{"D4803"},{"R259"},{"D5059"},{"D5187"},{"D5315"},
{"B681"},{"D5571"},{"D5699"},{"D5827"},{"D5955"},{"R106"},{"B779"},{"Y447"},{"R58"},{"B829"},
{"D6723"},{"D6851"}},
{{"D68"},{"D196"},{"B42"},{"D452"},{"R804"},{"B93"},{"D836"},{"D964"},
{"B141"},{"D1220"},{"R710"},{"D1476"},{"D1604"},{"D1732"},{"D1860"},{"D1988"},{"D2116"},{"B287"},
{"Y454"},{"R564"},{"D2628"},{"Y455"},{"D2884"},{"B380"},{"Y456"},{"D3268"},{"B426"},{"Y457"},
{"R424"},{"D3780"},{"Y458"},{"R370"},{"D4164"},{"Y459"},{"D4420"},{"D4548"},{"Y460"},{"D4804"},
{"B620"},{"D5060"},{"D5188"},{"B669"},{"D5444"},{"D5572"},{"B719"},{"D5828"},{"R129"},{"D6084"},
{"D6212"},{"R81"},{"D6468"},{"D6596"},{"R36"},{"D6852"}},
{{"D69"},{"D197"},{"Y466"},{"R825"},
{"B73"},{"D709"},{"R776"},{"D965"},{"D1093"},{"D1221"},{"B174"},{"D1477"},{"D1605"},{"D1733"},
{"D1861"},{"D1989"},{"B268"},{"D2245"},{"R586"},{"D2501"},{"D2629"},{"D2757"},{"B369"},{"D3013"},
{"D3141"},{"D3269"},{"D3397"},{"D3525"},{"B468"},{"D3781"},{"R394"},{"B510"},{"D4165"},{"R344"},
{"D4421"},{"D4549"},{"D4677"},{"D4805"},{"Y461"},{"D5061"},{"D5189"},{"Y462"},{"D5445"},{"B706"},
{"Y463"},{"D5829"},{"B760"},{"Y464"},{"R100"},{"D6341"},{"Y465"},{"D6597"},{"D6725"},{"D6853"}
},
{{"D70"},{"D198"},{"D326"},{"B65"},{"Y467"},{"D710"},{"B114"},{"Y468"},{"D1094"},{"B164"},
{"Y469"},{"R697"},{"D1606"},{"Y470"},{"R655"},{"D1990"},{"Y471"},{"D2246"},{"B311"},{"Y472"},
{"D2630"},{"D2758"},{"Y473"},{"D3014"},{"D3142"},{"D3270"},{"R461"},{"B453"},{"D3654"},{"R407"},
{"B500"},{"D4038"},{"R366"},{"B545"},{"D4422"},{"R312"},{"D4678"},{"D4806"},{"D4934"},{"B649"},
{"D5190"},{"D5318"},{"D5446"},{"D5574"},{"D5702"},{"B740"},{"D5958"},{"R116"},{"B788"},{"D6342"},
{"R67"},{"B841"},{"D6726"},{"D6854"}},
{{"D71"},{"D199"},{"D327"},{"D455"},{"R815"},{"D711"},
{"D839"},{"D967"},{"D1095"},{"D1223"},{"D1351"},{"D1479"},{"D1607"},{"R672"},{"D1863"},{"D1991"},
{"D2119"},{"D2247"},{"D2375"},{"R574"},{"D2631"},{"D2759"},{"D2887"},{"D3015"},{"Y474"},{"D3271"},
{"D3399"},{"Y475"},{"R428"},{"D3783"},{"Y476"},{"R377"},{"B539"},{"Y477"},{"R330"},{"D4551"},
{"Y478"},{"D4807"},{"D4935"},{"Y479"},{"D5191"},{"B679"},{"Y480"},{"R188"},{"D5703"},{"D5831"},
{"R140"},{"D6087"},{"D6215"},{"R87"},{"B824"},{"D6599"},{"R45"},{"D6855"}},
{{"D72"},{"D200"},
{"Y484"},{"D456"},{"D584"},{"Y485"},{"R785"},{"D968"},{"Y486"},{"R741"},{"B184"},{"D1480"},
{"D1608"},{"B237"},{"D1864"},{"D1992"},{"D2120"},{"D2248"},{"R594"},{"D2504"},{"D2632"},{"D2760"},
{"D2888"},{"D3016"},{"R496"},{"D3272"},{"D3400"},{"D3528"},{"D3656"},{"D3784"},{"D3912"},{"D4040"},
{"D4168"},{"R350"},{"B572"},{"D4552"},{"D4680"},{"D4808"},{"D4936"},{"D5064"},{"D5192"},{"D5320"},
{"D5448"},{"B716"},{"Y481"},{"R154"},{"D5960"},{"Y482"},{"R114"},{"D6344"},{"Y483"},{"D6600"},
{"D6728"},{"D6856"}},
{{"D73"},{"D201"},{"D329"},{"D457"},{"D585"},{"R808"},{"D841"},{"D969"},
{"R759"},{"D1225"},{"Y487"},{"R707"},{"D1609"},{"Y488"},{"R660"},{"B265"},{"Y489"},{"R612"},
{"B316"},{"Y490"},{"D2633"},{"D2761"},{"Y491"},{"D3017"},{"B414"},{"Y492"},{"D3401"},{"D3529"},
{"Y493"},{"D3785"},{"B508"},{"D4041"},{"R376"},{"B559"},{"D4425"},{"R321"},{"B611"},{"D4809"},
{"D4937"},{"B653"},{"D5193"},{"D5321"},{"D5449"},{"D5577"},{"R178"},{"D5833"},{"D5961"},{"R133"},
{"B802"},{"D6345"},{"D6473"},{"B846"},{"D6729"},{"D6857"}},
{{"D74"},{"D202"},{"D330"},{"D458"},
{"R827"},{"D714"},{"D842"},{"R777"},{"D1098"},{"D1226"},{"D1354"},{"B204"},{"D1610"},{"D1738"},
{"D1866"},{"D1994"},{"R635"},{"B302"},{"D2378"},{"D2506"},{"B357"},{"D2762"},{"R540"},{"D3018"},
{"D3146"},{"D3274"},{"D3402"},{"D3530"},{"D3658"},{"D3786"},{"Y494"},{"R396"},{"D4170"},{"Y495"},
{"R339"},{"B591"},{"Y496"},{"D4810"},{"B647"},{"Y497"},{"D5194"},{"B690"},{"Y498"},{"D5578"},
{"D5706"},{"Y499"},{"R151"},{"D6090"},{"Y500"},{"D6346"},{"B836"},{"D6602"},{"D6730"},{"D6858"}
},
{{"D75"},{"D203"},{"Y502"},{"R843"},{"B99"},{"Y503"},{"R800"},{"B144"},{"Y504"},{"R746"},
{"B193"},{"Y505"},{"D1611"},{"B239"},{"Y506"},{"D1995"},{"D2123"},{"D2251"},{"R606"},{"D2507"},
{"D2635"},{"D2763"},{"D2891"},{"D3019"},{"D3147"},{"B440"},{"D3403"},{"D3531"},{"B487"},{"D3787"},
{"D3915"},{"D4043"},{"D4171"},{"D4299"},{"D4427"},{"D4555"},{"D4683"},{"B628"},{"D4939"},{"D5067"},
{"B678"},{"D5323"},{"R214"},{"B731"},{"D5707"},{"R166"},{"D5963"},{"D6091"},{"R118"},{"B828"},
{"Y501"},{"R71"},{"D6731"},{"D6859"}},
{{"D76"},{"D204"},{"D332"},{"D460"},{"D588"},{"R816"},
{"B134"},{"D972"},{"D1100"},{"B181"},{"D1356"},{"D1484"},{"B232"},{"D1740"},{"R671"},{"B278"},
{"Y507"},{"D2252"},{"B325"},{"Y508"},{"D2636"},{"D2764"},{"Y509"},{"R530"},{"B421"},{"Y510"},
{"R479"},{"D3532"},{"Y511"},{"D3788"},{"B524"},{"Y512"},{"D4172"},{"B565"},{"Y513"},{"R331"},
{"D4684"},{"D4812"},{"D4940"},{"D5068"},{"D5196"},{"R232"},{"B711"},{"D5580"},{"D5708"},{"B764"},
{"D5964"},{"R137"},{"B810"},{"D6348"},{"R89"},{"D6604"},{"D6732"},{"D6860"}},
{{"D77"},{"D205"},
{"D333"},{"Y520"},{"R842"},{"D717"},{"D845"},{"R787"},{"B165"},{"D1229"},{"D1357"},{"B219"},
{"D1613"},{"R693"},{"D1869"},{"D1997"},{"D2125"},{"D2253"},{"D2381"},{"D2509"},{"B359"},{"D2765"},
{"D2893"},{"B408"},{"D3149"},{"D3277"},{"D3405"},{"D3533"},{"D3661"},{"B507"},{"D3917"},{"D4045"},
{"B553"},{"D4301"},{"D4429"},{"D4557"},{"Y514"},{"R301"},{"B650"},{"Y515"},{"R256"},{"B702"},
{"Y516"},{"R205"},{"D5709"},{"Y517"},{"R162"},{"D6093"},{"Y518"},{"R109"},{"D6477"},{"Y519"},
{"D6733"},{"D6861"}},
{{"D78"},{"D206"},{"D334"},{"D462"},{"B105"},{"Y521"},{"R807"},{"B155"},
{"Y522"},{"R756"},{"B200"},{"Y523"},{"D1614"},{"D1742"},{"Y524"},{"D1998"},{"B303"},{"Y525"},
{"D2382"},{"D2510"},{"Y526"},{"R569"},{"D2894"},{"D3022"},{"R520"},{"B447"},{"D3406"},{"D3534"},
{"D3662"},{"D3790"},{"D3918"},{"D4046"},{"D4174"},{"D4302"},{"B590"},{"D4558"},{"D4686"},{"D4814"},
{"D4942"},{"D5070"},{"B685"},{"D5326"},{"R228"},{"D5582"},{"D5710"},{"R177"},{"B785"},{"D6094"},
{"R127"},{"B833"},{"D6478"},{"D6606"},{"D6734"},{"D6862"}},
{{"D79"},{"D207"},{"D335"},{"D463"},
{"D591"},{"R824"},{"B139"},{"D975"},{"R783"},{"B196"},{"D1359"},{"R733"},{"B240"},{"D1743"},
{"D1871"},{"D1999"},{"D2127"},{"R630"},{"D2383"},{"D2511"},{"R584"},{"D2767"},{"Y527"},{"D3023"},
{"D3151"},{"Y528"},{"R493"},{"D3535"},{"Y529"},{"D3791"},{"D3919"},{"Y530"},{"R391"},{"D4303"},
{"Y531"},{"R346"},{"D4687"},{"Y532"},{"R299"},{"B675"},{"Y533"},{"R243"},{"B723"},{"D5583"},
{"D5711"},{"D5839"},{"D5967"},{"D6095"},{"B823"},{"D6351"},{"R98"},{"D6607"},{"D6735"},{"D6863"}
},
{{"D80"},{"D208"},{"D336"},{"Y538"},{"R846"},{"B129"},{"Y539"},{"R795"},{"B182"},{"Y540"},
{"R750"},{"D1488"},{"D1616"},{"D1744"},{"B272"},{"D2000"},{"R653"},{"D2256"},{"D2384"},{"R608"},
{"B376"},{"D2768"},{"D2896"},{"D3024"},{"D3152"},{"D3280"},{"D3408"},{"D3536"},{"R464"},{"D3792"},
{"D3920"},{"D4048"},{"B570"},{"D4304"},{"R359"},{"D4560"},{"D4688"},{"D4816"},{"D4944"},{"D5072"},
{"D5200"},{"B713"},{"Y534"},{"D5584"},{"D5712"},{"Y535"},{"R167"},{"B809"},{"Y536"},{"R119"},
{"D6480"},{"Y537"},{"D6736"},{"D6864"}},
{{"D81"},{"D209"},{"D337"},{"D465"},{"D593"},{"D721"},
{"R823"},{"B167"},{"D1105"},{"R770"},{"B210"},{"Y541"},{"R720"},{"D1745"},{"Y542"},{"R676"},
{"D2129"},{"Y543"},{"D2385"},{"D2513"},{"Y544"},{"D2769"},{"B404"},{"Y545"},{"D3153"},{"B454"},
{"Y546"},{"D3537"},{"D3665"},{"D3793"},{"D3921"},{"D4049"},{"D4177"},{"R379"},{"D4433"},{"D4561"},
{"R336"},{"D4817"},{"D4945"},{"D5073"},{"B697"},{"D5329"},{"R236"},{"D5585"},{"D5713"},{"D5841"},
{"D5969"},{"D6097"},{"D6225"},{"D6353"},{"D6481"},{"D6609"},{"D6737"},{"D6865"}},
{{"D82"},{"D210"},
{"D338"},{"D466"},{"D594"},{"R835"},{"B149"},{"D978"},{"D1106"},{"D1234"},{"D1362"},{"R743"},
{"D1618"},{"D1746"},{"R696"},{"D2002"},{"D2130"},{"R648"},{"D2386"},{"D2514"},{"D2642"},{"D2770"},
{"D2898"},{"D3026"},{"D3154"},{"D3282"},{"R497"},{"B496"},{"Y547"},{"D3794"},{"D3922"},{"Y548"},
{"D4178"},{"D4306"},{"Y549"},{"R355"},{"B640"},{"Y550"},{"D4946"},{"B691"},{"Y551"},{"R255"},
{"D5458"},{"Y552"},{"D5714"},{"B783"},{"Y553"},{"R157"},{"B832"},{"D6354"},{"D6482"},{"D6610"},
{"D6738"},{"D6866"}},
{{"D83"},{"D211"},{"D339"},{"Y556"},{"D595"},{"D723"},{"Y557"},{"D979"},
{"B185"},{"Y558"},{"R757"},{"D1491"},{"Y559"},{"R709"},{"B282"},{"Y560"},{"D2131"},{"B338"},
{"D2387"},{"D2515"},{"B379"},{"D2771"},{"D2899"},{"D3027"},{"D3155"},{"D3283"},{"D3411"},{"D3539"},
{"D3667"},{"B525"},{"D3923"},{"R423"},{"B574"},{"D4307"},{"R373"},{"D4563"},{"D4691"},{"D4819"},
{"D4947"},{"D5075"},{"R279"},{"B722"},{"D5459"},{"R227"},{"B768"},{"D5843"},{"D5971"},{"B820"},
{"Y554"},{"D6355"},{"D6483"},{"Y555"},{"D6739"},{"D6867"}},
{{"D84"},{"D212"},{"D340"},{"D468"},
{"D596"},{"D724"},{"R829"},{"B177"},{"D1108"},{"R779"},{"B223"},{"D1492"},{"R736"},{"D1748"},
{"D1876"},{"D2004"},{"D2132"},{"Y561"},{"R636"},{"B370"},{"Y562"},{"D2772"},{"D2900"},{"Y563"},
{"R536"},{"B469"},{"Y564"},{"D3540"},{"B515"},{"Y565"},{"D3924"},{"B562"},{"Y566"},{"D4308"},
{"D4436"},{"D4564"},{"R342"},{"D4820"},{"D4948"},{"R291"},{"B708"},{"D5332"},{"R247"},{"B755"},
{"D5716"},{"D5844"},{"D5972"},{"D6100"},{"D6228"},{"D6356"},{"D6484"},{"D6612"},{"D6740"},{"D6868"}
},
{{"D85"},{"Y573"},{"D341"},{"D469"},{"D597"},{"R850"},{"B162"},{"D981"},{"R798"},{"D1237"},
{"D1365"},{"R749"},{"B263"},{"D1749"},{"R706"},{"B313"},{"D2133"},{"R658"},{"B355"},{"D2517"},
{"R604"},{"B407"},{"D2901"},{"D3029"},{"B452"},{"D3285"},{"D3413"},{"D3541"},{"D3669"},{"R456"},
{"D3925"},{"D4053"},{"D4181"},{"D4309"},{"Y567"},{"D4565"},{"B651"},{"Y568"},{"R318"},{"B701"},
{"Y569"},{"R265"},{"B744"},{"Y570"},{"D5717"},{"B791"},{"Y571"},{"R165"},{"D6229"},{"Y572"},
{"D6485"},{"D6613"},{"D6741"},{"D6869"}},
{{"D86"},{"D214"},{"D342"},{"Y574"},{"D598"},{"D726"},
{"Y575"},{"R822"},{"D1110"},{"Y576"},{"D1366"},{"B251"},{"Y577"},{"R725"},{"B297"},{"Y578"},
{"R670"},{"D2262"},{"Y579"},{"R624"},{"D2646"},{"Y580"},{"D2902"},{"D3030"},{"D3158"},{"R529"},
{"B488"},{"D3542"},{"D3670"},{"D3798"},{"D3926"},{"D4054"},{"D4182"},{"D4310"},{"R385"},{"D4566"},
{"D4694"},{"R337"},{"D4950"},{"D5078"},{"R286"},{"D5334"},{"D5462"},{"R233"},{"B777"},{"D5846"},
{"D5974"},{"D6102"},{"D6230"},{"D6358"},{"D6486"},{"D6614"},{"D6742"},{"D6870"}},
{{"D87"},{"D215"},
{"D343"},{"D471"},{"D599"},{"D727"},{"R838"},{"B183"},{"D1111"},{"R786"},{"D1367"},{"D1495"},
{"D1623"},{"B280"},{"D1879"},{"R689"},{"D2135"},{"D2263"},{"D2391"},{"B377"},{"D2647"},{"R592"},
{"B428"},{"Y581"},{"D3159"},{"D3287"},{"Y582"},{"D3543"},{"B527"},{"Y583"},{"D3927"},{"D4055"},
{"Y584"},{"R404"},{"B621"},{"Y585"},{"D4695"},{"B667"},{"Y586"},{"D5079"},{"D5207"},{"D5335"},
{"R257"},{"D5591"},{"D5719"},{"D5847"},{"B814"},{"D6103"},{"D6231"},{"D6359"},{"D6487"},{"D6615"},
{"D6743"},{"D6871"}},
{{"D88"},{"Y591"},{"D344"},{"D472"},{"Y592"},{"D728"},{"D856"},{"Y593"},
{"R809"},{"B220"},{"D1368"},{"D1496"},{"B267"},{"D1752"},{"D1880"},{"B321"},{"D2136"},{"D2264"},
{"B364"},{"D2520"},{"R620"},{"B418"},{"D2904"},{"R563"},{"D3160"},{"D3288"},{"D3416"},{"D3544"},
{"D3672"},{"R466"},{"B557"},{"D4056"},{"D4184"},{"B608"},{"D4440"},{"D4568"},{"B655"},{"D4824"},
{"D4952"},{"D5080"},{"Y587"},{"R273"},{"B753"},{"Y588"},{"R230"},{"B803"},{"Y589"},{"D6104"},
{"B852"},{"Y590"},{"D6488"},{"D6616"},{"D6744"},{"D6872"}},
{{"D89"},{"D217"},{"D345"},{"D473"},
{"D601"},{"D729"},{"D857"},{"R832"},{"D1113"},{"Y594"},{"D1369"},{"B258"},{"Y595"},{"D1753"},
{"B305"},{"Y596"},{"D2137"},{"B352"},{"Y597"},{"D2521"},{"D2649"},{"Y598"},{"R583"},{"D3033"},
{"Y599"},{"R534"},{"D3417"},{"Y600"},{"D3673"},{"D3801"},{"D3929"},{"D4057"},{"B595"},{"D4313"},
{"D4441"},{"B645"},{"D4697"},{"D4825"},{"B693"},{"D5081"},{"D5209"},{"B739"},{"D5465"},{"R246"},
{"B793"},{"D5849"},{"R199"},{"D6105"},{"D6233"},{"D6361"},{"D6489"},{"D6617"},{"D6745"},{"D6873"}
},
{{"D90"},{"D218"},{"D346"},{"D474"},{"D602"},{"D730"},{"R851"},{"D986"},{"D1114"},{"R797"},
{"B244"},{"D1498"},{"D1626"},{"D1754"},{"D1882"},{"D2010"},{"D2138"},{"D2266"},{"D2394"},{"D2522"},
{"D2650"},{"D2778"},{"D2906"},{"D3034"},{"D3162"},{"D3290"},{"D3418"},{"D3546"},{"D3674"},{"Y601"},
{"R463"},{"B585"},{"Y602"},{"R410"},{"D4442"},{"Y603"},{"R362"},{"B682"},{"Y604"},{"R313"},
{"B725"},{"Y605"},{"D5466"},{"D5594"},{"Y606"},{"R223"},{"D5978"},{"D6106"},{"R171"},{"D6362"},
{"D6490"},{"D6618"},{"D6746"},{"D6874"}},
{{"D91"},{"Y609"},{"D347"},{"D475"},{"Y610"},{"D731"},
{"D859"},{"Y611"},{"D1115"},{"D1243"},{"Y612"},{"R771"},{"B279"},{"Y613"},{"D1883"},{"D2011"},
{"D2139"},{"D2267"},{"D2395"},{"D2523"},{"R628"},{"D2779"},{"D2907"},{"D3035"},{"D3163"},{"D3291"},
{"D3419"},{"B520"},{"D3675"},{"D3803"},{"B568"},{"D4059"},{"D4187"},{"D4315"},{"D4443"},{"D4571"},
{"D4699"},{"D4827"},{"D4955"},{"B714"},{"D5211"},{"R283"},{"B763"},{"D5595"},{"R234"},{"D5851"},
{"Y607"},{"D6107"},{"D6235"},{"Y608"},{"D6491"},{"D6619"},{"D6747"},{"D6875"}},
{{"D92"},{"D220"},
{"D348"},{"D476"},{"D604"},{"D732"},{"D860"},{"D988"},{"B217"},{"D1244"},{"R794"},{"B266"},
{"D1628"},{"R744"},{"B312"},{"Y614"},{"R694"},{"D2268"},{"Y615"},{"D2524"},{"D2652"},{"Y616"},
{"D2908"},{"B460"},{"Y617"},{"R545"},{"D3420"},{"Y618"},{"D3676"},{"B558"},{"Y619"},{"D4060"},
{"B603"},{"Y620"},{"D4444"},{"B659"},{"D4700"},{"D4828"},{"B700"},{"D5084"},{"D5212"},{"D5340"},
{"D5468"},{"R261"},{"B805"},{"D5852"},{"R210"},{"B845"},{"D6236"},{"D6364"},{"D6492"},{"D6620"},
{"D6748"},{"D6876"}},
{{"D93"},{"D221"},{"D349"},{"D477"},{"D605"},{"D733"},{"D861"},{"D989"},
{"D1117"},{"R806"},{"D1373"},{"D1501"},{"R758"},{"B301"},{"D1885"},{"D2013"},{"B354"},{"D2269"},
{"R662"},{"D2525"},{"D2653"},{"R619"},{"D2909"},{"D3037"},{"D3165"},{"B493"},{"D3421"},{"D3549"},
{"D3677"},{"D3805"},{"R475"},{"B592"},{"D4189"},{"D4317"},{"B638"},{"Y621"},{"D4701"},{"B695"},
{"Y622"},{"D5085"},{"B738"},{"Y623"},{"D5469"},{"B792"},{"Y624"},{"R231"},{"B838"},{"Y625"},
{"D6237"},{"D6365"},{"Y626"},{"D6621"},{"D6749"},{"D6877"}},
{{"D94"},{"Y627"},{"D350"},{"D478"},
{"Y628"},{"D734"},{"D862"},{"Y629"},{"R826"},{"B238"},{"Y630"},{"R782"},{"D1630"},{"Y631"},
{"R732"},{"D2014"},{"Y632"},{"R681"},{"D2398"},{"Y633"},{"D2654"},{"B437"},{"D2910"},{"R587"},
{"D3166"},{"D3294"},{"D3422"},{"B529"},{"D3678"},{"R492"},{"B578"},{"D4062"},{"D4190"},{"D4318"},
{"D4446"},{"D4574"},{"D4702"},{"D4830"},{"D4958"},{"D5086"},{"D5214"},{"D5342"},{"D5470"},{"D5598"},
{"D5726"},{"B821"},{"D5982"},{"D6110"},{"D6238"},{"D6366"},{"D6494"},{"D6622"},{"D6750"},{"D6878"}
},
{{"D95"},{"D223"},{"D351"},{"D479"},{"D607"},{"D735"},{"D863"},{"R845"},{"B228"},{"D1247"},
{"R801"},{"B273"},{"D1631"},{"D1759"},{"B326"},{"D2015"},{"D2143"},{"B378"},{"D2399"},{"D2527"},
{"D2655"},{"Y634"},{"R605"},{"B470"},{"Y635"},{"D3295"},{"B519"},{"Y636"},{"R507"},{"D3807"},
{"Y637"},{"D4063"},{"B614"},{"Y638"},{"R417"},{"D4575"},{"Y639"},{"D4831"},{"B710"},{"Y640"},
{"D5215"},{"B759"},{"D5471"},{"R271"},{"B812"},{"D5855"},{"R219"},{"D6111"},{"D6239"},{"D6367"},
{"D6495"},{"D6623"},{"D6751"},{"D6879"}},
{{"D96"},{"D224"},{"Y645"},{"D480"},{"D608"},{"Y646"},
{"D864"},{"D992"},{"D1120"},{"R818"},{"D1376"},{"D1504"},{"R768"},{"D1760"},{"D1888"},{"D2016"},
{"D2144"},{"D2272"},{"R673"},{"D2528"},{"D2656"},{"R623"},{"B456"},{"D3040"},{"D3168"},{"D3296"},
{"D3424"},{"R525"},{"B552"},{"D3808"},{"R485"},{"D4064"},{"D4192"},{"D4320"},{"D4448"},{"D4576"},
{"D4704"},{"D4832"},{"D4960"},{"D5088"},{"D5216"},{"Y641"},{"D5472"},{"B796"},{"Y642"},{"R235"},
{"B842"},{"Y643"},{"D6240"},{"D6368"},{"Y644"},{"D6624"},{"D6752"},{"D6880"}},
{{"D97"},{"D225"},
{"D353"},{"D481"},{"D609"},{"D737"},{"D865"},{"Y647"},{"R836"},{"D1249"},{"Y648"},{"R791"},
{"B296"},{"Y649"},{"R740"},{"D2017"},{"Y650"},{"R692"},{"B394"},{"Y651"},{"D2657"},{"D2785"},
{"Y652"},{"R601"},{"B492"},{"Y653"},{"R546"},{"D3553"},{"D3681"},{"D3809"},{"B589"},{"D4065"},
{"D4193"},{"B635"},{"D4449"},{"D4577"},{"B686"},{"D4833"},{"R351"},{"B733"},{"D5217"},{"R304"},
{"B782"},{"D5601"},{"R254"},{"B835"},{"D5985"},{"D6113"},{"D6241"},{"D6369"},{"D6497"},{"D6625"},
{"D6753"},{"D6881"}},
{{"D98"},{"D226"},{"D354"},{"D482"},{"D610"},{"D738"},{"D866"},{"D994"},
{"B236"},{"D1250"},{"D1378"},{"B290"},{"D1634"},{"R763"},{"D1890"},{"D2018"},{"R714"},{"B387"},
{"D2402"},{"D2530"},{"B435"},{"D2786"},{"R613"},{"D3042"},{"D3170"},{"R568"},{"D3426"},{"Y654"},
{"R518"},{"D3810"},{"Y655"},{"D4066"},{"D4194"},{"Y656"},{"D4450"},{"B677"},{"Y657"},{"D4834"},
{"D4962"},{"Y658"},{"D5218"},{"B775"},{"Y659"},{"R276"},{"D5730"},{"Y660"},{"R226"},{"D6114"},
{"D6242"},{"D6370"},{"D6498"},{"D6626"},{"D6754"},{"D6882"}},
{{"D99"},{"D227"},{"Y663"},{"D483"},
{"D611"},{"Y664"},{"D867"},{"D995"},{"Y665"},{"R831"},{"B270"},{"Y666"},{"D1635"},{"B320"},
{"D1891"},{"R730"},{"B372"},{"D2275"},{"D2403"},{"D2531"},{"D2659"},{"R633"},{"D2915"},{"D3043"},
{"D3171"},{"D3299"},{"D3427"},{"D3555"},{"D3683"},{"D3811"},{"R488"},{"B615"},{"D4195"},{"D4323"},
{"D4451"},{"D4579"},{"D4707"},{"D4835"},{"D4963"},{"D5091"},{"B761"},{"D5347"},{"R297"},{"D5603"},
{"D5731"},{"R245"},{"D5987"},{"Y661"},{"D6243"},{"D6371"},{"Y662"},{"D6627"},{"D6755"},{"D6883"}
},
{{"D100"},{"D228"},{"D356"},{"D484"},{"D612"},{"D740"},{"D868"},{"D996"},{"D1124"},{"B261"},
{"D1380"},{"R799"},{"B307"},{"Y667"},{"R753"},{"D2020"},{"Y668"},{"D2276"},{"D2404"},{"Y669"},
{"D2660"},{"D2788"},{"Y670"},{"R609"},{"D3172"},{"Y671"},{"D3428"},{"D3556"},{"Y672"},{"D3812"},
{"B600"},{"Y673"},{"R459"},{"B654"},{"D4452"},{"D4580"},{"D4708"},{"D4836"},{"R363"},{"B750"},
{"D5220"},{"R317"},{"D5476"},{"D5604"},{"R266"},{"B840"},{"D5988"},{"D6116"},{"D6244"},{"D6372"},
{"D6500"},{"D6628"},{"D6756"},{"D6884"}},
{{"D101"},{"D229"},{"D357"},{"D485"},{"D613"},{"D741"},
{"D869"},{"D997"},{"D1125"},{"D1253"},{"R819"},{"B299"},{"D1637"},{"D1765"},{"D1893"},{"D2021"},
{"R724"},{"B391"},{"D2405"},{"R677"},{"D2661"},{"D2789"},{"R627"},{"B491"},{"D3173"},{"D3301"},
{"B544"},{"D3557"},{"R526"},{"B584"},{"D3941"},{"D4069"},{"D4197"},{"Y674"},{"R434"},{"D4581"},
{"Y675"},{"R382"},{"B735"},{"Y676"},{"R333"},{"D5349"},{"Y677"},{"D5605"},{"B827"},{"Y678"},
{"D5989"},{"D6117"},{"Y679"},{"D6373"},{"D6501"},{"Y680"},{"D6757"},{"D6885"}},
{{"D102"},{"D230"},
{"Y681"},{"D486"},{"D614"},{"Y682"},{"D870"},{"D998"},{"Y683"},{"R837"},{"B284"},{"Y684"},
{"R789"},{"D1766"},{"Y685"},{"R742"},{"D2150"},{"Y686"},{"D2406"},{"B427"},{"D2662"},{"D2790"},
{"B480"},{"D3046"},{"R602"},{"B528"},{"D3430"},{"D3558"},{"B579"},{"D3814"},{"D3942"},{"D4070"},
{"D4198"},{"R455"},{"B670"},{"D4582"},{"D4710"},{"D4838"},{"D4966"},{"R353"},{"D5222"},{"D5350"},
{"D5478"},{"B819"},{"D5734"},{"D5862"},{"D5990"},{"D6118"},{"D6246"},{"D6374"},{"D6502"},{"D6630"},
{"D6758"},{"D6886"}},
{{"D103"},{"D231"},{"D359"},{"D487"},{"D615"},{"D743"},{"D871"},{"D999"},
{"D1127"},{"B271"},{"D1383"},{"R810"},{"D1639"},{"D1767"},{"R767"},{"B366"},{"D2151"},{"R713"},
{"B413"},{"Y687"},{"D2663"},{"D2791"},{"Y688"},{"R618"},{"B514"},{"Y689"},{"D3431"},{"B564"},
{"Y690"},{"D3815"},{"D3943"},{"Y691"},{"D4199"},{"B658"},{"Y692"},{"R425"},{"D4711"},{"Y693"},
{"R371"},{"D5095"},{"D5223"},{"R325"},{"B806"},{"D5607"},{"D5735"},{"B849"},{"D5991"},{"D6119"},
{"D6247"},{"D6375"},{"D6503"},{"D6631"},{"D6759"},{"D6887"}},
{{"D104"},{"D232"},{"D360"},{"Y699"},
{"D616"},{"D744"},{"Y700"},{"D1000"},{"D1128"},{"D1256"},{"D1384"},{"D1512"},{"D1640"},{"R778"},
{"B353"},{"D2024"},{"R737"},{"D2280"},{"D2408"},{"D2536"},{"B449"},{"D2792"},{"D2920"},{"B505"},
{"D3176"},{"R591"},{"D3432"},{"D3560"},{"D3688"},{"B599"},{"D3944"},{"R491"},{"B644"},{"D4328"},
{"D4456"},{"B696"},{"D4712"},{"D4840"},{"B741"},{"Y694"},{"D5224"},{"B789"},{"Y695"},{"R298"},
{"D5736"},{"Y696"},{"D5992"},{"D6120"},{"Y697"},{"D6376"},{"D6504"},{"Y698"},{"D6760"},{"D6888"}
},
{{"D105"},{"D233"},{"D361"},{"D489"},{"D617"},{"D745"},{"D873"},{"D1001"},{"Y701"},{"R849"},
{"B292"},{"Y702"},{"D1641"},{"B344"},{"Y703"},{"R752"},{"B393"},{"Y704"},{"R701"},{"B436"},
{"Y705"},{"R657"},{"D2921"},{"Y706"},{"D3177"},{"D3305"},{"D3433"},{"R557"},{"D3689"},{"D3817"},
{"D3945"},{"B631"},{"D4201"},{"D4329"},{"B683"},{"D4585"},{"D4713"},{"B727"},{"D4969"},{"R369"},
{"D5225"},{"D5353"},{"R320"},{"B825"},{"D5737"},{"R270"},{"D5993"},{"D6121"},{"D6249"},{"D6377"},
{"D6505"},{"D6633"},{"D6761"},{"D6889"}},
{{"D106"},{"D234"},{"D362"},{"D490"},{"D618"},{"D746"},
{"D874"},{"D1002"},{"D1130"},{"D1258"},{"D1386"},{"D1514"},{"B330"},{"D1770"},{"R775"},{"D2026"},
{"D2154"},{"D2282"},{"D2410"},{"D2538"},{"D2666"},{"B477"},{"D2922"},{"D3050"},{"D3178"},{"Y707"},
{"R575"},{"D3562"},{"Y708"},{"D3818"},{"B622"},{"Y709"},{"D4202"},{"B665"},{"Y710"},{"R429"},
{"D4714"},{"Y711"},{"D4970"},{"B769"},{"Y712"},{"D5354"},{"B811"},{"Y713"},{"R290"},{"D5866"},
{"D5994"},{"D6122"},{"D6250"},{"D6378"},{"D6506"},{"D6634"},{"D6762"},{"D6890"}},
{{"D107"},{"D235"},
{"D363"},{"Y717"},{"D619"},{"D747"},{"Y718"},{"D1003"},{"D1131"},{"Y719"},{"D1387"},{"D1515"},
{"Y720"},{"R788"},{"D1899"},{"D2027"},{"R747"},{"D2283"},{"D2411"},{"R691"},{"D2667"},{"D2795"},
{"R651"},{"B512"},{"D3179"},{"R595"},{"B560"},{"D3563"},{"D3691"},{"B609"},{"D3947"},{"R502"},
{"D4203"},{"D4331"},{"R450"},{"D4587"},{"D4715"},{"D4843"},{"B751"},{"D5099"},{"R352"},{"B798"},
{"D5483"},{"R307"},{"B847"},{"Y714"},{"D5995"},{"D6123"},{"Y715"},{"D6379"},{"D6507"},{"Y716"},
{"D6763"},{"D6891"}},
{{"D108"},{"D236"},{"D364"},{"D492"},{"D620"},{"D748"},{"D876"},{"D1004"},
{"D1132"},{"D1260"},{"D1388"},{"D1516"},{"R812"},{"D1772"},{"Y721"},{"R762"},{"D2156"},{"Y722"},
{"R711"},{"D2540"},{"Y723"},{"R667"},{"B501"},{"Y724"},{"R617"},{"B543"},{"Y725"},{"R567"},
{"D3692"},{"Y726"},{"R517"},{"B642"},{"D4204"},{"R471"},{"D4460"},{"D4588"},{"D4716"},{"B745"},
{"D4972"},{"D5100"},{"B786"},{"D5356"},{"R323"},{"B834"},{"D5740"},{"D5868"},{"D5996"},{"D6124"},
{"D6252"},{"D6380"},{"D6508"},{"D6636"},{"D6764"},{"D6892"}},
{{"D109"},{"D237"},{"D365"},{"D493"},
{"D621"},{"D749"},{"D877"},{"D1005"},{"D1133"},{"D1261"},{"D1389"},{"R834"},{"B337"},{"D1773"},
{"R780"},{"D2029"},{"D2157"},{"D2285"},{"B434"},{"D2541"},{"D2669"},{"D2797"},{"D2925"},{"R639"},
{"B534"},{"D3309"},{"D3437"},{"D3565"},{"D3693"},{"R542"},{"B630"},{"Y727"},{"R495"},{"D4333"},
{"Y728"},{"D4589"},{"B729"},{"Y729"},{"D4973"},{"B772"},{"Y730"},{"R349"},{"B822"},{"Y731"},
{"D5741"},{"D5869"},{"Y732"},{"D6125"},{"D6253"},{"Y733"},{"D6509"},{"D6637"},{"D6765"},{"D6893"}
},
{{"D110"},{"D238"},{"D366"},{"Y735"},{"D622"},{"D750"},{"Y736"},{"D1006"},{"D1134"},{"Y737"},
{"R847"},{"D1518"},{"Y738"},{"D1774"},{"D1902"},{"Y739"},{"D2158"},{"D2286"},{"Y740"},{"D2542"},
{"D2670"},{"D2798"},{"R654"},{"D3054"},{"D3182"},{"R611"},{"B571"},{"D3566"},{"R559"},{"D3822"},
{"D3950"},{"R509"},{"B663"},{"D4334"},{"D4462"},{"B715"},{"D4718"},{"R414"},{"B762"},{"D5102"},
{"R367"},{"D5358"},{"D5486"},{"D5614"},{"D5742"},{"D5870"},{"D5998"},{"D6126"},{"D6254"},{"D6382"},
{"D6510"},{"Y734"},{"D6766"},{"D6894"}},
{{"D111"},{"D239"},{"D367"},{"D495"},{"D623"},{"D751"},
{"D879"},{"D1007"},{"D1135"},{"D1263"},{"D1391"},{"D1519"},{"R821"},{"D1775"},{"D1903"},{"D2031"},
{"D2159"},{"D2287"},{"R723"},{"D2543"},{"Y741"},{"D2799"},{"B506"},{"Y742"},{"R626"},{"B556"},
{"Y743"},{"R578"},{"B602"},{"Y744"},{"R527"},{"B652"},{"Y745"},{"R478"},{"D4463"},{"Y746"},
{"R432"},{"B747"},{"D4975"},{"D5103"},{"B795"},{"D5359"},{"R334"},{"D5615"},{"D5743"},{"D5871"},
{"D5999"},{"D6127"},{"D6255"},{"D6383"},{"D6511"},{"D6639"},{"D6767"},{"D6895"}},
{{"D112"},{"Y752"},
{"D368"},{"D496"},{"Y753"},{"D752"},{"D880"},{"D1008"},{"D1136"},{"D1264"},{"D1392"},{"D1520"},
{"D1648"},{"D1776"},{"R796"},{"B402"},{"D2160"},{"D2288"},{"B446"},{"D2544"},{"R695"},{"B499"},
{"D2928"},{"D3056"},{"D3184"},{"D3312"},{"D3440"},{"D3568"},{"D3696"},{"D3824"},{"D3952"},{"D4080"},
{"D4208"},{"B688"},{"D4464"},{"D4592"},{"B736"},{"Y747"},{"D4976"},{"D5104"},{"Y748"},{"D5360"},
{"B837"},{"Y749"},{"D5744"},{"D5872"},{"Y750"},{"D6128"},{"D6256"},{"Y751"},{"D6512"},{"D6640"},
{"D6768"},{"D6896"}},
{{"D113"},{"D241"},{"D369"},{"D497"},{"D625"},{"D753"},{"Y754"},{"D1009"},
{"D1137"},{"Y755"},{"D1393"},{"D1521"},{"Y756"},{"D1777"},{"D1905"},{"Y757"},{"R761"},{"D2289"},
{"Y758"},{"R716"},{"B481"},{"Y759"},{"R664"},{"D3057"},{"Y760"},{"R614"},{"D3441"},{"D3569"},
{"D3697"},{"B627"},{"D3953"},{"D4081"},{"D4209"},{"D4337"},{"D4465"},{"B728"},{"D4721"},{"R421"},
{"B771"},{"D5105"},{"D5233"},{"B826"},{"D5489"},{"R329"},{"D5745"},{"D5873"},{"D6001"},{"D6129"},
{"D6257"},{"D6385"},{"D6513"},{"D6641"},{"D6769"},{"D6897"}},
{{"D114"},{"D242"},{"D370"},{"D498"},
{"D626"},{"D754"},{"D882"},{"D1010"},{"D1138"},{"D1266"},{"D1394"},{"D1522"},{"R830"},{"D1778"},
{"D1906"},{"R781"},{"D2162"},{"D2290"},{"R731"},{"B474"},{"D2674"},{"R684"},{"B522"},{"D3058"},
{"R642"},{"B569"},{"Y761"},{"D3570"},{"B616"},{"Y762"},{"R538"},{"D4082"},{"Y763"},{"D4338"},
{"B709"},{"Y764"},{"D4722"},{"D4850"},{"Y765"},{"D5106"},{"B808"},{"Y766"},{"D5490"},{"D5618"},
{"D5746"},{"D5874"},{"D6002"},{"D6130"},{"D6258"},{"D6386"},{"D6514"},{"D6642"},{"D6770"},{"D6898"}
},
{{"D115"},{"Y770"},{"D371"},{"D499"},{"Y771"},{"D755"},{"D883"},{"Y772"},{"D1139"},{"D1267"},
{"Y773"},{"D1523"},{"B358"},{"D1779"},{"D1907"},{"D2035"},{"D2163"},{"R751"},{"D2419"},{"D2547"},
{"D2675"},{"B504"},{"D2931"},{"D3059"},{"D3187"},{"D3315"},{"D3443"},{"D3571"},{"D3699"},{"D3827"},
{"D3955"},{"D4083"},{"R513"},{"B703"},{"D4467"},{"R462"},{"D4723"},{"D4851"},{"R413"},{"B801"},
{"D5235"},{"R365"},{"D5491"},{"Y767"},{"D5747"},{"D5875"},{"Y768"},{"D6131"},{"D6259"},{"Y769"},
{"D6515"},{"D6643"},{"D6771"},{"D6899"}},
{{"D116"},{"D244"},{"D372"},{"D500"},{"D628"},{"D756"},
{"D884"},{"D1012"},{"D1140"},{"D1268"},{"D1396"},{"D1524"},{"Y774"},{"D1780"},{"D1908"},{"Y775"},
{"R773"},{"D2292"},{"Y776"},{"D2548"},{"B489"},{"Y777"},{"R674"},{"B537"},{"Y778"},{"R625"},
{"B593"},{"Y779"},{"D3700"},{"D3828"},{"Y780"},{"R535"},{"D4212"},{"D4340"},{"D4468"},{"D4596"},
{"D4724"},{"R436"},{"B780"},{"D5108"},{"D5236"},{"D5364"},{"D5492"},{"D5620"},{"D5748"},{"D5876"},
{"D6004"},{"D6132"},{"D6260"},{"D6388"},{"D6516"},{"D6644"},{"D6772"},{"D6900"}},
{{"D117"},{"D245"},
{"D373"},{"D501"},{"D629"},{"D757"},{"D885"},{"D1013"},{"D1141"},{"D1269"},{"D1397"},{"D1525"},
{"R839"},{"D1781"},{"D1909"},{"R793"},{"B431"},{"D2293"},{"D2421"},{"B476"},{"D2677"},{"R700"},
{"B530"},{"D3061"},{"R649"},{"B573"},{"D3445"},{"R603"},{"D3701"},{"D3829"},{"R549"},{"D4085"},
{"Y781"},{"R498"},{"B718"},{"Y782"},{"D4725"},{"B767"},{"Y783"},{"R403"},{"B816"},{"Y784"},
{"D5493"},{"D5621"},{"Y785"},{"D5877"},{"D6005"},{"Y786"},{"D6261"},{"D6389"},{"D6517"},{"D6645"},
{"D6773"},{"D6901"}},
{{"D118"},{"Y788"},{"D374"},{"D502"},{"Y789"},{"D758"},{"D886"},{"Y790"},
{"D1142"},{"D1270"},{"Y791"},{"D1526"},{"B371"},{"Y792"},{"R811"},{"D2038"},{"Y793"},{"R765"},
{"D2422"},{"D2550"},{"R712"},{"D2806"},{"D2934"},{"D3062"},{"D3190"},{"D3318"},{"R615"},{"B612"},
{"D3702"},{"R572"},{"B664"},{"D4086"},{"D4214"},{"B707"},{"D4470"},{"D4598"},{"B758"},{"D4854"},
{"D4982"},{"B807"},{"D5238"},{"R372"},{"B851"},{"D5622"},{"D5750"},{"D5878"},{"D6006"},{"D6134"},
{"D6262"},{"Y787"},{"D6518"},{"D6646"},{"D6774"},{"D6902"}},
{{"D119"},{"D247"},{"D375"},{"D503"},
{"D631"},{"D759"},{"D887"},{"D1015"},{"D1143"},{"D1271"},{"D1399"},{"D1527"},{"D1655"},{"R828"},
{"D1911"},{"D2039"},{"D2167"},{"D2295"},{"Y794"},{"R735"},{"D2679"},{"Y795"},{"R685"},{"B549"},
{"Y796"},{"R641"},{"D3447"},{"Y797"},{"R589"},{"D3831"},{"Y798"},{"D4087"},{"B698"},{"Y799"},
{"D4471"},{"B749"},{"Y800"},{"R441"},{"B797"},{"D5111"},{"D5239"},{"B844"},{"D5495"},{"D5623"},
{"D5751"},{"D5879"},{"D6007"},{"D6135"},{"D6263"},{"D6391"},{"D6519"},{"D6647"},{"D6775"},{"D6903"}
},
{{"D120"},{"D248"},{"Y806"},{"D504"},{"D632"},{"D760"},{"D888"},{"D1016"},{"D1144"},{"D1272"},
{"D1400"},{"D1528"},{"D1656"},{"D1784"},{"D1912"},{"D2040"},{"D2168"},{"D2296"},{"R754"},{"D2552"},
{"D2680"},{"R703"},{"D2936"},{"D3064"},{"D3192"},{"B586"},{"D3448"},{"D3576"},{"B636"},{"D3832"},
{"R558"},{"B680"},{"D4216"},{"R510"},{"B734"},{"D4600"},{"D4728"},{"B778"},{"Y801"},{"D5112"},
{"D5240"},{"Y802"},{"D5496"},{"D5624"},{"Y803"},{"D5880"},{"D6008"},{"Y804"},{"D6264"},{"D6392"},
{"Y805"},{"D6648"},{"D6776"},{"D6904"}},
{{"D121"},{"D249"},{"D377"},{"D505"},{"Y807"},{"D761"},
{"D889"},{"Y808"},{"D1145"},{"D1273"},{"Y809"},{"D1529"},{"D1657"},{"Y810"},{"D1913"},{"D2041"},
{"Y811"},{"R772"},{"D2425"},{"Y812"},{"D2681"},{"D2809"},{"Y813"},{"R680"},{"D3193"},{"D3321"},
{"R631"},{"D3577"},{"D3705"},{"D3833"},{"B668"},{"D4089"},{"D4217"},{"D4345"},{"D4473"},{"R486"},
{"B770"},{"D4857"},{"R439"},{"B817"},{"D5241"},{"R388"},{"D5497"},{"D5625"},{"D5753"},{"D5881"},
{"D6009"},{"D6137"},{"D6265"},{"D6393"},{"D6521"},{"D6649"},{"D6777"},{"D6905"}},
{{"D122"},{"D250"},
{"D378"},{"D506"},{"D634"},{"D762"},{"D890"},{"D1018"},{"D1146"},{"D1274"},{"D1402"},{"D1530"},
{"D1658"},{"D1786"},{"D1914"},{"D2042"},{"R792"},{"B466"},{"D2426"},{"D2554"},{"B511"},{"D2810"},
{"R698"},{"D3066"},{"Y814"},{"R650"},{"D3450"},{"Y815"},{"D3706"},{"B660"},{"Y816"},{"D4090"},
{"D4218"},{"Y817"},{"D4474"},{"B754"},{"Y818"},{"D4858"},{"B800"},{"Y819"},{"D5242"},{"D5370"},
{"Y820"},{"D5626"},{"D5754"},{"D5882"},{"D6010"},{"D6138"},{"D6266"},{"D6394"},{"D6522"},{"D6650"},
{"D6778"},{"D6906"}},
{{"D123"},{"D251"},{"Y824"},{"D507"},{"D635"},{"Y825"},{"D891"},{"D1019"},
{"Y826"},{"D1275"},{"D1403"},{"D1531"},{"D1659"},{"D1787"},{"D1915"},{"R817"},{"D2171"},{"D2299"},
{"R764"},{"D2555"},{"D2683"},{"D2811"},{"B548"},{"D3067"},{"R665"},{"D3323"},{"D3451"},{"R616"},
{"D3707"},{"D3835"},{"D3963"},{"D4091"},{"D4219"},{"R522"},{"D4475"},{"D4603"},{"D4731"},{"B790"},
{"D4987"},{"D5115"},{"D5243"},{"D5371"},{"D5499"},{"D5627"},{"Y821"},{"D5883"},{"D6011"},{"Y822"},
{"D6267"},{"D6395"},{"Y823"},{"D6651"},{"D6779"},{"D6907"}},
{{"D124"},{"D252"},{"D380"},{"D508"},
{"D636"},{"D764"},{"D892"},{"D1020"},{"D1148"},{"D1276"},{"Y827"},{"D1532"},{"D1660"},{"Y828"},
{"R833"},{"D2044"},{"Y829"},{"D2300"},{"B485"},{"Y830"},{"R734"},{"B540"},{"Y831"},{"R688"},
{"B581"},{"Y832"},{"R637"},{"B632"},{"Y833"},{"R588"},{"D3964"},{"D4092"},{"R544"},{"D4348"},
{"D4476"},{"D4604"},{"B781"},{"D4860"},{"R448"},{"B831"},{"D5244"},{"R400"},{"D5500"},{"D5628"},
{"D5756"},{"D5884"},{"D6012"},{"D6140"},{"D6268"},{"D6396"},{"D6524"},{"D6652"},{"D6780"},{"D6908"}
},
{{"D125"},{"D253"},{"D381"},{"D509"},{"D637"},{"D765"},{"D893"},{"D1021"},{"D1149"},{"D1277"},
{"D1405"},{"D1533"},{"D1661"},{"D1789"},{"D1917"},{"D2045"},{"R805"},{"B473"},{"D2429"},{"D2557"},
{"D2685"},{"D2813"},{"R708"},{"D3069"},{"D3197"},{"R656"},{"D3453"},{"D3581"},{"D3709"},{"D3837"},
{"Y834"},{"D4093"},{"D4221"},{"Y835"},{"R514"},{"B765"},{"Y836"},{"R468"},{"B813"},{"Y837"},
{"D5245"},{"D5373"},{"Y838"},{"D5629"},{"D5757"},{"Y839"},{"D6013"},{"D6141"},{"Y840"},{"D6397"},
{"D6525"},{"D6653"},{"D6781"},{"D6909"}},
{{"D126"},{"D254"},{"Y842"},{"D510"},{"D638"},{"Y843"},
{"D894"},{"D1022"},{"Y844"},{"D1278"},{"D1406"},{"Y845"},{"D1662"},{"D1790"},{"Y846"},{"R820"},
{"D2174"},{"D2302"},{"D2430"},{"B513"},{"D2686"},{"R722"},{"D2942"},{"D3070"},{"D3198"},{"B606"},
{"D3454"},{"D3582"},{"B657"},{"D3838"},{"R579"},{"B704"},{"D4222"},{"R531"},{"D4478"},{"D4606"},
{"R484"},{"D4862"},{"D4990"},{"D5118"},{"B848"},{"D5374"},{"D5502"},{"D5630"},{"D5758"},{"D5886"},
{"D6014"},{"D6142"},{"D6270"},{"D6398"},{"Y841"},{"D6654"},{"D6782"},{"D6910"}},
{{"D127"},{"D255"},
{"D383"},{"D511"},{"D639"},{"D767"},{"D895"},{"D1023"},{"D1151"},{"D1279"},{"D1407"},{"D1535"},
{"D1663"},{"D1791"},{"R844"},{"D2047"},{"Y847"},{"D2303"},{"B494"},{"Y848"},{"D2687"},{"D2815"},
{"Y849"},{"D3071"},{"D3199"},{"Y850"},{"D3455"},{"B643"},{"Y851"},{"R596"},{"D3967"},{"Y852"},
{"D4223"},{"D4351"},{"D4479"},{"D4607"},{"D4735"},{"D4863"},{"R452"},{"B839"},{"D5247"},{"D5375"},
{"D5503"},{"D5631"},{"D5759"},{"D5887"},{"D6015"},{"D6143"},{"D6271"},{"D6399"},{"D6527"},{"D6655"},
{"D6783"},{"D6911"}}
};
