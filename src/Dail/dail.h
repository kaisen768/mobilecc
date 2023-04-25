#ifndef _DAIL_H_
#define _DAIL_H_

#include <iostream>
#include <thread>
#include <string>
#include <functional>

extern "C" {
    #include <stdlib.h>
    #include <unistd.h>
    #include <strings.h>
    #include <assert.h>
    #include <errno.h>
    #include <sys/socket.h>

    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <string.h>
    #include <sys/epoll.h>
};

#define DEFAULT_PPPD_PATHNAME           "pppd"
// #define DEFAULT_PPPD_PATHNAME           "/home/kaisen/wk/program/rep/mobilecc/build/assertest-long"
#define DEFAULT_PPPD_INTERFACE          "ppp0"
#define DEFAULT_CHAT_CONNECT_PATHNAME   "/tmp/chat-connect-defualt"
#define DEFAULT_CHAT_DISCONN_PATHNAME   "/tmp/chat-disconnect-defualt"

typedef enum dial_status {
    DIAL_UNOPENED = 0,
    DIAL_PROCESSING,
    DIAL_RERUN,
} DIAL_STATUS_E;

typedef std::function<void (ssize_t nread, const char *buf)> process_stream_cb;
typedef std::function<void (int exit_status, int term_signal)> process_exit_cb;

class dail
{
private:
    std::thread hdlthread;
    bool run_state;
    std::string procip;
    process_stream_cb _stream_cb;
    process_exit_cb _exit_cb;
    DIAL_STATUS_E status;

    void handle_thread_handler();
    void child_exit_handle(int signum);
    void default_exit_callback(int exit_status, int term_signal);

    int generate_pppd_args();

public:
    dail(const std::string file = DEFAULT_PPPD_PATHNAME);
    ~dail();

    void start();

    void stop();

    bool online();

    std::string get_process_ip();

    DIAL_STATUS_E get_status();

    void register_stream_callback(process_stream_cb stream_cb);
    void register_exit_callback(process_exit_cb exit_cb);

public:
    int         _fds[2];
    pid_t       _pid;
    std::string _exec;
    const char* _args[32];
};

#endif
