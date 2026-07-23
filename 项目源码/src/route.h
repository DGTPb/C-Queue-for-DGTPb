#ifndef ROUTE_H
#define ROUTE_H

#include "data.h"
#include "graph.h"

#define WALK_SPEED_M_PER_MIN 70   /* 步行速度：70 米/分钟 */

/* 推荐游览路线结果 */
typedef struct {
    int visit_order[MAX_SPOTS]; /* 游览景点下标序列（起点为首个） */
    int count;
    int total_distance;         /* 米 */
    int total_minutes;          /* 预估步行时长（分钟） */
    int reachable;              /* 是否所有目标均可达 */
} RouteResult;

/* 最近邻启发式生成游览路线。
   start_idx 为起点下标；targets[] 为期望游览的景点下标集合。
   起点本身不计入待访问集合。 */
RouteResult route_recommend(const Graph *g, int start_idx,
                            const int *targets, int target_count);

#endif /* ROUTE_H */
