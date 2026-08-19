#include "collector.h"
#include "server_uploader.h"
#include "ui_ncurses.h"

int main() {
    Collector collector;
    NCursesDisplay display;
    Uploader uploader(collector, "localhost", 8080, "/statistics");
    display.start_ncurses(collector);
    return 0;
}