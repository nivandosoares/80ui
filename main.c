#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define ARRAY_LEN(x) (int)(sizeof(x) / sizeof((x)[0]))
#define EDITOR_LINES 128
#define OUTPUT_LINES 64
#define MAX_LINE_LEN 160

typedef enum {
    FOCUS_MENU = 0,
    FOCUS_EDITOR,
    FOCUS_PROJECT,
    FOCUS_OUTPUT,
    FOCUS_COUNT
} Focus;

typedef struct {
    char lines[EDITOR_LINES][MAX_LINE_LEN];
    int line_count;
    int cursor_x;
    int cursor_y;
    int scroll_y;
} Editor;

typedef struct {
    char lines[OUTPUT_LINES][MAX_LINE_LEN];
    int count;
    int scroll;
} OutputPanel;

static const char *menus[] = {
    "File", "Edit", "Search", "Run", "Compile", "Debug", "Project", "Options", "Window", "Help"
};

static const char *project_files[] = {
    "MAIN.C", "GRAPH.C", "MATH.C", "UTIL.C", "UI.C", "UI.H", "MAKEFILE", "README.TXT"
};

static const char *compile_msgs[] = {
    "Compiling MAIN.C",
    "Compiling UI.C",
    "Linking PROJECT.EXE",
    "Turbo Link  Version 7.1 Copyright (c)",
    "Success: 0 Errors, 0 Warnings"
};

static void editor_init(Editor *ed) {
    memset(ed, 0, sizeof(*ed));
    const char *seed[] = {
        "#include <stdio.h>",
        "",
        "int soma(int a, int b) {",
        "    return a + b;",
        "}",
        "",
        "int main(void) {",
        "    int total = soma(40, 2);",
        "    printf(\"Turbo C++ feel! total=%d\\n\", total);",
        "    return 0;",
        "}"
    };

    ed->line_count = ARRAY_LEN(seed);
    for (int i = 0; i < ed->line_count; i++) {
        snprintf(ed->lines[i], MAX_LINE_LEN, "%s", seed[i]);
    }
}

static void output_init(OutputPanel *out) {
    memset(out, 0, sizeof(*out));
    snprintf(out->lines[out->count++], MAX_LINE_LEN, "Turbo C++ IDE ready.");
    snprintf(out->lines[out->count++], MAX_LINE_LEN, "Press F9 to Compile or Ctrl+F9 to Run.");
}

static void output_push(OutputPanel *out, const char *msg) {
    if (out->count < OUTPUT_LINES) {
        snprintf(out->lines[out->count++], MAX_LINE_LEN, "%s", msg);
        return;
    }
    for (int i = 1; i < OUTPUT_LINES; i++) {
        memmove(out->lines[i - 1], out->lines[i], MAX_LINE_LEN);
    }
    snprintf(out->lines[OUTPUT_LINES - 1], MAX_LINE_LEN, "%s", msg);
}

static void draw_top_title(int w) {
    attron(COLOR_PAIR(1));
    mvhline(0, 0, ' ', w);
    mvprintw(0, 2, "Turbo C++ 3.0   [Untitled Project]");
    attroff(COLOR_PAIR(1));
}

static void draw_menu_bar(int w, int active_menu, bool menu_open) {
    attron(COLOR_PAIR(2));
    mvhline(1, 0, ' ', w);

    int col = 1;
    for (int i = 0; i < ARRAY_LEN(menus); i++) {
        if (i == active_menu && menu_open) {
            attron(COLOR_PAIR(5));
            mvprintw(1, col, "%s", menus[i]);
            attroff(COLOR_PAIR(5));
        } else {
            mvprintw(1, col, "%s", menus[i]);
        }
        col += (int)strlen(menus[i]) + 2;
    }

    attroff(COLOR_PAIR(2));
}

static void draw_frame(int y, int x, int h, int w, short color, const char *title, bool focused) {
    attron(COLOR_PAIR(color));
    mvhline(y, x, ACS_HLINE, w);
    mvhline(y + h - 1, x, ACS_HLINE, w);
    mvvline(y, x, ACS_VLINE, h);
    mvvline(y, x + w - 1, ACS_VLINE, h);
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w - 1, ACS_URCORNER);
    mvaddch(y + h - 1, x, ACS_LLCORNER);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);
    if (title) {
        if (focused) attron(A_BOLD);
        mvprintw(y, x + 2, " %s ", title);
        if (focused) attroff(A_BOLD);
    }
    attroff(COLOR_PAIR(color));
}

static void draw_editor(const Editor *ed, int y, int x, int h, int w, bool focused) {
    draw_frame(y, x, h, w, 3, "Editor", focused);
    int body_h = h - 2;
    int body_w = w - 2;

    for (int row = 0; row < body_h; row++) {
        int line_idx = ed->scroll_y + row;
        mvhline(y + 1 + row, x + 1, ' ', body_w);

        attron(COLOR_PAIR(6));
        mvprintw(y + 1 + row, x + 1, "%4d ", line_idx + 1);
        attroff(COLOR_PAIR(6));

        if (line_idx < ed->line_count) {
            attron(COLOR_PAIR(3));
            mvprintw(y + 1 + row, x + 7, "%.*s", body_w - 6, ed->lines[line_idx]);
            attroff(COLOR_PAIR(3));
        }
    }

    if (focused) {
        int cy = ed->cursor_y - ed->scroll_y;
        int cx = ed->cursor_x;
        if (cy >= 0 && cy < body_h && cx >= 0 && cx < body_w - 6) {
            move(y + 1 + cy, x + 7 + cx);
            curs_set(1);
        } else {
            curs_set(0);
        }
    }
}

