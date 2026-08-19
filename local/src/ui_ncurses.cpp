#include "ui_ncurses.h"

#include <locale.h>
#include <ncursesw/curses.h>
#include <unistd.h>

#include <chrono>
#include <deque>
#include <fstream>
#include <string>

#include "collector.h"

namespace {
    // Colors namespace
    namespace colors {
        constexpr int green = 1;
        constexpr int red = 2;
        constexpr int yellow = 3;
        constexpr int blue = 4;
        constexpr int magenta = 5;
        constexpr int cyan = 6;
        constexpr int white = 7;
        constexpr int black = 8;
    }  // namespace colors

    // Overall monitor size
    namespace layout {
        constexpr int monitor_width = 172;
        constexpr int content_win_height = 23;
        constexpr int poll_timeout_ms = 50;
    }  // namespace layout

    // layout for the top window.
    namespace top_window {
        constexpr int height = 3;
        constexpr int start_row = 0;
        constexpr int start_col = 0;
        constexpr int content_row = 1;
        constexpr int content_col = 2;
    }  // namespace top_window

    // process page and layout
    namespace process_window {
        constexpr int start_row = 3;
        constexpr int start_col = 0;
        constexpr int last_data_row = 21;

        constexpr int header_row = 1;
        constexpr int header_col = 2;
        constexpr int col_gap = 1;
        constexpr int width_pid = 7;
        constexpr int width_user = 9;
        constexpr int width_pri = 4;
        constexpr int width_nice = 4;
        constexpr int width_virt = 8;
        constexpr int width_res = 8;
        constexpr int width_shr = 8;
        constexpr int width_state = 2;
        constexpr int width_cpu = 6;
        constexpr int width_mem = 6;
        constexpr int width_time = 9;
        constexpr int width_threads = 5;
        constexpr int width_fd = 5;

        constexpr int col_pid = header_col;
        constexpr int col_user = col_pid + width_pid + col_gap;
        constexpr int col_pri = col_user + width_user + col_gap;
        constexpr int col_nice = col_pri + width_pri + col_gap;
        constexpr int col_virt = col_nice + width_nice + col_gap;
        constexpr int col_res = col_virt + width_virt + col_gap;
        constexpr int col_shr = col_res + width_res + col_gap;
        constexpr int col_state = col_shr + width_shr + col_gap;
        constexpr int col_cpu = col_state + width_state + col_gap;
        constexpr int col_mem = col_cpu + width_cpu + col_gap;
        constexpr int col_time = col_mem + width_mem + col_gap;
        constexpr int col_threads = col_time + width_time + col_gap;
        constexpr int col_fd = col_threads + width_threads + col_gap;
        constexpr int col_command = col_fd + width_fd + col_gap;
    }  // namespace process_window

    // display_left_window's layout
    namespace left_window {
        constexpr int start_col = 0;
        constexpr int first_core_row = 2;
        constexpr int row_content_width = 60;
        constexpr int core_bar_width = 40;
        constexpr int core_bar_col_offset = 10;
        constexpr int mem_row = 19;
        constexpr int swap_row = 20;
        constexpr int mem_swap_bar_width = 40;
        constexpr int mem_swap_value_offset = 9;

        constexpr double kib_to_gib = 1.0 / 1048576.0;
    }  // namespace left_window

    // display_right_window's layout
    namespace right_window {
        constexpr int cpu_bar_row = 2;
        constexpr int cpu_bar_width = 50;
        constexpr int cpu_bar_label_offset = 9;
        constexpr int cpu_bar_col_offset = 11;
    }  // namespace right_window

    // display_cpu_history_graph's layout
    namespace history_graph {
        constexpr int row_start = 3;
        constexpr int length = 66;
        constexpr int seconds = 60;
        constexpr int percent_rows = 11;
        constexpr int percent_step = 10;
        constexpr int tick_col_0s = 0;
        constexpr int tick_col_15s = 14;
        constexpr int tick_col_30s = 29;
        constexpr int tick_col_45s = 44;
        constexpr int tick_col_60s = 59;
        constexpr int axis_label_0s = 5;
        constexpr int axis_label_15s = 20;
        constexpr int axis_label_30s = 35;
        constexpr int axis_label_45s = 50;
        constexpr int axis_label_60s = 64;
        constexpr int zero_label_offset = 2;
        constexpr int default_label_offset = 1;
    }  // namespace history_graph

