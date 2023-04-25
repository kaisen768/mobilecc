#include "uartbase.h"

uartbase::uartbase(std::string _device_name ,int _baudrate ,int _bitnum ,int _stopbits ,int _parity)
{
    fd = -1;
    device_name = _device_name;
    baudrate    = _baudrate;
    bitnum      = _bitnum;
    stopbits    = _stopbits;
    parity      = _parity;
    run_state   = false;
}

uartbase::~uartbase()
{
    closeport();
}

////////////////////////////////////////////////////////////////////////////
int uartbase::openport(std::string pathname)
{
    if (pathname.empty()) {
        fprintf(stderr ,"input %s error!\n" ,pathname.c_str());
        return -1;
    }

    fd = open(pathname.c_str(), O_RDWR|O_NOCTTY);
    if (fd < 0)
        goto err;

    fcntl(fd, F_SETFL, 0);

err:
    return fd;
}

void uartbase::closeport()
{
    run_state = false;
    device_name.clear();

    if (fd > 0) {
        close(fd);
        fd = -1;
    }
}
bool uartbase::setupport()
{
    if (!isatty(fd))
    {
        fprintf(stderr, "\nERROR: file descriptor %d is NOT a serial port\n", fd);
        return false;
    }

    struct termios config;

    if (tcgetattr(fd, &config) < 0)
    {
        fprintf(stderr, "\nERROR: could not read configuration of fd %d\n", fd);
        return false;
    }

    /*
     * Input flags - Turn off input processing
	 * convert break to null byte, no CR to NL translation,
	 * no NL to CR translation, don't mark parity errors or breaks
	 * no input parity check, don't strip high bit off,
	 * no XON/XOFF software flow control
     */
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
						                    INLCR | PARMRK | INPCK | ISTRIP | IXON);

    /*
     * Output flags - Turn off output processing
	 * no CR to NL translation, no NL to CR-NL translation,
	 * no NL to CR translation, no column 0 CR suppression,
	 * no Ctrl-D suppression, no fill characters, no case mapping,
	 * no local output processing
     */
	config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
						                    ONOCR | OFILL | OPOST);

    #ifdef OLCUC
		config.c_oflag &= ~OLCUC;
	#endif

	#ifdef ONOEOT
		config.c_oflag &= ~ONOEOT;
	#endif

    /*
     * No line processing:
	 * echo off, echo newline off, canonical mode off,
	 * extended input processing off, signal chars off
     */
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    /* clear current char size mask */
	config.c_cflag &= ~(CSIZE);

    /* parity checking */
    switch (parity)
    {
    case 'N':
        config.c_cflag &= ~(PARENB);
        break;
    case 'O':
        config.c_cflag |= (PARENB | PARODD);
        break;
    case 'E':
        config.c_cflag |= (PARENB | PARODD);
        break;
    default:
        fprintf(stderr, "\nERROR: invail parity argument [%d], it must be 'N'/'O' or 'E'\n", parity);
        return false;
        break;
    }

    /* force data bit input */
    if (bitnum == 7)
	    config.c_cflag |= CS7;
    else
        config.c_cflag |= CS8;

    /*
     * One input byte is enough to return from read()
	 * Inter-character timer off
     */
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 10;

    /*
     * Get the current options for the port
	 *  struct termios options;
	 *  tcgetattr(fd, &options);
     */

	/* Apply baudrate */
    switch (baudrate)
    {
    case 1200:
        if (cfsetispeed(&config, B1200) < 0 || cfsetospeed(&config, B1200) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baudrate);
			return false;
		}
        break;

    case 1800:
		cfsetispeed(&config, B1800);
		cfsetospeed(&config, B1800);
		break;

    case 9600:
		cfsetispeed(&config, B9600);
		cfsetospeed(&config, B9600);
		break;

    case 19200:
		cfsetispeed(&config, B19200);
		cfsetospeed(&config, B19200);
		break;

    case 38400:
		if (cfsetispeed(&config, B38400) < 0 || cfsetospeed(&config, B38400) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baudrate);
			return false;
		}
		break;

    case 57600:
		if (cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baudrate);
			return false;
		}
		break;

    case 115200:
		if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baudrate);
			return false;
		}
		break;

    case 460800:
		if (cfsetispeed(&config, B460800) < 0 || cfsetospeed(&config, B460800) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baudrate);
			return false;
		}
		break;

	case 921600:
		if (cfsetispeed(&config, B921600) < 0 || cfsetospeed(&config, B921600) < 0)
		{
			fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baudrate);
			return false;
		}
		break;

    default:
        fprintf(stderr, "ERROR: Desired baud rate %d could not be set, aborting.\n", baudrate);
        return false;

        break;
    }

    /* Apply the configuration */
    if(tcsetattr(fd, TCSAFLUSH, &config) < 0)
	{
		fprintf(stderr, "\nERROR: could not set configuration of fd %d\n", fd);
		return false;
	}

    return true;
}

int uartbase::send(const char *buf, int len) const
{
    if ((fd < 0) || (nullptr == buf) || (len <= 0))
        return -1;

	return write(fd ,buf ,len);
}

void uartbase::readselectthreadhandler()
{
    char _buf[1024] = {0}; 
    int _len = 0;

    while (run_state) {
        if (fd < 0)
            usleep(500*1000);

        memset(_buf, 0, sizeof(_buf));
        _len = read(fd, _buf, sizeof(_buf));
        if(_len > 0)
            onreceive(_len, _buf);
    }
}

void uartbase::start()
{
    int r = openport(device_name);
    if (r <= 0)
        return;

    r = setupport();
    if (true != r) {
        closeport();
        return;
    }

    run_state = true;

    read_select_thread = std::thread(&uartbase::readselectthreadhandler, this);
    read_select_thread.detach();

    printf("---->uart start successful!\n");
}

void uartbase::stop()
{
    closeport();
}

bool uartbase::isenable()
{
    return run_state;
}

int uartbase::getfd()
{
    return fd;
}
