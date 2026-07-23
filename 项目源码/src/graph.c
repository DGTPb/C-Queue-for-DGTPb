#include "graph.h"

void graph_dijkstra(const Graph *g, int src_idx, int *dist, int *prev) {
    int n = g->spot_count;
    unsigned char visited[MAX_SPOTS];

    for (int i = 0; i < n; i++) {
        dist[i] = -1;
        prev[i] = -1;
        visited[i] = 0;
    }
    if (src_idx < 0 || src_idx >= n) return;

    dist[src_idx] = 0;
    for (int iter = 0; iter < n; iter++) {
        /* 在未访问点中选 dist 最小者 */
        int u = -1, best = -1;
        for (int i = 0; i < n; i++) {
            if (visited[i] || dist[i] < 0) continue;
            if (u < 0 || dist[i] < best) {
                u = i;
                best = dist[i];
            }
        }
        if (u < 0) break;          /* 其余点不可达 */
        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (visited[v]) continue;
            int w = g->adj[u][v];
            if (w <= 0) continue;   /* 0=自身，-1=不连通 */
            int nd = dist[u] + w;
            if (dist[v] < 0 || nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
            }
        }
    }
}

DijkstraResult graph_shortest_path(const Graph *g, int src_idx, int dst_idx) {
    DijkstraResult r;
    r.count = 0;
    r.total_distance = -1;

    int n = g->spot_count;
    if (src_idx < 0 || src_idx >= n) return r;
    if (dst_idx  < 0 || dst_idx  >= n) return r;

    static int dist[MAX_SPOTS];
    static int prev[MAX_SPOTS];
    graph_dijkstra(g, src_idx, dist, prev);

    if (dist[dst_idx] < 0) return r;   /* 不可达 */

    /* 回溯前驱得到逆序路径 */
    int stack[MAX_SPOTS];
    int top = 0;
    int cur = dst_idx;
    while (cur != -1 && top < MAX_SPOTS) {
        stack[top++] = cur;
        if (cur == src_idx) break;
        cur = prev[cur];
    }

    /* 反转为正序 */
    r.count = top;
    for (int i = 0; i < top; i++) {
        r.indices[i] = stack[top - 1 - i];
    }
    r.total_distance = dist[dst_idx];
    return r;
}