    // display_task_summary's layout
    namespace task_summary {
        constexpr int row = 16;
        constexpr int bottom_margin = 3;
        constexpr int win_min_coord = 1;
    }  // namespace task_summary

    // display_graph_colors' layout
    namespace graph_colors {
        constexpr int row = 19;
        constexpr int label_col_offset = 6;
        constexpr int column_spacing = 14;
        constexpr int bar_offset = 4;
    }  // namespace graph_colors

    // display_top_window's time conversions
    namespace time_conv {
        constexpr int seconds_per_day = 86400;
        constexpr int seconds_per_hour = 3600;
        constexpr int seconds_per_minute = 60;
    }  // namespace time_conv

}  // namespace

void NCursesDisplay::start_ncurses(Collector& collector) {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    // Set all color pairs we can use
    start_color();
    use_default_colors();
    init_pair(colors::green, COLOR_GREEN, -1);      // green
    init_pair(colors::red, COLOR_RED, -1);          // red
    init_pair(colors::yellow, COLOR_YELLOW, -1);    // yellow
    init_pair(colors::blue, COLOR_BLUE, -1);        // blue
    init_pair(colors::magenta, COLOR_MAGENTA, -1);  // magenta
    init_pair(colors::cyan, COLOR_CYAN, -1);        // cyan
    init_pair(colors::white, COLOR_WHITE, -1);      // white
    init_pair(colors::black, COLOR_BLACK, -1);      // black

    // Top box
    WINDOW* top_win = newwin(top_window::height, layout::monitor_width, top_window::start_row,
                             top_window::start_col);

    // Left box
    WINDOW* left_win = newwin(layout::content_win_height, layout::monitor_width / 2,
                              top_window::height, left_window::start_col);

    // Right box
    WINDOW* right_win = newwin(layout::content_win_height, layout::monitor_width / 2,
                               top_window::height, layout::monitor_width / 2);

    // Processs box
    WINDOW* process_win = newwin(layout::content_win_height, layout::monitor_width,
                                 process_window::start_row, process_window::start_col);

    auto checked_time = std::chrono::steady_clock::now();
    int hist_graph_cols_filled = 0;

    bool systems_page = true;
    bool process_page = false;
    timeout(layout::poll_timeout_ms);
    while (true) {
        int ch = getch();
        if (ch == 'q') break;
        if (ch == '1') {
            systems_page = true;
            process_page = false;
        }
        if (ch == '2') {
            systems_page = false;
            process_page = true;
        }

        bool advance_hist_graph = false;
        auto curr_time = std::chrono::steady_clock::now();
        if (curr_time - checked_time >= std::chrono::seconds(1)) {
            advance_hist_graph = true;
            checked_time += std::chrono::seconds(1);
            ;
            if (hist_graph_cols_filled < history_graph::seconds) {
                hist_graph_cols_filled++;
            }
        }

        auto monitor_data = collector.get_monitor_data();
        if (!monitor_data) continue;

        if (systems_page) {
            werase(top_win);
            werase(left_win);
            werase(right_win);
            wborder_set(top_win, WACS_VLINE, WACS_VLINE, WACS_HLINE, WACS_HLINE, WACS_ULCORNER,
                        WACS_URCORNER, WACS_LLCORNER, WACS_LRCORNER);
            wborder_set(right_win, WACS_VLINE, WACS_VLINE, WACS_HLINE, WACS_HLINE, WACS_ULCORNER,
                        WACS_URCORNER, WACS_LLCORNER, WACS_LRCORNER);
            wborder_set(left_win, WACS_VLINE, WACS_VLINE, WACS_HLINE, WACS_HLINE, WACS_ULCORNER,
                        WACS_URCORNER, WACS_LLCORNER, WACS_LRCORNER);
            display_top_window(top_win, *monitor_data);
            display_right_window(right_win, *monitor_data, advance_hist_graph,
                                 hist_graph_cols_filled);
            display_left_window(left_win, *monitor_data);

            wrefresh(top_win);
            wrefresh(left_win);
            wrefresh(right_win);
        } else {
            werase(top_win);
            werase(process_win);
            wborder_set(top_win, WACS_VLINE, WACS_VLINE, WACS_HLINE, WACS_HLINE, WACS_ULCORNER,
                        WACS_URCORNER, WACS_LLCORNER, WACS_LRCORNER);
            wborder_set(process_win, WACS_VLINE, WACS_VLINE, WACS_HLINE, WACS_HLINE, WACS_ULCORNER,
                        WACS_URCORNER, WACS_LLCORNER, WACS_LRCORNER);
            display_top_window(top_win, *monitor_data);
            display_process_window(process_win, *monitor_data);

            wrefresh(top_win);
            wrefresh(process_win);
        }
    }
    endwin();
}

