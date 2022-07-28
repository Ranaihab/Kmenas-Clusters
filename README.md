# Kmenas-Clusters
Implementation of the k means algorithm using the open MP 
We first read the data points fro the file points.txt. Then, we implement the algorithm using open mp functions only and the numebr of threads created is the same numbers of clusters. Each thread is responsible for calculating the distance between the centroid of a certain cluster and each data point. Then, the master thread will decide which cluster does each point belong to.  
