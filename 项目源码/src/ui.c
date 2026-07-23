#include "ui.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

void ui_init(void) {
#ifdef _WIN32
    /* 启用 UTF-8 输出，避免中文乱码 */
    SetConsoleOutputCP(CP_UTF8);
    /* 启用虚拟终端处理，使 ANSI 颜色码生效 */
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(h, &mode)) {
            SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
#endif
}

void ui_clear(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

void ui_pause(void) {
    printf("\n按回车键继续...");
    fflush(stdout);
    char buf[16];
    if (!fgets(buf, sizeof(buf), stdin)) {
        /* 输入已结束，忽略 */
    }
}

void ui_print_title(const char *title) {
    printf(COLOR_BOLD COLOR_CYAN);
    printf("========== %s ==========\n", title);
    printf(COLOR_RESET);
}

void ui_print_menu(void) {
    ui_print_title("云山湖景区地图系统");
    printf("  1. 查看景区地图\n");
    printf("  2. 景点列表与详情\n");
    printf("  3. 最短路径导航 (Dijkstra)\n");
    printf("  4. 推荐游览路线\n");
    printf("  5. 重新加载数据\n");
    printf("  0. 退出\n");
    printf("----------------------------------------\n");
}

void ui_read_line(const char *prompt, char *buf, int size) {
    if (prompt && *prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }
    if (!fgets(buf, size, stdin)) {
        buf[0] = '\0';
        return;
    }
    /* 去除行尾换行 */
    int len = (int)strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[--len] = '\0';
    }
}

int ui_read_int(const char *prompt, int *out) {
    char buf[64];
    ui_read_line(prompt, buf, sizeof(buf));

    char *p = buf;
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '\0') return 0;

    int sign = 1;
    if (*p == '-') { sign = -1; p++; }
    else if (*p == '+') { p++; }

    if (!(*p >= '0' && *p <= '9')) return 0;

    long v = 0;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (*p - '0');
        p++;
    }
    /* 后续应全为空白 */
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p != '\0') return 0;

    *out = (int)(sign * v);
    return 1;
}
