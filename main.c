#include <ctype.h>
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_LEN(x) (int)(sizeof(x) / sizeof((x)[0]))
#define MAX_ITEMS 64

typedef struct {
    const char *title;
    const char *items[MAX_ITEMS];
    int item_count;
    int selected;
    int scroll;
} Pane;

typedef struct {
    int y;
    int x;
    int h;
    int w;
} Rect;

typedef enum {
    FOCUS_LEFT_TREE = 0,
    FOCUS_RIGHT_LIST,
    FOCUS_BOTTOM_TASKS,
    FOCUS_COUNT
} FocusArea;

static const char *menu_items[] = {"File", "Options", "View", "Tree", "Help"};
static const char *left_tree[] = {
    "C:\\",
    "  DOS",
    "  GAMES",
    "  PROJECTS",
    "  TOOLS",
    "  WINDOWS",
    "D:\\",
    "  BACKUP",
    "  MUSIC",
    "  DRIVERS",
    "E:\\",
    "  OLD_APPS"
};

static const char *right_files[] = {
    "AUTOEXEC.BAT      1,024   06-14-94",
    "CONFIG.SYS          832   06-14-94",
    "COMMAND.COM      54,645  05-31-95",
    "DOSKEY.COM        4,139  08-24-94",
    "EDIT.COM         69,760  11-18-95",
    "FDISK.EXE        63,084  09-09-95",
    "FORMAT.COM       22,912  10-06-95",
    "HIMEM.SYS        11,520  07-11-94",
    "MEM.EXE          38,675  08-24-94",
    "MSCDEX.EXE       25,120  04-27-95",
    "QBASIC.EXE      194,309  05-31-95",
    "SCANDISK.EXE    134,738  10-06-95",
    "SMARTDRV.EXE     45,964  08-24-94",
    "TREE.COM           6,912  05-31-95",
    "XCOPY.EXE        11,632  08-24-94"
};

static const char *task_items[] = {
    "Disk Copy", "Backup", "Virus Scan", "Quick Format", "Undelete",
    "Defrag", "Setup", "Diagnostics", "System Info", "Command Prompt"
};

static void clamp_scroll(Pane *pane, int visible_rows) {
    if (pane->selected < 0) {
        pane->selected = 0;
    }
    if (pane->selected >= pane->item_count) {
        pane->selected = pane->item_count - 1;
    }
    if (pane->selected < pane->scroll) {
        pane->scroll = pane->selected;
    }
    if (pane->selected >= pane->scroll + visible_rows) {
        pane->scroll = pane->selected - visible_rows + 1;
    }
    if (pane->scroll < 0) {
        pane->scroll = 0;
    }
}

static void draw_bevel(int y, int x, int h, int w) {
    mvhline(y, x, ACS_HLINE, w);
    mvvline(y, x, ACS_VLINE, h);
    mvhline(y + h - 1, x, ACS_HLINE, w);
    mvvline(y, x + w - 1, ACS_VLINE, h);
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w - 1, ACS_URCORNER);
    mvaddch(y + h - 1, x, ACS_LLCORNER);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);
}

static void draw_header(const char *title, int row, int width, short color_pair) {
    attron(COLOR_PAIR(color_pair));
    mvhline(row, 0, ' ', width);
    mvprintw(row, 1, "%s", title);
    attroff(COLOR_PAIR(color_pair));
}

static void draw_menu_bar(int width, int active_index) {
    draw_header("MS-DOS Shell", 0, width, 2);
    attron(COLOR_PAIR(3));
    mvhline(1, 0, ' ', width);
    int col = 1;
    for (int i = 0; i < ARRAY_LEN(menu_items); i++) {
        if (i == active_index) {
            attron(A_REVERSE);
        }
        mvprintw(1, col, "%s", menu_items[i]);
        if (i == active_index) {
            attroff(A_REVERSE);
        }
        col += (int)strlen(menu_items[i]) + 2;
    }
    attroff(COLOR_PAIR(3));
}

