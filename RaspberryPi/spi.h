#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <time.h>
#include <assert.h>

#define CONFIG_SPI_MODE		(3)
#define CONFIG_SPI_SPEED_MAX	(1*1000*1000) // 1MHz

#define BPS			(100*1000) // 100KHz

#define FRAME_SIZE		(287)
#define FRAME_DELAY		(100*1000*1000) // 100 ms

#define TRACE printf("[trace] %s %d\n", __func__, __LINE__)

extern int spi_init(const char *device);
extern int spi_transfer_frame(int fd, int speed, uint8_t *tx, uint8_t *rx, int len);

extern void hexdump(const unsigned char *buffer, const unsigned int length);

#endif
