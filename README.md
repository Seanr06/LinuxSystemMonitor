# Linux System Monitor 
- A system monitor in C++ that parses files from /proc and displays CPU and memory statistics using a ncurses-based interface.
<img width="1377" height="787" alt="image" src="https://github.com/user-attachments/assets/2f9fab61-cecd-4927-bbb0-6eac576f94e9" />
## Features
- Parses /proc/stat and /proc/meminfo to display CPU per-core usage along with a total CPU usage bar and history graph that updates continuously in the terminal.
- Displays memory usage and swap statistics bars in the terminal.
- Parses /proc/version and /etc/os-release to display the kernel version as well as the OS name and version.
## Planned Features
- Display per-process statistics in the terminal, parsing from each PID directory files.
- Display network info by parsing from /net/dev.
- Separate parsing and statistical information from the UI by using multiple threads and potentially reducing overhead.