static void draw_toolbar(int y, int width) {
    attron(COLOR_PAIR(1));
    mvhline(y, 0, ' ', width);
    mvprintw(y, 1, "[A] [B] [C] [Z]  Drives");
    attroff(COLOR_PAIR(1));
}

static void draw_status_bar(int y, int width, const char *status_text) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char clock_text[16];
    strftime(clock_text, sizeof(clock_text), "%H:%M:%S", tm_info);

    attron(COLOR_PAIR(4));
    mvhline(y, 0, ' ', width);
    mvprintw(y, 1, "F1=Help  Tab=Next Window  Enter=Open  Esc=Exit");
    mvprintw(y, width - (int)strlen(clock_text) - 2, "%s", clock_text);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(5));
    mvhline(y - 1, 0, ' ', width);
    mvprintw(y - 1, 1, "%s", status_text);
    attroff(COLOR_PAIR(5));
}

static void draw_pane(Rect r, const Pane *pane, bool focused) {
    attron(COLOR_PAIR(1));
    draw_bevel(r.y, r.x, r.h, r.w);
    mvhline(r.y + 1, r.x + 1, ' ', r.w - 2);
    mvprintw(r.y + 1, r.x + 2, "%s", pane->title);
    attroff(COLOR_PAIR(1));

    int list_y = r.y + 2;
    int list_h = r.h - 3;
    for (int i = 0; i < list_h; i++) {
        int idx = pane->scroll + i;
        mvhline(list_y + i, r.x + 1, ' ', r.w - 2);
        if (idx >= pane->item_count) {
            continue;
        }
        if (idx == pane->selected) {
            attron(COLOR_PAIR(focused ? 6 : 5));
            mvprintw(list_y + i, r.x + 2, "%.*s", r.w - 4, pane->items[idx]);
            attroff(COLOR_PAIR(focused ? 6 : 5));
        } else {
            mvprintw(list_y + i, r.x + 2, "%.*s", r.w - 4, pane->items[idx]);
        }
    }
}

static void pane_move(Pane *pane, int delta, int visible_rows) {
    pane->selected += delta;
    if (pane->selected < 0) {
        pane->selected = 0;
    }
    if (pane->selected >= pane->item_count) {
        pane->selected = pane->item_count - 1;
    }
    clamp_scroll(pane, visible_rows);
}

