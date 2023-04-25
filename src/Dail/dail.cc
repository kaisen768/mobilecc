#include "dail.h"
#include "ipaddr.h"

static int spawn_start(const char *exec, char *const *args, pid_t *pid, int *fds);

dail::dail(const std::string file)
{
    run_state = false;
    status = DIAL_UNOPENED;

    for (size_t i = 0; i < sizeof(_fds)/sizeof(_fds[0]); i++)
        _fds[i] = -1;

    _pid = -1;
    _exec = file;

    for (size_t i = 0; i < sizeof(_args)/sizeof(_args[0]); i++)
        _args[i] = nullptr;

    _args[0] = (char *)_exec.c_str();

    _stream_cb = nullptr;
    _exit_cb = nullptr;
}

dail::~dail()
{
}

void dail::start()
{
    int err;

    system("killall -9 pppd");

    err = generate_pppd_args();
    if (err)
        return;

    err = spawn_start(_exec.c_str(), (char *const *)_args, &_pid, _fds);
    if (err)
        return;

    run_state = true;
    status = DIAL_PROCESSING;
    hdlthread = std::thread(&dail::handle_thread_handler, this);
    hdlthread.detach();
}

void dail::stop()
{
    run_state = false;

    if (_pid > 0)
        kill(_pid, SIGTERM);

    while (status != DIAL_UNOPENED) { usleep(1000); }

    _pid = -1;
    procip.clear();
}

bool dail::online()
{
    char ip[32] = {0};

    if (_pid < 0)
        return false;

    if (get_interface_ipaddr(DEFAULT_PPPD_INTERFACE, ip, sizeof(ip)) != 0)
        return false;

    procip = ip;

    return true;
}

std::string dail::get_process_ip()
{
    char ip[32] = {0};

    if (!procip.empty())
        goto result;
    
    if (get_interface_ipaddr(DEFAULT_PPPD_INTERFACE, ip, sizeof(ip)) != 0)
        procip.clear();

    procip = ip;

result:
    return procip;
}

DIAL_STATUS_E dail::get_status()
{
    return status;
}

void dail::register_stream_callback(process_stream_cb stream_cb)
{
    _stream_cb = stream_cb;
}

void dail::register_exit_callback(process_exit_cb exit_cb)
{
    _exit_cb = exit_cb;
}

static inline int socketpair_create(int fds[2])
{
    int temp[2];
    int flags;

    flags = SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK;

    if (0 != socketpair(AF_UNIX, flags, 0, temp))
        return -1;

    fds[0] = temp[0];
    fds[1] = temp[1];

    return 0;
}

static inline int nonblock_fcntl(int fd, int set)
{
    int flags;
    int r;

    do
        r = fcntl(fd, F_GETFL);
    while (r == -1 && errno == EINTR);

    if (r == -1)
        return (errno);

    /* Bail out now if already set/clear. */
    if (!!(r & O_NONBLOCK) == !!set)
        return 0;

    if (set)
        flags = r | O_NONBLOCK;
    else
        flags = r & ~O_NONBLOCK;

    do
        r = fcntl(fd, F_SETFL, flags);
    while (r == -1 && errno == EINTR);

    if (r)
        return (errno);

    return 0;
}

static int global_signal_fds[2] = { -1, -1 };

static void signal_handler(int signum)
{
    int err;
    do {
        err = write(global_signal_fds[0], &signum, sizeof signum);
    } while (err == -1 && errno == EINTR);
}

static inline int signal_register_handle(int signum)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    if (sigfillset(&sa.sa_mask))
        abort();
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;

    if (sigaction(signum, &sa, NULL))
        return -1;

    return 0;
}

static void spawn_start_child_exec(const char *exec, char *const *args, int fds[2])
{
    sigset_t signewset;
    int fd;
    int n;

    for (n = 1; n < 32; n += 1) {
        if (n == SIGKILL || n == SIGSTOP)
        continue;  /* Can't be changed. */

        if (SIG_ERR != signal(n, SIG_DFL))
        continue;
    }

    fd = dup2(fds[0], 1);

    if (fd == -1)
      return;

    if (fd <= 2)
      nonblock_fcntl(fd, 0);

    /* Reset signal mask just before exec. */
    sigemptyset(&signewset);
    if (sigprocmask(SIG_SETMASK, &signewset, NULL) != 0)
        abort();

    execvp(exec, args);
}

