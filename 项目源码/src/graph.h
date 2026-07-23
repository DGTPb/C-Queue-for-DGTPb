#ifndef GRAPH_H
#define GRAPH_H

#include "data.h"

/* 单条最短路径结果：下标序列 + 总距离 */
typedef struct {
    int indices[MAX_SPOTS]; /* src -> ... -> dst 的下标序列 */
    int count;
    int total_distance;     /* 米；不可达为 -1 */
} DijkstraResult;

/* 计算从 src_idx 出发到所有点的最短路径。
   dist[i]=最短距离(-1 不可达)，prev[i]=前驱下标(-1 无前驱)。 */
void graph_dijkstra(const Graph *g, int src_idx, int *dist, int *prev);

/* 计算 src_idx -> dst_idx 的最短路径 */
DijkstraResult graph_shortest_path(const Graph *g, int src_idx, int dst_idx);

#endif /* GRAPH_H */
