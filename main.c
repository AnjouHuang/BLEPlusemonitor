#include "common.h"
       



int main(void)
{
		uart_init();		//串口初始化
//		fatfs_init();		//文件系统初始化
		sampling_init();	//adc采样初始化
		ble_app_init();		//蓝牙服务初始化
		spi_flash_init();
		spi_flash_test();
		printf("spi test finish\r\n");
		for(;;){
			idle_state_handle();
		}



//文件系统相关	
//		FIL file;
//		FRESULT ff_result;
//		BYTE buffer[244];   /* File copy buffer */
//		UINT br;
//		ret_code_t err_code;
//		
//	printf("file.obj.objsize=%d\r\n", (int)file.obj.objsize);
//	
//    for (;;)
//    {
//				//如果开始采样,则将adc值记录在文件中
//				if(sample_state == true)
//				{
//					save_adc_value(file_name, adc_val);
//					//延时20ms, 即50HZ
//					nrf_delay_ms(20);
//				}
//				
//				else if(1 == file_manage_state)
//				{
//					if(0 == (int)file.obj.objsize) //若文件尚未打开,则打开文件
//					{
//								ff_result = f_open(&file, "a.hex", FA_READ );
//						
//								printf("file.obj.objsize=%d\r\n", (int)file.obj.objsize);
//						
//								if (ff_result != FR_OK)
//								{
//										printf("Unable to open or create file");
//										file_manage_state = 0;
//										continue;
//								}
//					}
//					
//					ff_result = f_read(&file, buffer, m_ble_nus_max_data_len, (UINT *) &br);
//					if(br == 0)
//					{
//						file_manage_state = 0;
//						(void) f_close(&file);
//						printf("file.obj.objsize=%d\r\n", (int)file.obj.objsize);
//						printf("done \r\n");
//						continue;
//					}
//					
//					
//					do
//					{                    
//							err_code = data_send(buffer, br);
//						
//							if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_RESOURCES) &&
//											 (err_code != NRF_ERROR_NOT_FOUND) )
//							{
//											APP_ERROR_CHECK(err_code);
//							}
////							if (err_code == NRF_SUCCESS)
////							{
////									m_len_sent += length;     
////									m_data_array[0]++;
////									m_data_array[length-1]++;    
////							}
//						} while (err_code != NRF_SUCCESS);
//						
//						//nrf_delay_ms(5);
//				}
//				
//				else
//				{
//					nrf_delay_ms(50);
//				}
//				
//				
//				
//    }
}