void NCursesDisplay::display_top_window(WINDOW* win, const StatisticsData& monitor_data) {
    long long uptime_seconds = monitor_data.up_time;
    long uptime_days = uptime_seconds / time_conv::seconds_per_day;
    long uptime_hours = (uptime_seconds % time_conv::seconds_per_day) / time_conv::seconds_per_hour;
    long uptime_minutes =
        (uptime_seconds % time_conv::seconds_per_hour) / time_conv::seconds_per_minute;

    const std::string& os_info = monitor_data.os_info;
    const std::string& kernel_info = monitor_data.kernel_info;
    float last_minute_load = monitor_data.last_minute_load;
    float last_five_minutes_load = monitor_data.last_five_minutes_load;
    float last_fifteen_minutes_load = monitor_data.last_fifteen_minutes_load;
    float up_time = monitor_data.up_time;

    if (uptime_days > 0) {
        mvwprintw(win, top_window::content_row, top_window::content_col,
                  "%s  |  %s  |  uptime %ldd %02ld:%02ld  |  LoadAVG 1m: %.2f  5m: %.2f  15m: %.2f",
                  os_info.c_str(), kernel_info.c_str(), uptime_days, uptime_hours, uptime_minutes,
                  last_minute_load, last_five_minutes_load, last_fifteen_minutes_load);
    } else {
        mvwprintw(win, top_window::content_row, top_window::content_col,
                  "%s  |  %s  |  uptime %02ld:%02ld  |  LoadAVG 1m: %.2f  5m: %.2f  15m: %.2f",
                  os_info.c_str(), kernel_info.c_str(), uptime_hours, uptime_minutes,
                  last_minute_load, last_five_minutes_load, last_fifteen_minutes_load);
    }
    std::string menu = "[1] Systems  [2] Processes";
    int top_height, top_width;
    getmaxyx(win, top_height, top_width);
    mvwprintw(win, top_window::content_row, top_width - static_cast<int>(menu.size()) - 1, "%s",
              menu.c_str());
}

void NCursesDisplay::display_left_window(WINDOW* win, const StatisticsData& monitor_data) {
    int num_cores = static_cast<int>(monitor_data.cores.size());
    for (int i = 0; i < num_cores; i++) {
        const CoreData& core = monitor_data.cores[i];
        float core_percentage_total = core.usage_percent;
        mvwprintw(win, i + left_window::first_core_row,
                  ((layout::monitor_width / 2) / 2) - (left_window::row_content_width / 2),
                  "Core %d:    ", i);
        display_cpu_graph(win, core_percentage_total, core, monitor_data,
                          i + left_window::first_core_row,
                          ((layout::monitor_width / 2) / 2) - (left_window::row_content_width / 2) +
                              left_window::core_bar_col_offset,
                          left_window::core_bar_width);
    }
    long mem_total = monitor_data.mem_total;
    long mem_used = monitor_data.mem_total - monitor_data.mem_free;
    long mem_buffers = monitor_data.buffers;
    long mem_cache = monitor_data.cached + monitor_data.sreclaimable - monitor_data.shmem;
    long mem_shared = monitor_data.shmem;
    display_memory_graph(win, mem_total, mem_used, mem_buffers, mem_cache, mem_shared,
                         left_window::mem_row,
                         ((layout::monitor_width / 2) / 2) - (left_window::row_content_width / 2),
                         left_window::mem_swap_bar_width);

    long swap_total = monitor_data.swap_total;
    long swap_used = monitor_data.swap_total - monitor_data.swap_free;
    display_swap_graph(win, swap_total, swap_used, left_window::swap_row,
                       ((layout::monitor_width / 2) / 2) - (left_window::row_content_width / 2),
                       left_window::mem_swap_bar_width);
}

