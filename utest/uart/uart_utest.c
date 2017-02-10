/**
 * @file uart_utest.c
 *
 * @brief unit test the uart hal module
 *
 * This test is designed to check the uart hal module.
 *
 * @author OT
 *
 * @date Feb 2016
 *
 */


#include <stdbool.h>
#include <string.h>
#include <hal.h>


void init(void)
{
	sys_init();
	uart_init(&uart_dev);
	uart_set_read_timeout(&uart_dev, 1);
}


uint8_t rx_buf[16] = {0x00,};
void rx_complete(uart_t *uart, void *buf, uint16_t len, void *param)
{
	uart_read(uart, buf, sizeof(rx_buf), rx_complete, param);
}


bool tx_flag = true;
uint8_t tx_buf[] = "Hello World\n\r";
void tx_complete(uart_t *uart, void *buf, uint16_t len, void *param)
{
	tx_flag = true;
}


int main(void)
{
	init();

	uart_read(&uart_dev, rx_buf, sizeof(rx_buf), rx_complete, NULL);
	while (1)
	{
		if (tx_flag)
		{
			tx_flag = false;
			sys_spin(1000);
			uart_write(&uart_dev, (void *)tx_buf, sizeof(tx_buf), tx_complete, NULL);
		}
	}

	return 0;
}

