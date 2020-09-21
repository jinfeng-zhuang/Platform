#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <tinycrypt/sha256.h>

#include "hex_dump.h"
#include "debug.h"

/*****************************************************************************/

#define SPI_PKG_SIZE 		(128)

#define SPI_DL_PAYLOAD_SIZE	\
	(sizeof(((struct master_frame_t *)0)->data.dl_cont.payload))

#define MAGIC_NUM		(0x58595453)

/*****************************************************************************/

struct master_frame_t {
        uint32_t magic;
#define CMD_QUERY               0
#define CMD_DL_START            1
#define CMD_DL_CONT             2
#define CMD_SET_SPI_FREQ        3
#define CMD_UART_ENABLE         4
#define CMD_DUMP_MEM            5
#define CMD_SET_MEM             6
        uint8_t cmd;
        union {
                struct {
                        uint8_t img_type;
#define OPTION_DMA_EN           (1 << 0)
#define OPTION_BYPASS_SHA_CHECK (1 << 1)
#define OPTION_DL_ONLY          (1 << 2)
                        uint16_t option;
                        uint32_t img_len;
                        uint8_t sha[32];
                        uint32_t img_loc;
                        uint8_t res1[SPI_PKG_SIZE - 48];
                } __attribute__((__packed__)) dl_start;
                struct {
                        uint8_t img_type;
                        uint8_t len;
                        uint8_t end_flag;
                        uint8_t payload[SPI_PKG_SIZE - 8];
                } __attribute__((__packed__)) dl_cont;
                struct {
                        uint8_t freq;
                        uint8_t payload[SPI_PKG_SIZE - 6];
                } __attribute__((__packed__)) set_spi_freq;
                struct {
                        uint8_t enable;
                        uint8_t payload[SPI_PKG_SIZE - 6];
                } __attribute__((__packed__)) uart_en;
                struct {
                        uint32_t addr;
                        uint8_t len;
                        uint8_t payload[SPI_PKG_SIZE - 10];
                } __attribute__((__packed__)) dump_mem;
                struct {
                        uint32_t addr;
                        uint32_t val;
                        uint8_t payload[SPI_PKG_SIZE - 13];
                } __attribute__((__packed__)) set_mem;
                struct {
                        uint8_t ins[SPI_PKG_SIZE - 5];
                } __attribute__((__packed__)) undoc_f1;
        }data;
} __attribute__((__packed__));

/*****************************************************************************/

static uint8_t spi_tx_buf[SPI_PKG_SIZE];
static struct master_frame_t *master_frame = (void *)spi_tx_buf;

/*****************************************************************************/

static FILE *file_output;

static int output_open(const char *filename)
{
	file_output = fopen(filename, "wb+");

	if (NULL == file_output) {
		CALLSTACK;
		return -1;
	}

	return 0;
}

static int output_close()
{
	int ret = 0;

	if (NULL == file_output) {
		CALLSTACK;
		return -1;
	}

	ret = fclose(file_output);

	return ret;
}

static int pkg_cont = 0;

static int transfer_image_to_big_edian(uint8_t *buffer, uint32_t length)
{
	uint8_t tmp0, tmp1, tmp2, tmp3;

	for (int i=0; i<length; i+=4) {
		tmp0 = buffer[i+0];
		tmp1 = buffer[i+1];
		tmp2 = buffer[i+2];
		tmp3 = buffer[i+3];
		buffer[i+0] = tmp3;
		buffer[i+1] = tmp2;
		buffer[i+2] = tmp1;
		buffer[i+3] = tmp0;
	}

	return 0;
}

static int output(uint8_t *buffer, uint32_t length)
{
	int ret;

	pkg_cont++;

	if (NULL == file_output) {
		CALLSTACK;
		return -1;
	}

	transfer_image_to_big_edian(buffer, length);

	ret = fwrite(buffer, sizeof(uint8_t), length, file_output);

	if (ret != length) {
		CALLSTACK;
		return -1;
	}

	return 0;
}

/*****************************************************************************/

static int packup_image(uint8_t *buffer, uint32_t length, uint32_t location)
{
	struct tc_sha256_state_struct state;

	// generate start download package
	memset(master_frame, 0, SPI_PKG_SIZE);
	master_frame->magic = MAGIC_NUM;
	master_frame->cmd = CMD_DL_START;
	master_frame->data.dl_start.img_len = length;
	master_frame->data.dl_start.img_loc = location;
	master_frame->data.dl_start.option = 0;
	master_frame->data.dl_start.option |= OPTION_DMA_EN;
	//master_frame->data.dl_start.option |= OPTION_DL_ONLY;
	//master_frame->data.dl_start.option |= OPTION_BYPASS_SHA_CHECK;
	
	tc_sha256_init(&state);
	tc_sha256_update(&state, buffer, length);
	tc_sha256_final(master_frame->data.dl_start.sha, &state);

	printf("\nSHA:\n");
	hex_dump(master_frame->data.dl_start.sha, 32);

        output(spi_tx_buf, SPI_PKG_SIZE);

	// packup image
	uint32_t remain = length;
	uint8_t *payload = buffer;

	while (remain) {
		int chunk = (remain > SPI_DL_PAYLOAD_SIZE) ? SPI_DL_PAYLOAD_SIZE : remain;
		int end_flag = (chunk == remain) ? 1 : 0;

		memset(master_frame, 0, SPI_PKG_SIZE);
		master_frame->magic = MAGIC_NUM;
		master_frame->cmd = CMD_DL_CONT;
		master_frame->data.dl_cont.img_type = 1;
		master_frame->data.dl_cont.len = chunk;
		master_frame->data.dl_cont.end_flag = end_flag;

		memcpy(master_frame->data.dl_cont.payload, payload, chunk);

		// write to output image
		output(spi_tx_buf, SPI_PKG_SIZE);

		remain -= chunk;
		payload += chunk;
	}

	printf("Length: %d \nLocation: %x\nPackages: %d\n", length, location, pkg_cont);

	return 0;
}

int format_image(const char *filename, uint32_t location)
{
	FILE *file = NULL;
	int ret;
	uint8_t *buffer;
	uint32_t length;

	file = fopen(filename, "rb");
	if (NULL == file) {
		CALLSTACK;
		return -1;
	}

	ret = fseek(file, 0, SEEK_END);

	length = ftell(file);

	ret = fseek(file, SEEK_SET, 0);

	buffer = (uint8_t *)malloc(length);
	if (NULL == buffer) {
		CALLSTACK;
		return -1;
	}

	ret = fread(buffer, sizeof(uint8_t), length, file);
	if (ret != length) {
		CALLSTACK;
		return -1;
	}

	ret = fclose(file);

	ret = packup_image(buffer, length, location);
	if (0 != ret) {
		CALLSTACK;
		return -1;
	}

	return 0;
}

/*****************************************************************************/

int format(const char *input, const uint32_t address, const char *output)
{
	int ret;

	ret = output_open(output);
	if (-1 == ret) {
		CALLSTACK;
		goto end;
	}

	ret = format_image(input, address);
	if (-1 == ret) {
		CALLSTACK;
		goto end;
	}

	ret = output_close();
	if (-1 == ret) {
		CALLSTACK;
		goto end;
	}

	ret = 0;
end:
	return ret;
}