void NCursesDisplay::display_cpu_graph(WINDOW* win, float percentage, const CoreData& core,
                                       const StatisticsData& monitor_data, int row, int col,
                                       int width) {
    long long core_total = core.total;
    long long core_user = core.user;
    long long core_nice = core.nice;
    long long core_system = core.system;
    long long core_irq = core.irq;
    long long core_softirq = core.softirq;

    if (core_total <= 0) {
        mvwprintw(win, row, col, "[");
        mvwprintw(win, row, col + width + 1, "]  %.01f%%", percentage);
    } else {
        int user_chars = (core_user / (double)core_total) * width;
        int nice_chars = (core_nice / (double)core_total) * width;
        int system_chars = (core_system / (double)core_total) * width;
        int irq_chars = ((core_irq + core_softirq) / (double)core_total) * width;

        mvwprintw(win, row, col, "[");

        wattron(win, COLOR_PAIR(colors::green));
        wprintw(win, "%s", std::string(user_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::green));

        wattron(win, COLOR_PAIR(colors::red));
        wprintw(win, "%s", std::string(system_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::red));

        wattron(win, COLOR_PAIR(colors::yellow));
        wprintw(win, "%s", std::string(irq_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::yellow));

        wattron(win, COLOR_PAIR(colors::blue));
        wprintw(win, "%s", std::string(nice_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::blue));

        mvwprintw(win, row, col + width + 1, "]  %.01f%%", percentage);
    }
}

void NCursesDisplay::display_memory_graph(WINDOW* win, long total_mem, long total_used_mem,
                                          long buffers, long cache, long shared, int row, int col,
                                          int width) {
    if (total_mem <= 0) {
        mvwprintw(win, row, col, "Mem:  [");
        mvwprintw(win, row, col + width + left_window::mem_swap_value_offset, "]  n/a");
    } else {
        long used_mem = total_used_mem - buffers - cache - shared;
        int used_chars = (used_mem / (double)total_mem) * width;
        int buffers_chars = (buffers / (double)total_mem) * width;
        int cache_chars = (cache / (double)total_mem) * width;
        int shared_chars = (shared / (double)total_mem) * width;

        mvwprintw(win, row, col, "Mem:  [");

        wattron(win, COLOR_PAIR(colors::green));
        wprintw(win, "%s", std::string(used_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::green));

        wattron(win, COLOR_PAIR(colors::cyan));
        wprintw(win, "%s", std::string(buffers_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::cyan));

        wattron(win, COLOR_PAIR(colors::yellow));
        wprintw(win, "%s", std::string(cache_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::yellow));

        wattron(win, COLOR_PAIR(colors::blue));
        wprintw(win, "%s", std::string(shared_chars, '|').c_str());
        wattroff(win, COLOR_PAIR(colors::blue));

        mvwprintw(win, row, col + width + left_window::mem_swap_value_offset, "]  %.01fG/%.01fG",
                  used_mem * left_window::kib_to_gib, total_mem * left_window::kib_to_gib);
    }
}

void NCursesDisplay::display_swap_graph(WINDOW* win, long swap_total, long swap_used, int row,
                                        int col, int width) {
    int swap_bar = 0;
    if (swap_total != 0) swap_bar = (swap_used / (double)swap_total) * width;

    mvwprintw(win, row, col, "Swp:  [");

    wattron(win, COLOR_PAIR(colors::green));
    wprintw(win, "%s", std::string(swap_bar, '|').c_str());
    wattroff(win, COLOR_PAIR(colors::green));

    mvwprintw(win, row, col + width + left_window::mem_swap_value_offset, "]  %.01fG/%.01fG",
              swap_used * left_window::kib_to_gib, swap_total * left_window::kib_to_gib);
}

