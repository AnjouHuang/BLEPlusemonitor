#include "nrf.h"
#include "bsp.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "common.h"

//sd卡接口引脚定义
#define SDC_SCK_PIN 	NRF_GPIO_PIN_MAP(0, 23)  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN 	NRF_GPIO_PIN_MAP(0, 20) ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN 	NRF_GPIO_PIN_MAP(0, 21) ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN 		NRF_GPIO_PIN_MAP(0, 19)   ///< SDC chip select (CS) pin.


/**
 * @brief  SDC block device definition
 * */
NRF_BLOCK_DEV_SDC_DEFINE(
    m_block_dev_sdc,
    NRF_BLOCK_DEV_SDC_CONFIG(
        SDC_SECTOR_SIZE,
        APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)),
    NFR_BLOCK_DEV_INFO_CONFIG("Nordic", "SDC", "1.00"));



char file_name[20] = "0000000000000.hex"; 	//记录数据的文件名
char time_stamp[14]; 												//采样开始时的时间戳
uint8_t file_manage_state = 0;


/**
 * @brief Function for demonstrating FAFTS usage.
 */
int fatfs_init()
{
		static FATFS fs;
    static DIR dir;
    static FILINFO fno;
    static FIL file;

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    printf("Initializing disk 0 (SDC)...");
    for (uint32_t retries = 5; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        printf("Disk initialization failed.");
        return -1;
    }

    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    printf(": %d MB", capacity);

    printf("Mounting volume...");
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        printf("Mount failed.");
        return -2;
    }
		
		printf("\r\n Listing directory: /");
    ff_result = f_opendir(&dir, "/");
    if (ff_result)
    {
        printf("Directory listing failed!");
        return -1;
    }

    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            printf("Directory read failed.");
            return -1;
				}
        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                printf("   <DIR>   %s \r\n",(uint32_t)fno.fname);
            }
            else
            {
                printf("%9lu  %s \r\n", fno.fsize, (uint32_t)fno.fname);
            }
        }
    }
    while (fno.fname[0]);
    printf("");
		
    return 0;
}

//将数据保存在本地文件中
int save_adc_value(char filename[], uint16_t val)
{
    FRESULT ff_result;
    FIL fil;
    uint32_t bytes_written;
    uint8_t buf[2];
		
    buf[0] = val >> 8;
    buf[1] = val & 0xFF;

    printf("adc_val: %d \r\n", val);

    ff_result = f_open(&fil, file_name , FA_READ | FA_WRITE | FA_OPEN_APPEND);
		
    if (ff_result != FR_OK)
    {
        printf("Unable to open or create file:  %s .\r\n", file_name);
        return -1;
    }

    ff_result = f_write(&fil, buf, 2 , (UINT *)&bytes_written);
		
    if (ff_result != FR_OK)
    {
        printf("Write failed\r\n.");
        return -2;
    }
    else
    {
        printf("%d bytes written.\r\n", bytes_written);
    }

    (void)f_close(&fil);
		
    return 0;
}



