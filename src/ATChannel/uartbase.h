#ifndef _UARTBASE_H_
#define _UARTBASE_H_

extern "C"{
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <math.h>
    #include <time.h>
    #include <unistd.h>
    #include <termios.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdlib.h>
}

#include <iostream>
#include <thread>

class uartbase
{
public:
    uartbase(std::string _device_name, int _baudrate=115200, int _bitnum=8, int _stopbits=1, int _parity='N');
    ~uartbase();

private:
    int openport(std::string pathname);
    void closeport();
    bool setupport();
    void readselectthreadhandler();

public:  
    int send(const char *buf, int len) const;
    virtual void onreceive(int nread, const char *buf){}
    void start();
    void stop();
    bool isenable();
    int getfd();

private:
    int fd;
    std::string device_name;
    int baudrate;
    int bitnum;
    int stopbits;
    int parity;
    bool run_state;
    std::thread read_select_thread;
};

#endif
