#ifndef UI_H
#define UI_H

/* ANSI 颜色（Windows 下由 ui_init 启用 VT 处理） */
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RESET   "\033[0m"

/* 终端初始化：Windows 下启用 ANSI 颜色与 UTF-8 输出 */
void ui_init(void);

/* 清屏 */
void ui_clear(void);

/* 等待回车 */
void ui_pause(void);

/* 打印带框标题 */
void ui_print_title(const char *title);

/* 打印主菜单 */
void ui_print_menu(void);

/* 读取一行文本（去除换行），返回是否成功 */
void ui_read_line(const char *prompt, char *buf, int size);

/* 读取一个整数，成功返回 1 并写入 *out，失败返回 0 */
int ui_read_int(const char *prompt, int *out);

#endif /* UI_H */