void NCursesDisplay::display_right_window(WINDOW* win, const StatisticsData& monitor_data,
                                          bool advance_hist_graph, int hist_graph_cols_filled) {
    // Print Cpu bar above history graph
    const CoreData& total_core = monitor_data.total_cpu_core;
    float total_percentage = total_core.usage_percent;
    int total_bar_col = ((layout::monitor_width / 2) / 2) - (right_window::cpu_bar_width / 2) -
                        right_window::cpu_bar_label_offset;
    mvwprintw(win, right_window::cpu_bar_row, total_bar_col, "CPU Usage: ");
    display_cpu_graph(win, total_percentage, total_core, monitor_data, right_window::cpu_bar_row,
                      total_bar_col + right_window::cpu_bar_col_offset,
                      right_window::cpu_bar_width);
    display_cpu_history_graph(win, advance_hist_graph, hist_graph_cols_filled, total_percentage,
                              layout::monitor_width / 2, history_graph::row_start);

    int summary_col = ((layout::monitor_width / 2) / 2) - (history_graph::length / 2);
    display_task_summary(win, monitor_data, task_summary::row, summary_col);
    display_graph_colors(win, graph_colors::row, summary_col);
}

void NCursesDisplay::display_cpu_history_graph(WINDOW* win, bool advance, int filled_columns,
                                               int cpu_percentage, int column, int row) {
    if (percentage_lines[0].empty()) {
        for (int i = 0; i < history_graph::percent_rows; i++) {
            for (int j = 0; j < history_graph::seconds; j++) {
                if (i != 0) {
                    percentage_lines[i].push_front("\u00B7");  // middle dot
                } else {
                    if (j == history_graph::tick_col_0s || j == history_graph::tick_col_15s ||
                        j == history_graph::tick_col_30s || j == history_graph::tick_col_45s ||
                        j == history_graph::tick_col_60s) {
                        percentage_lines[i].push_front("\u2502");  // middle line
                    } else
                        percentage_lines[i].push_front("-");
                }
            }
        }
    }

    int col = (column / 2) - (history_graph::length / 2);
    for (int i = history_graph::percent_rows - 1; i > -1; i--) {
        int col_start = i == history_graph::percent_rows - 1 ? col
                        : i == 0 ? col + history_graph::zero_label_offset
                                 : col + history_graph::default_label_offset;
        if (advance) {
            if (cpu_percentage / history_graph::percent_step >= i && i != 0) {
                percentage_lines[i].push_front("\u2503");  // bolded middle line
            } else {
                if (i != 0) percentage_lines[i].push_front("\u00B7");  // middle dot
            }
            if (i != 0) percentage_lines[i].pop_back();
        }

        mvwprintw(win, row, col_start, "%d%%  ", i * history_graph::percent_step);
        for (uint32_t j = 0; j < percentage_lines[i].size(); j++) {
            std::string character = percentage_lines[i].at(j);
            if (i != 0) {
                if (character == "\u2503") {
                    wattron(win, COLOR_PAIR(colors::magenta));
                    wprintw(win, "%s", character.c_str());
                    wattroff(win, COLOR_PAIR(colors::magenta));
                } else if (character == "\u00B7") {
                    wattron(win, A_DIM);
                    wprintw(win, "%s", character.c_str());
                    wattroff(win, A_DIM);
                }
            } else if (j < filled_columns) {
                if (character == "\u2502") {
                    wattron(win, COLOR_PAIR(colors::magenta));
                    wprintw(win, "\u2503");
                    wattroff(win, COLOR_PAIR(colors::magenta));
                } else if (character == "-") {
                    wattron(win, COLOR_PAIR(colors::magenta));
                    wprintw(win, "\u2501");
                    wattroff(win, COLOR_PAIR(colors::magenta));
                }
            } else {
                wprintw(win, "%s", character.c_str());
            }
        }
        row++;
    }
    mvwprintw(win, row, col + history_graph::axis_label_0s, "0s");
    mvwprintw(win, row, col + history_graph::axis_label_15s, "15s");
    mvwprintw(win, row, col + history_graph::axis_label_30s, "30s");
    mvwprintw(win, row, col + history_graph::axis_label_45s, "45s");
    mvwprintw(win, row, col + history_graph::axis_label_60s, "60s");
}

