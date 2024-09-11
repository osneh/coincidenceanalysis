#ifndef XYPICMIC_H
#define XYPICMIC_H

#include <stdbool.h>

#define COLS 54
#define ROWS 128
#define MAX_LINE_LENGTH 100
#define MAX_NAME_LENGTH 20
#define PI 3.14159265358979323846
#define COMBINATION_YR 3
#define COMBINATION_YB 7
#define COMBINATION_RB 5
#define MAX_POINTS

extern char arr[ROWS][COLS][MAX_NAME_LENGTH];

typedef struct {
    double x_start;
    double y_start;
    double x_end;
    double y_end;
    char type;
    unsigned int val;
} LineCoordinates;

typedef struct {
    double x;
    double y;
    bool intersects;
    unsigned long flag;
    int num;
} IntersectionPoint;

typedef struct {
    IntersectionPoint centroid;
    IntersectionPoint *array_points;
    int num_points;
} Cluster;


void replaceBackslashes(char *str);
double distance(double , double , double , double ); 
void extractRYBi(const char *, char *);
LineCoordinates calculateLineCoordinates(char , int );
IntersectionPoint calculateIntersection(LineCoordinates line1, LineCoordinates line2);
IntersectionPoint calculateCentroid(IntersectionPoint *cluster, int size);
void splitLineColor(LineCoordinates *, int ,LineCoordinates *, LineCoordinates *, LineCoordinates *); 
void xLines(IntersectionPoint *, int ,LineCoordinates *, int , LineCoordinates *, int , LineCoordinates * , int , int * );
void fillCentroids(int, IntersectionPoint *, int , IntersectionPoint * , int  );
void fillLines(char * [],LineCoordinates *, int, int *, int *, int *);
int colorFlag(char, char);
int assign_number(char);
void printIntersectionPoint(IntersectionPoint *item, int numIP);
void printIntersectionPoint0(IntersectionPoint *item);
void init_array(IntersectionPoint *, int);
unsigned char fill_bits(unsigned char, int);
int selThreshold(int);

//Cluster *init_cluster(int);
//void free_cluster(Cluster *);

//double distance_p(IntersectionPoint, IntersectionPoint);
//int find_nearest_cluster(IntersectionPoint, Cluster *, int);
//void hierarchical_clustering(IntersectionPoint *, int max_clusters);
void clustering(int, IntersectionPoint *, int, int *);
int is_value_present(int *, int, int);
int* get_unique_values(int *, int , int *);
void clustersMerger(int,int, IntersectionPoint *,int, int *, int, int *);
void getCentroids( IntersectionPoint * , int , IntersectionPoint *, int);


#endif /* XYPICMIC_H */
