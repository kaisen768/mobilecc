#include "dail.h"

// ----------------------------------------------------------------------------
//  PPPD Chat Connect File Configure
// ----------------------------------------------------------------------------

static inline int chat_file_connect_config(const char *filename, const char *apn)
{
    int i = 0;
    int l_num = 0;
    FILE *fp = NULL;

    const char *line[256];

    char cgdcont_cmd[64] = {0};
    if (apn)
        sprintf(cgdcont_cmd, "OK AT+CGDCONT=1,\"IP\",\"%s\",\"\",0,0\n", apn);
    else
        sprintf(cgdcont_cmd, "OK AT+CGDCONT=1,\"IP\",\"3gnet\",\"\",0,0\n");

    line[l_num++] = "TIMEOUT 1800\n";
    line[l_num++] = "ABORT \"BUSY\"\n";
    line[l_num++] = "ABORT \"NO CARRIER\"\n";
    line[l_num++] = "ABORT \"NODIALTONE\"\n";
    line[l_num++] = "ABORT \"ERROR\"\n";
    line[l_num++] = "ABORT \"NO ANSWER\"\n";
    line[l_num++] = "\"\" AT\n";
    line[l_num++] = "OK ATE0\n";
    line[l_num++] = "OK ATI;+CSUB;+CSQ;+CPIN?;+COPS?;+CGREG?;&D2\n";
    line[l_num++] = cgdcont_cmd;
    line[l_num++] = "OK-AT-OK ATDT*99#\n";
    line[l_num++] = "CONNECT \\d\\c\n";

    if ((fp = fopen(filename, "w+")) == NULL)
        return -1;

    while (i < l_num) {
        fwrite(line[i], strlen(line[i]), 1, fp);
        i++;
    }

    fclose(fp);
    return 0;
}

// ----------------------------------------------------------------------------
//  PPPD Chat Disconnect File Configure
// ----------------------------------------------------------------------------

static inline int chat_file_disconnect_config(const char *filename)
{
    int i = 0;
    int l_num = 0;
    FILE *fp = NULL;

    const char *line[256];

    line[l_num++] = "ABORT \"ERROR\"\n";
    line[l_num++] = "ABORT \"NO DIALTONE\"\n";
    line[l_num++] = "SAY \"\nSending break to the modem\n\"\n";
    line[l_num++] = "\"\"  +++\n";
    line[l_num++] = "\"\"  +++\n";
    line[l_num++] = "\"\"  +++\n";
    line[l_num++] = "SAY \"\nGoodbay\n\"";

    if ((fp = fopen(filename, "w+")) == NULL)
        return -1;

    while (i < l_num) {
        fwrite(line[i], strlen(line[i]), 1, fp);
        i++;
    }

    fclose(fp);
    return 0;
}

// ----------------------------------------------------------------------------
//  PPPD Process Arguments Configure
// ----------------------------------------------------------------------------
// pppd debug nodetach /dev/ttyUSB0 115200 usepeerdns noauth noipdefault novj novjccomp noccp defaultroute ipcp-accept-local ipcp-accept-remote connect '/usr/bin/chat -s -v -f /tmp/chat-connect'

int dail::generate_pppd_args()
{
    int err;
    int i = 0;

    err = chat_file_connect_config(DEFAULT_CHAT_CONNECT_PATHNAME, nullptr);
    if (err)
        return -1;

    err = chat_file_disconnect_config(DEFAULT_CHAT_DISCONN_PATHNAME);
    if (err)
        return -1;

    static char chat_connect[64];
    memset(chat_connect, 0, sizeof(chat_connect));
    sprintf(chat_connect, "chat -s -v -f %s", DEFAULT_CHAT_CONNECT_PATHNAME);

    static char chat_disconnect[64];
    memset(chat_disconnect, 0, sizeof(chat_disconnect));
    sprintf(chat_disconnect, "chat -s -v -f %s", DEFAULT_CHAT_DISCONN_PATHNAME);

    _args[i++] = _exec.c_str();

    /* Modem path, like /dev/ttyUSB3,/dev/ttyACM0, depend on your module, default path is /dev/ttyUSB3 */
    _args[i++] = "/dev/ttyUSB3";
    _args[i++] = "115200";

    /* The chat script, customize your APN in this file */
    _args[i++] = "connect";
    _args[i++] = chat_connect;

    /* The close script */
    _args[i++] = "disconnect";
    _args[i++] = chat_disconnect;

    /* Debug info from pppd */
    _args[i++] = "debug";
    
	/* The phone is not required to authenticate */
    _args[i++] = "noauth";
    
    /* Hide password in debug messages */
    _args[i++] = "hide-password";

    /* f you want to use the HSDPA link as your gateway */
    _args[i++] = "defaultroute";

    /* pppd must not propose any IP address to the peer */
    _args[i++] = "noipdefault";

    /* No ppp compression */
    _args[i++] = "novj";
    _args[i++] = "novjccomp";
    _args[i++] = "noccp";
    _args[i++] = "ipcp-accept-local";
    _args[i++] = "ipcp-accept-remote";
    _args[i++] = "local";

    /* For sanity, keep a lock on the serial line */
    _args[i++] = "modem";
    _args[i++] = "nodetach";

    /* Hardware flow control */
    _args[i++] = "nocrtscts";

    /* Ask the peer for up to 2 DNS server addresses */
    _args[i++] = "usepeerdns";

    _args[i++] = (char*)0;

    return 0;
}