void NCursesDisplay::display_task_summary(WINDOW* win, const StatisticsData& monitor_data, int row,
                                          int col) {
    int num_running = 0;
    int num_sleeping = 0;
    int num_stopped = 0;
    int num_zombie = 0;
    int uninterruptible_count = 0;
    long long threads = 0;

    for (const ProcessData& process : monitor_data.pids) {
        threads += process.num_threads;
        switch (process.state) {
            case 'R':
                num_running++;
                break;
            case 'S':
                num_sleeping++;
                break;
            case 'D':
                uninterruptible_count++;
                break;
            case 'T':
            case 't':
                num_stopped++;
                break;
            case 'Z':
                num_zombie++;
                break;
            default:
                break;
        }
    }

    int total_count = static_cast<int>(monitor_data.pids.size());

    mvwprintw(
        win, row, col, "Tasks:   %d total, %d running, %d asleep, %d locked, %d stopped, %d zombie",
        total_count, num_running, num_sleeping, uninterruptible_count, num_stopped, num_zombie);
    mvwprintw(win, row + 1, col, "Threads: %lld    Ctxt/s:  %.0f    Intr/s: %.0f", threads,
              monitor_data.ctxt_per_second, monitor_data.interruptions_per_second);
}

void NCursesDisplay::display_graph_colors(WINDOW* win, int row, int col) {
    int label_col[4] = {col + graph_colors::label_col_offset,
                        col + graph_colors::label_col_offset + graph_colors::column_spacing,
                        col + graph_colors::label_col_offset + 2 * graph_colors::column_spacing,
                        col + graph_colors::label_col_offset + 3 * graph_colors::column_spacing};

    mvwprintw(win, row, col, "CPU:  ");
    wattron(win, COLOR_PAIR(colors::green));
    mvwprintw(win, row, label_col[0], "||| ");
    wattroff(win, COLOR_PAIR(colors::green));
    mvwprintw(win, row, label_col[0] + graph_colors::bar_offset, "%-10s", "User");

    wattron(win, COLOR_PAIR(colors::red));
    mvwprintw(win, row, label_col[1], "||| ");
    wattroff(win, COLOR_PAIR(colors::red));
    mvwprintw(win, row, label_col[1] + graph_colors::bar_offset, "%-10s", "System");

    wattron(win, COLOR_PAIR(colors::yellow));
    mvwprintw(win, row, label_col[2], "||| ");
    wattroff(win, COLOR_PAIR(colors::yellow));
    mvwprintw(win, row, label_col[2] + graph_colors::bar_offset, "%-10s", "IRQ");

    wattron(win, COLOR_PAIR(colors::blue));
    mvwprintw(win, row, label_col[3], "||| ");
    wattroff(win, COLOR_PAIR(colors::blue));
    mvwprintw(win, row, label_col[3] + graph_colors::bar_offset, "%-10s", "Nice");

    mvwprintw(win, row + 1, col, "Mem:  ");
    wattron(win, COLOR_PAIR(colors::green));
    mvwprintw(win, row + 1, label_col[0], "||| ");
    wattroff(win, COLOR_PAIR(colors::green));
    mvwprintw(win, row + 1, label_col[0] + graph_colors::bar_offset, "%-10s", "Used");

    wattron(win, COLOR_PAIR(colors::yellow));
    mvwprintw(win, row + 1, label_col[1], "||| ");
    wattroff(win, COLOR_PAIR(colors::yellow));
    mvwprintw(win, row + 1, label_col[1] + graph_colors::bar_offset, "%-10s", "Cache");

    wattron(win, COLOR_PAIR(colors::cyan));
    mvwprintw(win, row + 1, label_col[2], "||| ");
    wattroff(win, COLOR_PAIR(colors::cyan));
    mvwprintw(win, row + 1, label_col[2] + graph_colors::bar_offset, "%-10s", "buffers");

    wattron(win, COLOR_PAIR(colors::blue));
    mvwprintw(win, row + 1, label_col[3], "||| ");
    wattroff(win, COLOR_PAIR(colors::blue));
    mvwprintw(win, row + 1, label_col[3] + graph_colors::bar_offset, "%-10s", "Shared");
}