static void draw_project(int y, int x, int h, int w, int selected, bool focused) {
    draw_frame(y, x, h, w, 3, "Project", focused);
    int body_h = h - 2;

    for (int i = 0; i < body_h; i++) {
        mvhline(y + 1 + i, x + 1, ' ', w - 2);
        if (i >= ARRAY_LEN(project_files)) continue;

        if (focused && i == selected) {
            attron(COLOR_PAIR(5));
            mvprintw(y + 1 + i, x + 2, "%.*s", w - 4, project_files[i]);
            attroff(COLOR_PAIR(5));
        } else {
            attron(COLOR_PAIR(3));
            mvprintw(y + 1 + i, x + 2, "%.*s", w - 4, project_files[i]);
            attroff(COLOR_PAIR(3));
        }
    }
}

static void draw_output(const OutputPanel *out, int y, int x, int h, int w, bool focused) {
    draw_frame(y, x, h, w, 3, "Message", focused);
    int body_h = h - 2;

    for (int i = 0; i < body_h; i++) {
        int idx = out->scroll + i;
        mvhline(y + 1 + i, x + 1, ' ', w - 2);
        if (idx >= out->count) continue;
        attron(COLOR_PAIR(3));
        mvprintw(y + 1 + i, x + 2, "%.*s", w - 4, out->lines[idx]);
        attroff(COLOR_PAIR(3));
    }
}

static void draw_status(int h, int w, Focus focus, const char *status) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char clock_text[16];
    strftime(clock_text, sizeof(clock_text), "%H:%M:%S", tm_info);

    attron(COLOR_PAIR(2));
    mvhline(h - 2, 0, ' ', w);
    mvprintw(h - 2, 1, "F1 Help  F6 Next Pane  F9 Compile  Ctrl+F9 Run  F10 Menu  Esc Quit");
    mvprintw(h - 2, w - (int)strlen(clock_text) - 2, "%s", clock_text);
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(1));
    mvhline(h - 1, 0, ' ', w);
    mvprintw(h - 1, 1, "[%s] %s",
             focus == FOCUS_MENU   ? "MENU"
             : focus == FOCUS_EDITOR ? "EDITOR"
             : focus == FOCUS_PROJECT ? "PROJECT"
                                     : "OUTPUT",
             status);
    attroff(COLOR_PAIR(1));
}

static void maybe_scroll_editor(Editor *ed, int body_h) {
    if (ed->cursor_y < 0) ed->cursor_y = 0;
    if (ed->cursor_y >= ed->line_count) ed->cursor_y = ed->line_count - 1;
    if (ed->cursor_y < 0) ed->cursor_y = 0;

    int line_len = (int)strlen(ed->lines[ed->cursor_y]);
    if (ed->cursor_x < 0) ed->cursor_x = 0;
    if (ed->cursor_x > line_len) ed->cursor_x = line_len;

    if (ed->cursor_y < ed->scroll_y) ed->scroll_y = ed->cursor_y;
    if (ed->cursor_y >= ed->scroll_y + body_h) ed->scroll_y = ed->cursor_y - body_h + 1;
    if (ed->scroll_y < 0) ed->scroll_y = 0;
}

