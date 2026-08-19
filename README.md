# Linux System Monitor 
- Linux monitor using ncurses in C++ to parse virtual filesystem files such as /proc/stat to display a TUI and send data history to a spring boot service.
<img width="1725" height="501" alt="linuxmonitorgif" src="https://github.com/user-attachments/assets/b386d2f6-73dc-4eed-9f8d-7b3070e89039" />
<img width="1440" height="975" alt="image" src="https://github.com/user-attachments/assets/ebb60eb8-167c-43b3-bb6b-a788c37a9d39" />
## Features
- Parses filesystem files such as /proc/stat to display memory, swap, and CPU per-core usage with bars and graphs for continuous updates in the terminal.
- Displays per-process statistics by parsing files in each /proc/[PID] directory, displaying individual process data in a toggleable ncurses terminal window.
- Transfers /proc data collection to a background thread, pushing data through a shared_ptr guarded by a mutex.
- Connects the C++ client to the Java service with a background thread that uses cpp-httplib to upload the serialized JSON monitor metrics through a C++-to-Java-to-PostgreSQL pipeline.
- Uses a Spring Boot REST API in Java with JPA/Hibernate to map snapshot monitor data to PostgreSQL and to query methods for specific data lookup.
- Containerizes the Java Spring Boot service and PostgreSQL database with Docker, allowing for reproducible runs.
