#ifndef UI_NCURSES_H
#define UI_NCURSES_H

#include <locale.h>
#include <ncursesw/curses.h>

#include <chrono>
#include <deque>
#include <string>
#include <thread>

#include "collector.h"
#include "mem.h"
#include "process.h"
#include "stat.h"
#include "systeminfo.h"
#include "ui_ncurses.h"

class Collector;

class NCursesDisplay {
   private:
    std::deque<std::string> percentage_lines[11];

   public:
    // Right window
    void display_cpu_graph(WINDOW* win, float percentage, const CoreData& core,
                           const StatisticsData& monitor_data, int row, int col, int width);
    void display_memory_graph(WINDOW* win, long total_mem, long total_used_mem, long buffers,
                              long cache, long shared, int row, int col, int width);
    void display_swap_graph(WINDOW* win, long swap_total, long swap_used, int row, int col,
                            int width);

    // Left window
    void display_cpu_history_graph(WINDOW* win, bool advance, int filled_columns,
                                   int cpu_percentage, int column, int row);
    void display_task_summary(WINDOW* win, const StatisticsData& monitor_data, int row, int col);
    void display_graph_colors(WINDOW* win, int row, int col);

    // All windows
    void display_left_window(WINDOW* win, const StatisticsData& monitor_data);
    void display_right_window(WINDOW* win, const StatisticsData& monitor_data,
                              bool advance_hist_graph, int hist_graph_cols_filled);
    void display_top_window(WINDOW* win, const StatisticsData& monitor_data);
    void display_process_window(WINDOW* win, const StatisticsData& monitor_data);

    void start_ncurses(Collector& collector);
};

#endif