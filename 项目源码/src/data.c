#include "data.h"

#include <ctype.h>
#include <string.h>

/* 去除行尾空白（含 \r \n） */
static void trim_trailing(char *s) {
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }
}

/* 跳过前导空白，返回首个非空白字符指针 */
static char *skip_leading(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

static int load_spots(Graph *g, const char *file) {
    FILE *fp = fopen(file, "r");
    if (!fp) return -1;

    char line[512];
    g->spot_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *p = skip_leading(line);
        if (*p == '\0' || *p == '#') continue;
        if (g->spot_count >= MAX_SPOTS) break;

        Spot *s = &g->spots[g->spot_count];
        char desc[256] = "";
        /* 描述为行尾剩余部分，允许包含空格 */
        int n = sscanf(p, "%d %63s %31s %d %d %255[^\n]",
                       &s->id, s->name, s->category, &s->x, &s->y, desc);
        if (n < 5) continue;            /* 字段不足，跳过 */
        if (n < 6) desc[0] = '\0';

        char *d = skip_leading(desc);
        trim_trailing(d);
        strncpy(s->description, d, sizeof(s->description) - 1);
        s->description[sizeof(s->description) - 1] = '\0';

        g->spot_count++;
    }
    fclose(fp);
    return 0;
}

static int load_paths(Graph *g, const char *file) {
    FILE *fp = fopen(file, "r");
    if (!fp) return -1;

    char line[512];
    g->path_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *p = skip_leading(line);
        if (*p == '\0' || *p == '#') continue;
        if (g->path_count >= MAX_PATHS) break;

        Path *e = &g->paths[g->path_count];
        int n = sscanf(p, "%d %d %d %15s",
                       &e->from, &e->to, &e->distance, e->type);
        if (n < 3) continue;
        if (n < 4) strcpy(e->type, "步道");

        g->path_count++;
    }
    fclose(fp);
    return 0;
}

int data_load(Graph *g, const char *spots_file, const char *paths_file) {
    memset(g, 0, sizeof(*g));

    /* 邻接矩阵初始化：-1 表示不连通，对角线 0 */
    for (int i = 0; i < MAX_SPOTS; i++) {
        for (int j = 0; j < MAX_SPOTS; j++) {
            g->adj[i][j] = (i == j) ? 0 : -1;
        }
    }

    if (load_spots(g, spots_file) != 0) return -1;
    if (load_paths(g, paths_file) != 0) return -1;

    /* 构建无向图邻接矩阵，重边取较短者 */
    for (int k = 0; k < g->path_count; k++) {
        int ia = graph_index_of_id(g, g->paths[k].from);
        int ib = graph_index_of_id(g, g->paths[k].to);
        if (ia < 0 || ib < 0) continue;
        int d = g->paths[k].distance;
        if (g->adj[ia][ib] < 0 || d < g->adj[ia][ib]) {
            g->adj[ia][ib] = d;
            g->adj[ib][ia] = d;
        }
    }
    return 0;
}

int graph_index_of_id(const Graph *g, int id) {
    for (int i = 0; i < g->spot_count; i++) {
        if (g->spots[i].id == id) return i;
    }
    return -1;
}

const Spot *graph_find_spot_by_id(const Graph *g, int id) {
    int idx = graph_index_of_id(g, id);
    return (idx < 0) ? NULL : &g->spots[idx];
}
