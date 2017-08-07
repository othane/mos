/*
 * @file i2c_utest.c
 *
 * @brief unit test the i2c hal module
 *
 * This test is designed to check the uart hal module.
 *
 * @author CC
 *
 * @date Aug 2017
 *
 */


#include <stdbool.h>
#include <string.h>
#include <hal.h>

void init(void)
{
    sys_init();
    i2c_init(&i2c_dev);
}

int main(void)
{
    uint8_t tx_buf[1] = {0x00};
    uint8_t rx_buf[] = {};
    
	init();
    tx_buf[0] = 0x00;
    while(1){
        /*reading tempeature sensor from developmemnt board*/
        i2c_write(&i2c_dev,0x90,(void *)tx_buf,1, NULL, NULL);
        i2c_read(&i2c_dev, 0x90, rx_buf, 2, NULL, NULL);
    }
	return 0;
}

