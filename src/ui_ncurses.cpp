#include "ui_ncurses.h"
#include "stat.h"
#include "mem.h"

#include <string>
#include <ncursesw/curses.h>
#include <thread>
#include <chrono>
#include <deque>
#include <locale.h>

void NCursesDisplay::DisplaySystemsPage()
{
    // Parsers
    StatParser stats;
    MemoryParser memStats;

    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    // Set all color pairs we can use
    start_color();
    use_default_colors();
    init_pair(1, COLOR_GREEN, -1);   // green
    init_pair(2, COLOR_RED, -1);     // red
    init_pair(3, COLOR_YELLOW, -1);  // yellow
    init_pair(4, COLOR_BLUE, -1);    // blue
    init_pair(5, COLOR_MAGENTA, -1); // magenta
    init_pair(6, COLOR_CYAN, -1);    // cyan
    init_pair(7, COLOR_WHITE, -1);   // white
    init_pair(8, COLOR_BLACK, -1);   // black

    // Top box
    WINDOW *topWin = newwin(3, COLS - 1, 0, 0);
    int topWinHeight, topWinWidth;
    getmaxyx(topWin, topWinHeight, topWinWidth);

    // Left box
    WINDOW *leftWin = newwin(LINES - 3, (COLS / 2) - 10, 3, 0);
    int leftWinHeight, leftWinWidth;
    getmaxyx(leftWin, leftWinHeight, leftWinWidth);

    // Right box
    WINDOW *rightWin = newwin(LINES - 3, (COLS / 2) + 10, 3, (COLS / 2) - 10);
    int rightWinHeight, rightWinWidth;
    getmaxyx(rightWin, rightWinHeight, rightWinWidth);

    // Might remove these
    cchar_t ls, rs, ts, bs, tl, tr, bl, br;
    setcchar(&ls, L"│", 0, 0, nullptr);
    setcchar(&rs, L"│", 0, 0, nullptr);
    setcchar(&ts, L"─", 0, 0, nullptr);
    setcchar(&bs, L"─", 0, 0, nullptr);
    setcchar(&tl, L"┌", 0, 0, nullptr);
    setcchar(&tr, L"┐", 0, 0, nullptr);
    setcchar(&bl, L"└", 0, 0, nullptr);
    setcchar(&br, L"┘", 0, 0, nullptr);

    timeout(500);
    int counter = 0;

    while (true)
    {
        counter++;

        int ch = getch();
        if (ch == 'q')
            break;

        stats.updateCPUUsage();
        stats.computeStats();
        memStats.computeMemoryStatistics();

        // erase and redraw
        werase(topWin);
        werase(leftWin);
        werase(rightWin);
        wborder_set(topWin, &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
        wborder_set(leftWin, &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
        wborder_set(rightWin, &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);

        int numCores = stats.getCoreCount();
        for (int i = 0; i < numCores; i++)
        {
            CoreCalculator core = stats.getIndividualCore(i);
            float corePercentageTotal = core.getCpuUsagePercent();
            mvwprintw(leftWin, i + 2, (leftWinWidth / 2) - (60 / 2), "Core %d:    ", i);
            DisplayCPUGraph(leftWin, corePercentageTotal, core, i + 2, (leftWinWidth / 2) - (60 / 2) + 10, 40);
        }
        DisplayMemoryGraph(leftWin, memStats, 19, (leftWinWidth / 2) - (60 / 2), 40);
        DisplaySwapGraph(leftWin, memStats, 20, (leftWinWidth / 2) - (60 / 2), 40);

        float totalPercentage = stats.getTotalCore().getCpuUsagePercent();
        int totalBarCol = (rightWinWidth / 2) - (50 / 2) - 9;
        mvwprintw(rightWin, 1, totalBarCol, "CPU Usage: ");
        // Print Cpu bar above history graph
        DisplayCPUGraph(rightWin, totalPercentage, stats.getTotalCore(), 1, totalBarCol + 11, 50);
        DisplayCPUHistoryGraph(rightWin, counter, totalPercentage, rightWinWidth, 2);

        wrefresh(topWin);
        wrefresh(leftWin);
        wrefresh(rightWin);
    }
    endwin();
}

void NCursesDisplay::DisplayMemoryGraph(WINDOW *win, MemoryParser &memStats, int row, int col, int width)
{
    long totalMem = memStats.getMemTotal();
    long totalUsedMem = memStats.getMemUsed();

    int buffers = (memStats.getBuffers() / (double)totalMem) * width;
    int cache = ((memStats.getCached() + memStats.getSReclaimable() / (double)totalMem) * width);
    int shared = (memStats.getShmem() / (double)totalMem) * width;
    int usedmem = ((totalUsedMem - shared - cache - buffers) / (double)totalMem) * width;

    mvwprintw(win, row, col, "Mem:  [");

    wattron(win, COLOR_PAIR(1));
    wprintw(win, "%s", std::string(usedmem, '|').c_str());
    wattroff(win, COLOR_PAIR(1));

    wattron(win, COLOR_PAIR(6));
    wprintw(win, "%s", std::string(buffers, '|').c_str());
    wattroff(win, COLOR_PAIR(6));

    wattron(win, COLOR_PAIR(3));
    wprintw(win, "%s", std::string(cache, '|').c_str());
    wattroff(win, COLOR_PAIR(3));

    wattron(win, COLOR_PAIR(4));
    wprintw(win, "%s", std::string(shared, '|').c_str());
    wattroff(win, COLOR_PAIR(4));

    mvwprintw(win, row, col + width + 9, "]  %.01fG/%.01fG", totalUsedMem * .000001, totalMem * .000001);
}

void NCursesDisplay::DisplaySwapGraph(WINDOW *win, MemoryParser &memStats, int row, int col, int width)
{
    long swapTotal = memStats.getSwapTotal();
    long swapUsed = memStats.getSwapUsed();

    int swapBar = 0;
    if (swapTotal != 0)
        swapBar = (swapUsed / (double)swapTotal) * width;

    mvwprintw(win, row, col, "Swp:  [");

    wattron(win, COLOR_PAIR(1));
    wprintw(win, "%s", std::string(swapBar, '|').c_str());
    wattroff(win, COLOR_PAIR(1));

    mvwprintw(win, row, col + width + 9, "]  %.01fG/%.01fG",
              swapUsed * .000001, swapTotal * .000001);
}

void NCursesDisplay::DisplayCPUGraph(WINDOW *win, float percentage, CoreCalculator corestats, int row, int col, int width)
{
    long long total = corestats.calcTotal();
    int user_chars = (corestats.getUser() / (double)total) * width;
    int nice_chars = (corestats.getNice() / (double)total) * width;
    int system_chars = (corestats.getSystem() / (double)total) * width;
    int irq_chars = ((corestats.getIRQ() + corestats.getSoftIRQ()) / (double)total) * width;
    int idle_chars = width - user_chars - nice_chars - system_chars - irq_chars;

    mvwprintw(win, row, col, "[");

    wattron(win, COLOR_PAIR(1));
    wprintw(win, "%s", std::string(user_chars, '|').c_str());
    wattroff(win, COLOR_PAIR(1));

    wattron(win, COLOR_PAIR(2));
    wprintw(win, "%s", std::string(system_chars, '|').c_str());
    wattroff(win, COLOR_PAIR(2));

    wattron(win, COLOR_PAIR(3));
    wprintw(win, "%s", std::string(irq_chars, '|').c_str());
    wattroff(win, COLOR_PAIR(3));

    wattron(win, COLOR_PAIR(4));
    wprintw(win, "%s", std::string(nice_chars, '|').c_str());
    wattroff(win, COLOR_PAIR(4));

    mvwprintw(win, row, col + width + 1, "]  %.01f%%", percentage);
}

void NCursesDisplay::DisplayCPUHistoryGraph(WINDOW *win, int time, int cpuPercentage, int column, int row)
{
    static std::deque<std::string> percentageLines[11];
    if (time == 1)
    {
        for (int i = 0; i < 11; i++)
        {
            for (int j = 0; j < 60; j++)
            {
                if (i != 0)
                {
                    percentageLines[i].push_front("\u00B7"); // middle dot
                }
                else
                {
                    if (j == 0 || j == 14 || j == 29 || j == 44 || j == 59)
                    {
                        percentageLines[i].push_front("\u2502"); // middle line
                    }
                    else
                        percentageLines[i].push_front("-");
                }
            }
        }
    }

    int graphLength = 66;
    int col = (column / 2) - (graphLength / 2);
    for (int i = 10; i > -1; i--)
    {

        int colStart = i == 10 ? col : i == 0 ? col + 2
                                              : col + 1;
        if (time % 2 == 0)
        {
            if (cpuPercentage / 10 >= i && i != 0)
            {
                percentageLines[i].push_front("\u2503"); // bolded middle line
            }
            else
            {
                if (i != 0)
                    percentageLines[i].push_front("\u00B7"); // middle dot
            }
            if (i != 0)
                percentageLines[i].pop_back();
        }

        mvwprintw(win, row, colStart, "%d%%  ", i * 10);
        for (int j = 0; j < percentageLines[i].size(); j++)
        {
        std:
            string character = percentageLines[i].at(j);
            if (i != 0)
            {
                if (character == "\u2503")
                {
                    wattron(win, COLOR_PAIR(5));
                    wprintw(win, "%s", character.c_str());
                    wattroff(win, COLOR_PAIR(5));
                }
                else if (character == "\u00B7")
                {
                    wattron(win, A_DIM);
                    wprintw(win, "%s", character.c_str());
                    wattroff(win, A_DIM);
                }
            }
            else if (j < time / 2)
            {
                if (character == "\u2502")
                {
                    wattron(win, COLOR_PAIR(5));
                    wprintw(win, "\u2503");
                    wattroff(win, COLOR_PAIR(5));
                }
                else if (character == "-")
                {
                    wattron(win, COLOR_PAIR(5));
                    wprintw(win, "\u2501");
                    wattroff(win, COLOR_PAIR(5));
                }
            }
            else
            {
                wprintw(win, "%s", character.c_str());
            }
        }
        row++;
    }
    mvwprintw(win, row, col + 5, "0s");
    mvwprintw(win, row, col + 20, "15s");
    mvwprintw(win, row, col + 35, "30s");
    mvwprintw(win, row, col + 50, "45s");
    mvwprintw(win, row, col + 64, "60s");
}