int main(void) {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(200);

    start_color();
    use_default_colors();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // title/status
    init_pair(2, COLOR_BLACK, COLOR_CYAN);   // menu/help bar
    init_pair(3, COLOR_WHITE, COLOR_BLUE);   // workspace text
    init_pair(4, COLOR_BLUE, COLOR_BLUE);    // fill
    init_pair(5, COLOR_BLACK, COLOR_WHITE);  // highlighted selection
    init_pair(6, COLOR_CYAN, COLOR_BLUE);    // line numbers

    bkgd(COLOR_PAIR(4));

    Editor editor;
    OutputPanel output;
    editor_init(&editor);
    output_init(&output);

    Focus focus = FOCUS_EDITOR;
    int active_menu = 0;
    bool menu_open = false;
    int project_selected = 0;
    char status[160] = "Pronto.";
    bool running = true;

    while (running) {
        int h, w;
        getmaxyx(stdscr, h, w);

        if (h < 24 || w < 90) {
            erase();
            mvprintw(0, 0, "Terminal minimo 90x24. Atual: %dx%d", w, h);
            mvprintw(1, 0, "Aumente a janela e pressione qualquer tecla. Esc para sair.");
            refresh();
            int ch = getch();
            if (ch == 27) running = false;
            continue;
        }

        int top = 2;
        int output_h = 7;
        int editor_h = h - top - output_h - 2;
        int project_w = 24;
        int editor_w = w - project_w;

        erase();
        draw_top_title(w);
        draw_menu_bar(w, active_menu, menu_open);
        draw_editor(&editor, top, 0, editor_h, editor_w, focus == FOCUS_EDITOR);
        draw_project(top, editor_w, editor_h, project_w, project_selected, focus == FOCUS_PROJECT);
        draw_output(&output, top + editor_h, 0, output_h, w, focus == FOCUS_OUTPUT);
        draw_status(h, w, focus, status);

        refresh();

        int ch = getch();
        if (ch == ERR) continue;

        if (ch == 27 && menu_open) {
            menu_open = false;
            focus = FOCUS_EDITOR;
            snprintf(status, sizeof(status), "Menu fechado.");
            continue;
        }

        if (ch == 27 && !menu_open) {
            running = false;
            continue;
        }

        if (ch == KEY_F(10)) {
            menu_open = !menu_open;
            focus = menu_open ? FOCUS_MENU : FOCUS_EDITOR;
            snprintf(status, sizeof(status), menu_open ? "Menu ativo." : "Menu fechado.");
            continue;
        }

        if (ch == KEY_F(6)) {
            focus = (Focus)((focus + 1) % FOCUS_COUNT);
            if (focus == FOCUS_MENU && !menu_open) focus = FOCUS_EDITOR;
            snprintf(status, sizeof(status), "Painel alterado.");
            continue;
        }

        if (ch == KEY_F(1)) {
            output_push(&output, "Help: F10 menu, F6 troca painel, F9 compila, Ctrl+F9 executa.");
            snprintf(status, sizeof(status), "Ajuda enviada ao Message.");
            continue;
        }

        if (ch == KEY_F(9)) {
            for (int i = 0; i < ARRAY_LEN(compile_msgs); i++) {
                output_push(&output, compile_msgs[i]);
            }
            output.scroll = output.count > 5 ? output.count - 5 : 0;
            focus = FOCUS_OUTPUT;
            snprintf(status, sizeof(status), "Compilacao concluida.");
            continue;
        }

        if (ch == KEY_F(21)) { // Ctrl+F9 em muitos terminais
            output_push(&output, "Running PROJECT.EXE...");
            output_push(&output, "Turbo C++ feel! total=42");
            output_push(&output, "Program terminated normally.");
            output.scroll = output.count > 5 ? output.count - 5 : 0;
            focus = FOCUS_OUTPUT;
            snprintf(status, sizeof(status), "Execucao concluida.");
            continue;
        }

        if (focus == FOCUS_MENU || menu_open) {
            menu_open = true;
            focus = FOCUS_MENU;

            switch (ch) {
                case KEY_LEFT:
                    active_menu = (active_menu - 1 + ARRAY_LEN(menus)) % ARRAY_LEN(menus);
                    break;
                case KEY_RIGHT:
                    active_menu = (active_menu + 1) % ARRAY_LEN(menus);
                    break;
                case '\n':
                case KEY_ENTER:
                    output_push(&output, "----------------------------------------");
                    output_push(&output, menus[active_menu]);
                    output_push(&output, "(submenu visual simplificado neste demo)");
                    menu_open = false;
                    focus = FOCUS_EDITOR;
                    snprintf(status, sizeof(status), "Menu %s selecionado.", menus[active_menu]);
                    break;
                default:
                    break;
            }
            continue;
        }

        switch (focus) {
            case FOCUS_EDITOR: {
                int body_h = editor_h - 2;
                switch (ch) {
                    case KEY_UP:
                        editor.cursor_y--;
                        break;
                    case KEY_DOWN:
                        editor.cursor_y++;
                        break;
                    case KEY_LEFT:
                        editor.cursor_x--;
                        break;
                    case KEY_RIGHT:
                        editor.cursor_x++;
                        break;
                    case KEY_PPAGE:
                        editor.cursor_y -= body_h;
                        break;
                    case KEY_NPAGE:
                        editor.cursor_y += body_h;
                        break;
                    default:
                        break;
                }
                maybe_scroll_editor(&editor, body_h);
                snprintf(status, sizeof(status), "Linha %d, Col %d", editor.cursor_y + 1, editor.cursor_x + 1);
                break;
            }
            case FOCUS_PROJECT:
                if (ch == KEY_UP && project_selected > 0) project_selected--;
                if (ch == KEY_DOWN && project_selected < ARRAY_LEN(project_files) - 1) project_selected++;
                if (ch == '\n' || ch == KEY_ENTER) {
                    snprintf(status, sizeof(status), "Arquivo ativo: %s", project_files[project_selected]);
                    output_push(&output, "Switching active unit...");
                    output_push(&output, project_files[project_selected]);
                }
                break;
            case FOCUS_OUTPUT:
                if (ch == KEY_UP && output.scroll > 0) output.scroll--;
                if (ch == KEY_DOWN && output.scroll < output.count - 1) output.scroll++;
                if (ch == 'c' || ch == 'C') {
                    output_init(&output);
                    output.scroll = 0;
                    snprintf(status, sizeof(status), "Message limpo.");
                }
                break;
            case FOCUS_MENU:
            default:
                break;
        }
    }

    endwin();
    return 0;
}
