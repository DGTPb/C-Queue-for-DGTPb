#include <stdio.h>
#include <string.h>

#include "data.h"
#include "graph.h"
#include "map.h"
#include "route.h"
#include "ui.h"

/* 记忆成功加载时使用的路径，便于「重新加载」 */
static char g_spots_path[256] = "data/spots.txt";
static char g_paths_path[256] = "data/paths.txt";

/* 尝试若干候选路径加载数据，成功返回 1 */
static int try_load(Graph *g) {
    const char *spots_candidates[] = {
        "data/spots.txt", "../data/spots.txt", "../../data/spots.txt"
    };
    const char *paths_candidates[] = {
        "data/paths.txt", "../data/paths.txt", "../../data/paths.txt"
    };
    for (int i = 0; i < 3; i++) {
        if (data_load(g, spots_candidates[i], paths_candidates[i]) == 0) {
            strncpy(g_spots_path, spots_candidates[i], sizeof(g_spots_path) - 1);
            strncpy(g_paths_path, paths_candidates[i], sizeof(g_paths_path) - 1);
            return 1;
        }
    }
    return 0;
}

static int reload(Graph *g) {
    if (data_load(g, g_spots_path, g_paths_path) == 0) return 1;
    return try_load(g);
}

static void list_spots(const Graph *g) {
    printf("景点列表 (共 %d 个):\n", g->spot_count);
    for (int i = 0; i < g->spot_count; i++) {
        printf("  [%d] %s · %s · 坐标(%d,%d)\n",
               g->spots[i].id, g->spots[i].name, g->spots[i].category,
               g->spots[i].x, g->spots[i].y);
    }
}

/* 读取景点编号并返回下标，失败返回 -1 */
static int read_spot_index(const Graph *g, const char *prompt) {
    int id;
    if (!ui_read_int(prompt, &id)) {
        printf("输入无效。\n");
        return -1;
    }
    int idx = graph_index_of_id(g, id);
    if (idx < 0) {
        printf("景点编号 %d 不存在。\n", id);
        return -1;
    }
    return idx;
}

/* 菜单 2：景点详情 */
static void menu_spot_detail(const Graph *g) {
    list_spots(g);
    int idx = read_spot_index(g, "请输入要查看的景点编号: ");
    if (idx < 0) return;
    const Spot *s = &g->spots[idx];
    printf("\n--- 景点详情 ---\n");
    printf("  编号: %d\n", s->id);
    printf("  名称: %s\n", s->name);
    printf("  类别: %s\n", s->category);
    printf("  坐标: (%d, %d)\n", s->x, s->y);
    printf("  简介: %s\n", s->description);
}

/* 菜单 3：最短路径导航 */
static void menu_shortest_path(const Graph *g) {
    list_spots(g);
    int s = read_spot_index(g, "请输入起点编号: ");
    if (s < 0) return;
    int t = read_spot_index(g, "请输入终点编号: ");
    if (t < 0) return;
    if (s == t) {
        printf("起点与终点相同，无需导航。\n");
        return;
    }
    DijkstraResult r = graph_shortest_path(g, s, t);
    if (r.total_distance < 0) {
        printf("两景点之间不可达。\n");
        return;
    }
    int mins = (r.total_distance + WALK_SPEED_M_PER_MIN - 1) / WALK_SPEED_M_PER_MIN;
    printf("\n最短路径 (总距离 " COLOR_YELLOW "%d 米" COLOR_RESET
           "，约 %d 分钟):\n  ", r.total_distance, mins);
    for (int i = 0; i < r.count; i++) {
        printf(COLOR_GREEN "%s" COLOR_RESET, g->spots[r.indices[i]].name);
        if (i + 1 < r.count) printf(" -> ");
    }
    printf("\n");

    MapPath mp;
    mappath_from_indices(&mp, r.indices, r.count);
    printf("\n");
    map_render(g, &mp);
}

