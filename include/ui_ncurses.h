#ifndef UI_NCURSES_H
#define UI_NCURSES_H

#include "ui_ncurses.h"
#include "stat.h"
#include "mem.h"
#include <string>
#include <ncursesw/curses.h>
#include <thread>
#include <chrono>
#include <deque>
#include <locale.h>

class NCursesDisplay
{
public:
    void DisplaySystemsPage();
    void DisplayCPUGraph(WINDOW *win, float percentage, CoreCalculator corestats, int row, int col, int width);
    void DisplayCPUHistoryGraph(WINDOW *win, int time, int cpuPercentage, int column, int row);
    void DisplayMemoryGraph(WINDOW *win, MemoryParser &memstats, int row, int col, int width);
    void DisplaySwapGraph(WINDOW *win, MemoryParser &memStats, int row, int col, int width);
};

#endif