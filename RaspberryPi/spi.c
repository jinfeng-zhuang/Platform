#include "spi.h"

int spi_transfer_frame(int fd, int speed, uint8_t *tx_buf, uint8_t *rx_buf, int len)
{
    int ret;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = len,
        .delay_usecs = 0,
        .speed_hz = speed,
        .bits_per_word = 8,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1) {
        return -1;
    }

    return 0;
}

int spi_init(const char *device)
{
    int ret;
    int fd;

    int mode = CONFIG_SPI_MODE;
    int speed_max = CONFIG_SPI_SPEED_MAX;

    int bits = 8;

    fd = open(device, O_RDWR);
    if (fd < 0) {
        printf("failed to open %s\n", device);
        goto end;
    }

    /*
     * spi mode
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        goto end;

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        goto end;

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        goto end;

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        goto end;

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_max);
    if (ret == -1)
        goto end;

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed_max);
    if (ret == -1)
        goto end;

    ret = 0;
end:
    if (fd >= 0) {
        close(fd);
    }

    return ret;
}