/* 菜单 4：推荐游览路线 */
static void menu_route(const Graph *g) {
    list_spots(g);
    int s = read_spot_index(g, "请输入起点编号: ");
    if (s < 0) return;

    printf("游览目标: 1=全部景点  2=自选景点\n");
    int choice;
    if (!ui_read_int("选择: ", &choice)) choice = 1;

    int targets[MAX_SPOTS];
    int tc = 0;
    if (choice == 2) {
        printf("输入要游览的景点编号（每行一个，输入 0 结束）:\n");
        while (tc < MAX_SPOTS) {
            int id;
            if (!ui_read_int("> ", &id)) {
                printf("输入无效，请重新输入。\n");
                continue;
            }
            if (id == 0) break;
            int idx = graph_index_of_id(g, id);
            if (idx < 0) {
                printf("编号 %d 不存在，已跳过。\n", id);
                continue;
            }
            if (idx == s) {
                printf("起点已作为出发点，跳过。\n");
                continue;
            }
            targets[tc++] = idx;
        }
    } else {
        for (int i = 0; i < g->spot_count; i++) {
            if (i != s) targets[tc++] = i;
        }
    }
    if (tc == 0) {
        printf("无可游览的景点。\n");
        return;
    }

    RouteResult r = route_recommend(g, s, targets, tc);
    if (!r.reachable) {
        printf(COLOR_YELLOW "警告: 部分目标不可达，已跳过。\n" COLOR_RESET);
    }
    printf("\n推荐游览路线 (总距离 " COLOR_YELLOW "%d 米" COLOR_RESET
           "，约 %d 分钟):\n  ", r.total_distance, r.total_minutes);
    for (int i = 0; i < r.count; i++) {
        if (i > 0) {
            DijkstraResult seg = graph_shortest_path(g, r.visit_order[i - 1],
                                                     r.visit_order[i]);
            printf(" --(" COLOR_CYAN "%d米" COLOR_RESET ")-->  ",
                   seg.total_distance);
        }
        printf(COLOR_GREEN "%s" COLOR_RESET, g->spots[r.visit_order[i]].name);
    }
    printf("\n");

    /* 展开为完整下标序列（含途经点）用于地图高亮 */
    int full[MAX_SPOTS];
    int fc = 0;
    for (int i = 0; i < r.count; i++) {
        if (i == 0) {
            full[fc++] = r.visit_order[i];
        } else {
            DijkstraResult seg = graph_shortest_path(g, r.visit_order[i - 1],
                                                     r.visit_order[i]);
            for (int j = 1; j < seg.count && fc < MAX_SPOTS; j++) {
                full[fc++] = seg.indices[j];
            }
        }
    }
    MapPath mp;
    mappath_from_indices(&mp, full, fc);
    printf("\n");
    map_render(g, &mp);
}

/* 菜单 5：重新加载数据 */
static void menu_reload(Graph *g) {
    if (reload(g)) {
        printf("已重新加载: %d 个景点, %d 条道路。\n",
               g->spot_count, g->path_count);
    } else {
        printf(COLOR_RED "重新加载失败，未找到数据文件。\n" COLOR_RESET);
    }
}

int main(void) {
    ui_init();

    static Graph g;
    if (!try_load(&g)) {
        printf(COLOR_RED "错误: 无法加载数据文件 data/spots.txt 与 data/paths.txt。\n"
               COLOR_RESET);
        printf("请确保在项目根目录或 build 目录下运行本程序。\n");
        return 1;
    }
    printf("数据加载成功: %d 个景点, %d 条道路。\n", g.spot_count, g.path_count);
    ui_pause();

    int running = 1;
    while (running) {
        ui_clear();
        ui_print_menu();
        int choice;
        if (!ui_read_int("请选择: ", &choice)) {
            if (feof(stdin)) { running = 0; continue; }  /* 输入结束：退出 */
            choice = -1;
        }
        printf("\n");
        switch (choice) {
            case 1: map_render(&g, NULL); break;
            case 2: menu_spot_detail(&g); break;
            case 3: menu_shortest_path(&g); break;
            case 4: menu_route(&g); break;
            case 5: menu_reload(&g); break;
            case 0: running = 0; continue;
            default:
                printf("无效选项，请重新选择。\n");
                break;
        }
        ui_pause();
    }

    printf("感谢使用云山湖景区地图系统，再见！\n");
    return 0;
}