int main(void) {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(200);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE, COLOR_BLUE);
        init_pair(3, COLOR_BLACK, COLOR_CYAN);
        init_pair(4, COLOR_BLACK, COLOR_CYAN);
        init_pair(5, COLOR_BLACK, COLOR_WHITE);
        init_pair(6, COLOR_WHITE, COLOR_BLUE);
    }

    Pane tree = {.title = "Directory Tree", .item_count = ARRAY_LEN(left_tree)};
    Pane files = {.title = "C:\\*.*", .item_count = ARRAY_LEN(right_files)};
    Pane tasks = {.title = "Disk Utilities", .item_count = ARRAY_LEN(task_items)};

    for (int i = 0; i < tree.item_count; i++) tree.items[i] = left_tree[i];
    for (int i = 0; i < files.item_count; i++) files.items[i] = right_files[i];
    for (int i = 0; i < tasks.item_count; i++) tasks.items[i] = task_items[i];

    FocusArea focus = FOCUS_LEFT_TREE;
    int menu_focus = 0;
    char status[256] = "Ready.";

    bool running = true;
    while (running) {
        int h, w;
        getmaxyx(stdscr, h, w);

        if (h < 20 || w < 70) {
            erase();
            mvprintw(0, 0, "Terminal too small. Minimum 70x20.");
            mvprintw(1, 0, "Current: %dx%d", w, h);
            refresh();
            int ch = getch();
            if (ch == 27 || ch == 'q' || ch == 'Q') {
                running = false;
            }
            continue;
        }

        Rect left = {.y = 3, .x = 0, .h = h - 8, .w = w / 2};
        Rect right = {.y = 3, .x = w / 2, .h = h - 8, .w = w - w / 2};
        Rect bottom = {.y = h - 5, .x = 0, .h = 3, .w = w};

        clamp_scroll(&tree, left.h - 3);
        clamp_scroll(&files, right.h - 3);
        clamp_scroll(&tasks, bottom.h - 3 < 1 ? 1 : bottom.h - 3 + 1);

        erase();
        draw_menu_bar(w, menu_focus);
        draw_toolbar(2, w);
        draw_pane(left, &tree, focus == FOCUS_LEFT_TREE);
        draw_pane(right, &files, focus == FOCUS_RIGHT_LIST);
        draw_pane(bottom, &tasks, focus == FOCUS_BOTTOM_TASKS);
        draw_status_bar(h - 1, w, status);
        refresh();

        int ch = getch();
        if (ch == ERR) {
            continue;
        }

        switch (ch) {
            case 27:
            case 'q':
            case 'Q':
                running = false;
                break;
            case '\t':
                focus = (FocusArea)((focus + 1) % FOCUS_COUNT);
                snprintf(status, sizeof(status), "Focus: %s",
                         focus == FOCUS_LEFT_TREE ? "Directory Tree"
                         : focus == FOCUS_RIGHT_LIST ? "File List"
                         : "Disk Utilities");
                break;
            case KEY_F(1):
                snprintf(status, sizeof(status), "Help: Tab cycles windows. Use arrows + Enter.");
                break;
            case KEY_LEFT:
                if (focus == FOCUS_LEFT_TREE || focus == FOCUS_RIGHT_LIST) {
                    menu_focus = (menu_focus - 1 + ARRAY_LEN(menu_items)) % ARRAY_LEN(menu_items);
                }
                if (focus == FOCUS_RIGHT_LIST) focus = FOCUS_LEFT_TREE;
                break;
            case KEY_RIGHT:
                if (focus == FOCUS_LEFT_TREE || focus == FOCUS_RIGHT_LIST) {
                    menu_focus = (menu_focus + 1) % ARRAY_LEN(menu_items);
                }
                if (focus == FOCUS_LEFT_TREE) focus = FOCUS_RIGHT_LIST;
                break;
            case KEY_UP:
                if (focus == FOCUS_LEFT_TREE) pane_move(&tree, -1, left.h - 3);
                if (focus == FOCUS_RIGHT_LIST) pane_move(&files, -1, right.h - 3);
                if (focus == FOCUS_BOTTOM_TASKS) pane_move(&tasks, -1, 1);
                break;
            case KEY_DOWN:
                if (focus == FOCUS_LEFT_TREE) pane_move(&tree, 1, left.h - 3);
                if (focus == FOCUS_RIGHT_LIST) pane_move(&files, 1, right.h - 3);
                if (focus == FOCUS_BOTTOM_TASKS) pane_move(&tasks, 1, 1);
                break;
            case KEY_NPAGE:
                if (focus == FOCUS_LEFT_TREE) pane_move(&tree, left.h - 3, left.h - 3);
                if (focus == FOCUS_RIGHT_LIST) pane_move(&files, right.h - 3, right.h - 3);
                break;
            case KEY_PPAGE:
                if (focus == FOCUS_LEFT_TREE) pane_move(&tree, -(left.h - 3), left.h - 3);
                if (focus == FOCUS_RIGHT_LIST) pane_move(&files, -(right.h - 3), right.h - 3);
                break;
            case '\n':
            case KEY_ENTER: {
                const char *label = NULL;
                if (focus == FOCUS_LEFT_TREE) label = tree.items[tree.selected];
                if (focus == FOCUS_RIGHT_LIST) label = files.items[files.selected];
                if (focus == FOCUS_BOTTOM_TASKS) label = tasks.items[tasks.selected];
                snprintf(status, sizeof(status), "Selected: %s", label ? label : "(none)");
                break;
            }
            default:
                if (isprint(ch)) {
                    snprintf(status, sizeof(status), "Key '%c' pressed. (F1 for help)", ch);
                } else {
                    snprintf(status, sizeof(status), "Key code %d pressed.", ch);
                }
                break;
        }
    }

    endwin();
    return 0;
}
