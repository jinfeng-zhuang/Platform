#include "spi.h"

static const char *device = "/dev/spidev0.0";

static unsigned char tx_buf[FRAME_SIZE];
static unsigned char rx_buf[FRAME_SIZE];

static struct timespec suspend_time = {
    .tv_sec = 0,
    .tv_nsec = FRAME_DELAY
};

int main(int argc, char *argv[])
{
    int ret;

    ret = spi_init(device);
    assert(-1 != ret);

    int fd = open(device, O_RDWR);
    assert(fd >= 0);

    for (int i=0; i<FRAME_SIZE; i++) {
        tx_buf[i] = i;
    }

    while (1) {
        memset(rx_buf, 0, FRAME_SIZE);
        ret = spi_transfer_frame(fd, BPS, tx_buf, rx_buf, FRAME_SIZE);
        assert(-1 != ret);

        printf("Send: \n");
        hexdump(tx_buf, FRAME_SIZE);

        printf("Recved: \n");
        hexdump(rx_buf, FRAME_SIZE);

        fflush(stdout);

        nanosleep(&suspend_time, NULL);
    }
}