static int spawn_start_child_fork(const char *exec, char * const args[], pid_t *pid, int fds[2])
{
    sigset_t signewset;
    sigset_t sigoldset;

    sigfillset(&signewset);
    sigdelset(&signewset, SIGKILL);
    sigdelset(&signewset, SIGSTOP);
    sigdelset(&signewset, SIGTRAP);
    sigdelset(&signewset, SIGSEGV);
    sigdelset(&signewset, SIGBUS);
    sigdelset(&signewset, SIGILL);
    sigdelset(&signewset, SIGSYS);
    sigdelset(&signewset, SIGABRT);
    if (pthread_sigmask(SIG_BLOCK, &signewset, &sigoldset) != 0)
        abort();

    *pid = fork();

    if (*pid == -1)
        return -1;

    if (*pid == 0) {
        spawn_start_child_exec(exec, args, fds);
        abort();
    }

    if (pthread_sigmask(SIG_SETMASK, &sigoldset, NULL) != 0)
        abort();

    return 0;
}

static int spawn_start(const char *exec, char * const args[], pid_t *pid, int fds[2])
{
    int err;
    int status;

    err = socketpair_create(global_signal_fds);
    if (err)
        goto error;

    err = socketpair_create(fds);
    if (err)
        goto error;

    err = signal_register_handle(SIGCHLD);
    if (err)
        goto error;

    err = spawn_start_child_fork(exec, args, pid, fds);
    if (err)
        goto error;

    return 0;

error:
    return -1;
}

void dail::child_exit_handle(int signum)
{
    assert(signum == SIGCHLD);

    int exit_status;
    int term_signal;
    int status;
    int options;
    pid_t pid;

    options = WNOHANG;

    do
        pid = waitpid(_pid, &status, options);
    while (pid == -1 && errno == EINTR);
    assert(pid == _pid);

    exit_status = 0;
    if (WIFEXITED(status))
        exit_status = WEXITSTATUS(status);

    term_signal = 0;
    if (WIFSIGNALED(status))
        term_signal = WTERMSIG(status);

    default_exit_callback(exit_status, term_signal);

    if (_exit_cb)
        _exit_cb(exit_status, term_signal);
}

void dail::default_exit_callback(int exit_status, int term_signal)
{
    run_state = false;
    _pid = -1;
}

void dail::handle_thread_handler()
{
#define EPOLL_EVENT_MAXNUM 5

    int err;
    int epollfd;
    int nfds;
    struct epoll_event ev, events[EPOLL_EVENT_MAXNUM];
    char recvbuf[1024];

    epollfd = epoll_create(EPOLL_EVENT_MAXNUM);
    
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = _fds[1];
    err = epoll_ctl(epollfd, EPOLL_CTL_ADD, _fds[1], &ev);
    if (err)
        goto error;

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = global_signal_fds[1];
    err = epoll_ctl(epollfd, EPOLL_CTL_ADD, global_signal_fds[1], &ev);
    if (err)
        goto error;

    while (run_state) {
        nfds = epoll_wait(epollfd, events, EPOLL_EVENT_MAXNUM, 200);

        for (size_t i = 0; i < nfds; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
                close(events[i].data.fd);
                continue;
            } else {
                // fprintf(stderr, "events fd:%d, _fds1:%d, global_signal_fds1:%d\n", 
                //     events[i].data.fd, _fds[1], global_signal_fds[1]);

                if (events[i].data.fd == _fds[1]) {
                    do
                    {
                        bzero(recvbuf, sizeof(recvbuf));
                        err = read(events[i].data.fd, recvbuf, sizeof(recvbuf));

                        if (_stream_cb)
                            _stream_cb(err, recvbuf);
                    } while (err > 0);
                }

                else if (events[i].data.fd == global_signal_fds[1]) {
                    int signum = 0;
                    err = read(events[i].data.fd, &signum, sizeof(signum));
                    if (err == sizeof(signum))
                        child_exit_handle(signum);
                }
            }
        }
    }

    status = DIAL_UNOPENED;
    return;

error:
    status = DIAL_UNOPENED;
    return;
}
