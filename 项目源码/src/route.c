#include "route.h"

RouteResult route_recommend(const Graph *g, int start_idx,
                            const int *targets, int target_count) {
    RouteResult r;
    r.count = 0;
    r.total_distance = 0;
    r.total_minutes = 0;
    r.reachable = 1;

    int n = g->spot_count;
    if (start_idx < 0 || start_idx >= n) return r;

    /* 构建待访问集合 */
    unsigned char want[MAX_SPOTS];
    for (int i = 0; i < n; i++) want[i] = 0;
    for (int k = 0; k < target_count; k++) {
        if (targets[k] >= 0 && targets[k] < n) want[targets[k]] = 1;
    }
    want[start_idx] = 0;   /* 起点不作为待访问目标 */

    int remaining = 0;
    for (int i = 0; i < n; i++) if (want[i]) remaining++;

    int current = start_idx;
    r.visit_order[r.count++] = current;

    static int dist[MAX_SPOTS];
    static int prev[MAX_SPOTS];

    while (remaining > 0) {
        graph_dijkstra(g, current, dist, prev);

        /* 在待访问集合中选 Dijkstra 距离最近者 */
        int best = -1, bestd = -1;
        for (int i = 0; i < n; i++) {
            if (!want[i] || dist[i] < 0) continue;
            if (best < 0 || dist[i] < bestd) {
                best = i;
                bestd = dist[i];
            }
        }
        if (best < 0) {              /* 剩余目标不可达 */
            r.reachable = 0;
            break;
        }
        r.total_distance += bestd;
        r.visit_order[r.count++] = best;
        want[best] = 0;
        current = best;
        remaining--;
    }

    r.total_minutes = (r.total_distance + WALK_SPEED_M_PER_MIN - 1) / WALK_SPEED_M_PER_MIN;
    return r;
}
