#ifndef DATA_H
#define DATA_H

#include <stdio.h>

#define MAX_SPOTS  64
#define MAX_PATHS  256
#define MAP_W      60   /* 地图宽（列） */
#define MAP_H      18   /* 地图高（行） */

/* 景点 */
typedef struct {
    int  id;
    char name[64];
    char category[32];
    char description[256];
    int  x, y;          /* 地图坐标（列, 行） */
} Spot;

/* 道路（无向边） */
typedef struct {
    int  from;
    int  to;
    int  distance;      /* 米 */
    char type[16];      /* 步道 / 公路 */
} Path;

/* 图：景点 + 道路 + 邻接矩阵 */
typedef struct {
    Spot spots[MAX_SPOTS];
    int  spot_count;
    Path paths[MAX_PATHS];
    int  path_count;
    int  adj[MAX_SPOTS][MAX_SPOTS]; /* 按景点下标索引，-1=不连通，0=自身，>0=距离 */
} Graph;

/* 从文件加载数据并构建邻接矩阵，成功返回 0，失败返回 -1 */
int data_load(Graph *g, const char *spots_file, const char *paths_file);

/* 景点 id -> spots 数组下标，找不到返回 -1 */
int graph_index_of_id(const Graph *g, int id);

/* 按 id 查找景点，找不到返回 NULL */
const Spot *graph_find_spot_by_id(const Graph *g, int id);

#endif /* DATA_H */
