#ifndef _ATCHANNEL_H_
#define _ATCHANNEL_H_

#include "uartbase.h"
#include "4Gmodule_paras.h"

#define DEFAULT_UART_NAME   "/dev/ttyUSB2"
#define MOUDLE_PARAS_USE_SHM 0

class atchannel : public uartbase
{
private:
    shm_4Gmodule_t *ec20;
    std::thread setupthread;
    bool run_state;
    bool _isexist;

    enum {
        SETUPSTEP_READY = 0,
        SETUPSTEP_IDENTIFICATION,   // 查询模块信息
        SETUPSTEP_SIM_CHECK,        // 检测SIM卡
        SETUPSTEP_S_SCLK,           // 禁用休眠模式
        SETUPSTEP_CPIN_CHECK,       // 检查是否需要密码
        SETUPSTEP_S_FUN_LESS,       // 设置最小功能
        SETUPSTEP_S_BAND,           // 设置为所有工作频段
        SETUPSTEP_S_OCSEARFCN,      // 清除 NB-IOT 存储的 EARFCN 列表
        SETUPSTEP_S_FUN_FULL,       // 设置全功能
        SETUPSTEP_S_GATT,           // 网络附着
        SETUPSTEP_CREG_CHECK,       // 查询网络注册状态
        SETUPSTEP_G_FUN,            // 获取功能参数
        SETUPSTEP_G_GATT            // 获取网络附着状态
    } setupstep;

private:
    int sendto(std::string command);
    void onreceive(int nread, const char *buf);

    void setup_handler();
    void command_handle(const std::string data);

    int command_indentification();
    int command_signal();
    int command_sim_insertion_status();
    int command_cpin_required();
    int command_phone_functional_set(int fun);
    int command_phone_functional_get();
    int command_ps_attachment_set(int state);
    int command_ps_attachment_get();
    int command_network_status();

public:
    atchannel(std::string _device_name = DEFAULT_UART_NAME, 
              int _baudrate = 115200, 
              int _bitnum = 8, 
              int _stopbits = 1, 
              int _parity = 'N');
    ~atchannel();

    void start();

    void stop();

    bool isrunning();

    bool isready();

    shm_4Gmodule_t *get_shm_4Gmodule();
};


#endif
