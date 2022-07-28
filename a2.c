#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define CLUSTER 5
#define numData 12

typedef struct {
    double x, y;
}Point;

void readFile(const char* name, Point points[]) {
    char str[15];
    char* split;
    int i = 0;
    FILE* file = fopen(name, "r");
    while (fgets(str, 15, file) && i < numData) {
        split = strtok(str, ",");
        points[i].x = atoi(split + 1);
        split = strtok(NULL, ")");
        points[i].y = atoi(split);
        i++;
    }
    fclose(file);
}

int main() {

    Point localMean;
    int id;
    int i, j;
    int minSoFar = 1e9;
    int minIdx = 0;
    Point localOldMean;
    int dataCount[CLUSTER];
    bool flag = true;
    Point clusterData[CLUSTER][numData];
    double dis[CLUSTER][numData];
    Point dataPoints[numData];
    Point clusterMeans[CLUSTER];
    readFile("/share/points.txt", dataPoints);

    printf("Generated means\n");
    for (i = 0; i < CLUSTER; i++) {
        clusterMeans[i].x = (rand() + 100) % 5;
        clusterMeans[i].y = (rand() + 100) % 5;
        printf("Cluster %d: \nmean x: %f \nmean y: %f \n", (i+1), clusterMeans[i].x,clusterMeans[i].y);

    }
    printf("\n");
    while (flag) {
        #pragma omp parallel default(shared) private(id, i) num_threads(CLUSTER)
        {
            id = omp_get_thread_num();
            for (i = 0; i < numData; i++) {
                {dis[id][i] = sqrt(((dataPoints[i].x - clusterMeans[id].x) * (dataPoints[i].x - clusterMeans[id].x)) +
                    ((dataPoints[i].y - clusterMeans[id].y) * (dataPoints[i].y - clusterMeans[id].y))); }
            }
        }
        for (i = 0; i < CLUSTER; i++) {
            dataCount[i] = 0;
        }
        for (i = 0; i < numData; i++) {
            minSoFar = 1e9;
            minIdx = 0;
            for (j = 0; j < CLUSTER; j++) {
                if (dis[j][i] < minSoFar) {
                    minSoFar = dis[j][i];
                    minIdx = j;
                }
            }
            clusterData[minIdx][dataCount[minIdx]] = dataPoints[i];
            dataCount[minIdx]++;
        }
        flag = false;
        #pragma omp parallel default(shared) private(id, i, localMean, localOldMean) num_threads(CLUSTER)
        {
            id = omp_get_thread_num();
            localOldMean = clusterMeans[id];
            localMean.x = 0;
            localMean.y = 0;
            for (i = 0; i < dataCount[id]; i++) {
                localMean.x += clusterData[id][i].x;
                localMean.y += clusterData[id][i].y;
            }
            if (dataCount[id] != 0) {
                localMean.x = localMean.x / dataCount[id];
                localMean.y = localMean.y / dataCount[id];        
            }
			if (fabs(localOldMean.x - localMean.x) >= 0.01 || fabs(localOldMean.y - localMean.y) >= 0.01) {
                #pragma omp critical
                flag = true;
            }
            clusterMeans[id] = localMean;
        }
    }
    for (i = 0; i < CLUSTER; i++) {
        printf("Cluster %d:\n", i + 1);
        for (j = 0; j < dataCount[i]; j++) {
            printf("(%d,%d)\n", (int)clusterData[i][j].x, (int)clusterData[i][j].y);
        }
    }
}

