
#include "common.h"

#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_pwr_mgmt.h"

#define SAMPLES_IN_BUFFER 1 
//设置缓冲的数量，决定要填满几个缓冲后启动中断
volatile uint8_t state = 1;
uint16_t adc_val = 0;    //adc读数
bool sample_state = false;   //采样状态开关

static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(1);
static nrf_saadc_value_t     m_buffer_pool[2][SAMPLES_IN_BUFFER];//设置缓冲的组数，这里也就是对应使用的通道的数量
static nrf_ppi_channel_t     m_ppi_channel;
static uint32_t              m_adc_evt_counter;


void timer_handler(nrf_timer_event_t event_type, void * p_context)
{

}


void saadc_sampling_event_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event every 20ms */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 20);
    nrf_drv_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   false);
    nrf_drv_timer_enable(&m_timer);
		//设置PPI通道
    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
                                                                                NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_task_addr   = nrf_drv_saadc_sample_task_get();

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel,
                                          timer_compare_event_addr,
                                          saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}


void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);

    APP_ERROR_CHECK(err_code);
}

void saadc_sampling_event_disable(void)
{
		ret_code_t err_code = nrf_drv_ppi_channel_disable(m_ppi_channel);

    APP_ERROR_CHECK(err_code);
}

void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;

        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);


        //printf("%d", p_event->data.done.p_buffer[0]);

				//adc_val = p_event->data.done.p_buffer[0];
				adc_val=fake_value(adc_val);
				printf("ADCvalue=%d\r\n",adc_val);
        m_adc_evt_counter++;
				
    }
}
//三通道回调
//void saadc_callback(nrf_drv_saadc_evt_t const *p_event)
//{
//	  ret_code_t err_code;
//    float val;  //保存SAADC采样数据计算的实际电压值
//	  if(p_event->type == NRF_DRV_SAADC_EVT_DONE)
//		{
//		    //设置好缓存，为下一次采样做准备
//			  err_code=nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer,
//			                                        SAMPLES_IN_BUFFER);
//			   APP_ERROR_CHECK(err_code);
//			   //读取采样结果，使用串口发送
//			   for(uint32_t i=0;i<SAMPLES_IN_BUFFER;i++)
//			   {
//			    val=p_event->data.done.p_buffer[i]*3.6/1024;	
////                printf("Voltage = %.3fV\r\n", val);
//					 switch(i)
//					 {
//					    case 0:printf("AIN2=%f\r\n",val); break;
//						case 1:printf("AIN0=%f\r\n",val); break;
//						case 2:printf("VDD=%f\r\n",val); break;
//						default:break;
//					 }
//			   }
//		}
//}
void saadc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2);//单端输入

    err_code = nrf_drv_saadc_init(NULL, saadc_callback);//调用ADC默认配置参数，可在sdk_config.h中修改
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);//通道选择
    APP_ERROR_CHECK(err_code);
		//adc缓冲设置
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

}
//三通道采样代码
/*
void saadc_init(void)
{
	ret_code_t err_code;
	//定义SAADC采样通道0初始化配置结构体变量,并用默认参数初始化
	nrf_saadc_channel_config_t channel_0_config=
	    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2);
	channel_0_config.gain=NRF_SAADC_GAIN1_6;
	channel_0_config.reference=NRF_SAADC_REFERENCE_INTERNAL;  //使用内部的0.6V作为参考电压
	
	
	//定义SAADC采样通道1初始化配置结构体，并用默认参数初始化
		nrf_saadc_channel_config_t channel_1_config=
	    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
	channel_1_config.gain=NRF_SAADC_GAIN1_6;
	channel_1_config.reference=NRF_SAADC_REFERENCE_INTERNAL;  //使用内部的0.6V作为参考电压

	//定义SAADC采样通道2初始化配置结构体
	nrf_saadc_channel_config_t channel_2_config=
	    NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(SAADC_CH_PSELP_PSELP_VDD);

	channel_2_config.gain=NRF_SAADC_GAIN1_6;
	channel_2_config.reference=NRF_SAADC_REFERENCE_INTERNAL;  //使用内部的0.6V作为参考电压

	//初始化SAADC
	err_code=nrf_drv_saadc_init(NULL,saadc_callback);
  APP_ERROR_CHECK(err_code);
	
	//初始化SAADC的通道0
	err_code = nrf_drv_saadc_channel_init(0, &channel_0_config);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_channel_init(1, &channel_1_config);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_channel_init(2, &channel_2_config);
  APP_ERROR_CHECK(err_code);
	
	//使用双缓存
	//设置好第一个缓存
	err_code=nrf_drv_saadc_buffer_convert(m_buffer_pool,SAMPLES_IN_BUFFER);
	APP_ERROR_CHECK(err_code);
//	//设置好第二个缓存
//	err_code=nrf_drv_saadc_buffer_convert(m_buffer_pool[1],SAMPLES_IN_BUFFER);
//	APP_ERROR_CHECK(err_code);
}
*/

/**
 * @brief Function for main application entry.
 */
int sampling_init(void)
{

    saadc_init();
    saadc_sampling_event_init();
    
    printf("SAADC init. \r\n");
		
		return 0;
}

void sampling_start(void)
{
	saadc_sampling_event_enable();
	sample_state = true;
}

void sampling_stop(void)
{
	sample_state = false;
	saadc_sampling_event_disable();
}

uint16_t fake_value(uint16_t val){
			return (val+1)%1000;
}