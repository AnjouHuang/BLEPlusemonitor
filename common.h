#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_hrs.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "sensorsim.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_freertos.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "fds.h"
#include "ble_conn_state.h"
#include "nrf_drv_clock.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "ff.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_delay.h"



//公有宏定义
#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */



//公有变量
extern uint16_t adc_val;    //adc读数
extern char file_name[20];  //目标文件名
extern char time_stamp[14]; //采样开始时的时间戳
extern bool sample_state;   //采样状态开关
extern uint8_t file_manage_state;
extern uint16_t   m_ble_nus_max_data_len;

//公有函数
extern void uart_event_handle(app_uart_evt_t * p_event);
extern void nus_data_handler(ble_nus_evt_t * p_evt);


extern int fatfs_init(void);        									//fatfs初始化
extern int  save_adc_value(char filename[], uint16_t val);  						//将adc值保存到文件
extern void ble_app_init(void);												//蓝牙服务初始化
extern void idle_state_handle(void);

extern int 	sampling_init(void);
extern void sampling_start(void);									//开始采样
extern void sampling_stop(void);											//停止采样
extern uint16_t fake_value(uint16_t);	

extern void uart_init(void);													//uart初始化
extern uint32_t data_send(uint8_t data[],uint16_t length);