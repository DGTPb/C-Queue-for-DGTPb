#include "map.h"

#include <stdio.h>
#include <string.h>

#include "ui.h"

/* 景点标记：1-9 用数字，10-35 用字母 a-z */
static char spot_marker(int id) {
    if (id >= 1 && id <= 9)   return (char)('0' + id);
    if (id >= 10 && id <= 35) return (char)('a' + (id - 10));
    return '?';
}

/* Bresenham 画线：仅覆盖空格，保留已有内容 */
static void draw_line(char grid[MAP_H][MAP_W],
                      int x0, int y0, int x1, int y1, char ch) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        if (x0 >= 0 && x0 < MAP_W && y0 >= 0 && y0 < MAP_H) {
            if (grid[y0][x0] == ' ') grid[y0][x0] = ch;
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

/* Bresenham 高亮：沿连线把道路格改为 '*' 并置 hl 标志，跳过景点格与边框 */
static void highlight_line(char grid[MAP_H][MAP_W],
                           unsigned char hl[MAP_H][MAP_W],
                           const unsigned char spot[MAP_H][MAP_W],
                           int x0, int y0, int x1, int y1) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        if (x0 >= 0 && x0 < MAP_W && y0 >= 0 && y0 < MAP_H) {
            if (!spot[y0][x0] && grid[y0][x0] != '#') {
                grid[y0][x0] = '*';
                hl[y0][x0] = 1;
            }
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

void map_render(const Graph *g, const MapPath *highlight) {
    static char grid[MAP_H][MAP_W];
    static unsigned char spot[MAP_H][MAP_W];
    static unsigned char hl[MAP_H][MAP_W];

    memset(grid, ' ', sizeof(grid));
    memset(spot, 0, sizeof(spot));
    memset(hl, 0, sizeof(hl));

    /* 边框 */
    for (int x = 0; x < MAP_W; x++) {
        grid[0][x] = '#';
        grid[MAP_H - 1][x] = '#';
    }
    for (int y = 0; y < MAP_H; y++) {
        grid[y][0] = '#';
        grid[y][MAP_W - 1] = '#';
    }

    /* 道路 */
    for (int k = 0; k < g->path_count; k++) {
        int ia = graph_index_of_id(g, g->paths[k].from);
        int ib = graph_index_of_id(g, g->paths[k].to);
        if (ia < 0 || ib < 0) continue;
        char ch = (strcmp(g->paths[k].type, "公路") == 0) ? ':' : '.';
        draw_line(grid,
                  g->spots[ia].x, g->spots[ia].y,
                  g->spots[ib].x, g->spots[ib].y, ch);
    }

    /* 景点标记（覆盖道路交汇点） */
    for (int i = 0; i < g->spot_count; i++) {
        int x = g->spots[i].x, y = g->spots[i].y;
        if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) {
            grid[y][x] = spot_marker(g->spots[i].id);
            spot[y][x] = 1;
        }
    }

    /* 高亮路径 */
    if (highlight) {
        for (int s = 0; s + 1 < highlight->count; s++) {
            int ia = highlight->indices[s];
            int ib = highlight->indices[s + 1];
            if (ia < 0 || ia >= g->spot_count) continue;
            if (ib < 0 || ib >= g->spot_count) continue;
            highlight_line(grid, hl, spot,
                           g->spots[ia].x, g->spots[ia].y,
                           g->spots[ib].x, g->spots[ib].y);
        }
    }

    /* 输出地图 */
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (hl[y][x]) {
                printf(COLOR_RED "*" COLOR_RESET);
            } else {
                putchar(grid[y][x]);
            }
        }
        putchar('\n');
    }

    /* 图例 */
    printf("\n图例: '.' 步道  ':' 公路  '*' 高亮路径  '#' 边框\n");
    printf("景点: ");
    for (int i = 0; i < g->spot_count; i++) {
        printf("%c:%s", spot_marker(g->spots[i].id), g->spots[i].name);
        if (i + 1 < g->spot_count) printf("  ");
    }
    printf("\n");
}

void mappath_from_indices(MapPath *mp, const int *idx, int n) {
    if (n > MAX_SPOTS) n = MAX_SPOTS;
    for (int i = 0; i < n; i++) mp->indices[i] = idx[i];
    mp->count = n;
}