void NCursesDisplay::display_process_window(WINDOW* win, const StatisticsData& monitor_data) {
    mvwprintw(win, process_window::header_row, process_window::col_pid, "%-*s",
              process_window::width_pid, "PID");
    mvwprintw(win, process_window::header_row, process_window::col_user, "%-*s",
              process_window::width_user, "USER");
    mvwprintw(win, process_window::header_row, process_window::col_pri, "%-*s",
              process_window::width_pri, "PRI");
    mvwprintw(win, process_window::header_row, process_window::col_nice, "%-*s",
              process_window::width_nice, "NI");
    mvwprintw(win, process_window::header_row, process_window::col_virt, "%-*s",
              process_window::width_virt, "VIRT");
    mvwprintw(win, process_window::header_row, process_window::col_res, "%-*s",
              process_window::width_res, "RES");
    mvwprintw(win, process_window::header_row, process_window::col_shr, "%-*s",
              process_window::width_shr, "SHR");
    mvwprintw(win, process_window::header_row, process_window::col_state, "%-*s",
              process_window::width_state, "S");
    mvwprintw(win, process_window::header_row, process_window::col_cpu, "%-*s",
              process_window::width_cpu, "%CPU");
    mvwprintw(win, process_window::header_row, process_window::col_mem, "%-*s",
              process_window::width_mem, "%MEM");
    mvwprintw(win, process_window::header_row, process_window::col_time, "%-*s",
              process_window::width_time, "TIME+");
    mvwprintw(win, process_window::header_row, process_window::col_threads, "%-*s",
              process_window::width_threads, "THR");
    mvwprintw(win, process_window::header_row, process_window::col_fd, "%-*s",
              process_window::width_fd, "FD");
    mvwprintw(win, process_window::header_row, process_window::col_command, "%s", "COMMAND");

    int win_height, win_width;
    getmaxyx(win, win_height, win_width);
    int max_command_width = win_width - process_window::col_command - 1;

    int first_data_row = process_window::header_row + 1;
    int max_rows = process_window::last_data_row - first_data_row + 1;
    if (max_rows < 0) max_rows = 0;

    int pid_count = static_cast<int>(monitor_data.pids.size());
    int rows_to_draw = pid_count < max_rows ? pid_count : max_rows;

    for (int i = 0; i < rows_to_draw; i++) {
        // Grab a random PID
        const ProcessData& process = monitor_data.pids[i];
        int row = first_data_row + i;

        long virt_kb = process.virtual_memory_bytes / 1024;
        long res_kb = process.resident_memory_bytes / 1024;
        long shr_kb = process.shared_memory_bytes / 1024;

        double mem_percent =
            monitor_data.mem_total > 0
                ? (process.resident_memory_bytes / (double)monitor_data.mem_total) * 100.0
                : 0;

        long clock_ticks_per_sec = sysconf(_SC_CLK_TCK);
        long long total_ticks = process.utime + process.stime;
        long total_seconds = clock_ticks_per_sec > 0 ? total_ticks / clock_ticks_per_sec : 0;
        long time_minutes = total_seconds / 60;
        long time_seconds = total_seconds % 60;

        std::string command_display = process.cmdline;
        if (max_command_width > 0 && (int)command_display.size() > max_command_width) {
            command_display = command_display.substr(0, max_command_width);
        }

        mvwprintw(win, row, process_window::col_pid, "%-*d", process_window::width_pid,
                  process.pid);
        mvwprintw(win, row, process_window::col_user, "%-*u", process_window::width_user,
                  process.uid);
        mvwprintw(win, row, process_window::col_pri, "%-*d", process_window::width_pri,
                  process.priority);
        mvwprintw(win, row, process_window::col_nice, "%-*d", process_window::width_nice,
                  process.nice);
        mvwprintw(win, row, process_window::col_virt, "%-*ld", process_window::width_virt, virt_kb);
        mvwprintw(win, row, process_window::col_res, "%-*ld", process_window::width_res, res_kb);
        mvwprintw(win, row, process_window::col_shr, "%-*ld", process_window::width_shr, shr_kb);
        mvwprintw(win, row, process_window::col_state, "%-*c", process_window::width_state,
                  process.state);
        mvwprintw(win, row, process_window::col_cpu, "%-*s", process_window::width_cpu, "n/a");
        mvwprintw(win, row, process_window::col_mem, "%-*.1f", process_window::width_mem,
                  mem_percent);
        mvwprintw(win, row, process_window::col_time, "%02ld:%02ld", time_minutes, time_seconds);
        mvwprintw(win, row, process_window::col_threads, "%-*d", process_window::width_threads,
                  process.num_threads);
        mvwprintw(win, row, process_window::col_fd, "%-*d", process_window::width_fd,
                  process.fd_count);
        mvwprintw(win, row, process_window::col_command, "%s", command_display.c_str());
    }
}