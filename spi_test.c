/**
 * @file spi_test.c
 *
 * Simple spidev testing program.
 * @author Michael Welling <mwelling@ieee.org>
 *
 * Copyright (C) 2015 QWERTY Embedded Design
 *
 * Based on a similar utility written for EMAC Inc. SPI class.
 *
 * Copyright (C) 2009 EMAC, Inc.
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <asm/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <linux/spi/spidev.h>

#define TRUE 1
#define FALSE 0

#define EPRINT(args...) fprintf(stderr, ##args)

#define MAX_LENGTH 64

int file_des;
char is_initialized = FALSE;
__u8 miso[MAX_LENGTH];
__u8 mosi[MAX_LENGTH];

int setup_spi(char *device_file);
int transmit_spi(int length, char *mosi_str);
void close_spi(void);
__u8 char2hex(char value);

static __u8 bits = 8;
static __u32 speed = 800000;
static __u16 delay = 1;

int main(int argc, char **argv)
{
	if (argc < 4) {
		printf("USAGE: %s device length mosi\n", argv[0]);
		printf("device: name of the spi device node\n"
		       "length: length of spi transaction(bytes)\n"
		       "mosi:   hex value to be transmitted\n\n"
		       "EX: %s /dev/spidev0.0 4 12AB\n\n"
		       "Note: mosi will be padded or truncated\n"
		       "      to the length speficied.\n"
		       "      %d bytes maximum length.\n", argv[0], MAX_LENGTH);
		return -1;
	}

	if (setup_spi(argv[1]) == -1)
		exit(EXIT_FAILURE);

	transmit_spi(atoi(argv[2]), argv[3]);

	close_spi();

	exit(EXIT_SUCCESS);
}

int setup_spi(char *device_file)
{
	int ret;

	if (is_initialized)
		return 0;

	file_des = open(device_file, O_RDWR);

	if (file_des == -1) {
		EPRINT("setup_spi: error opening device file: %s: %s\n",
		       device_file, strerror(errno));
		return -1;
	}

	ret = ioctl(file_des, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		EPRINT("setup_spi: Cannot set bits per word");
		return -1;
	}

	ret = ioctl(file_des, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		EPRINT("setup_spi: Cannot set max speed hz");
		return -1;
	}

	is_initialized = TRUE;
	return 0;
}

int transmit_spi(int length, char *mosi_str)
{
	__u8 nibble;
	int i;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)mosi,
		.rx_buf = (unsigned long)miso,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (!is_initialized)
		return -1;

	tr.len = (length < MAX_LENGTH) ? length : MAX_LENGTH;

	for (i = 0; i < (length * 2); i++) {
		if (i < strlen(mosi_str))
			nibble = char2hex(mosi_str[i]);
		else
			nibble = 0xf;

		mosi[i / 2] =
			((i % 2) == 0) ? nibble : ((mosi[i / 2] << 4) | nibble);
	}

	if (ioctl(file_des, SPI_IOC_MESSAGE(1), &tr) < 1) {
		EPRINT("spi_transmit: error writing device\n");
		return -1;
	}

	printf("MOSI  MISO\n");

	for (i = 0; i < length; i++)
		printf("%.2X  : %.2X\n", mosi[i], miso[i]);

	return 0;
}

void close_spi(void)
{
	if (!is_initialized)
		EPRINT("close_spi: call to close before initialization\n");
	else {
		close(file_des);
		is_initialized = FALSE;
	}
}

__u8 char2hex(char value)
{
	__u8 retval;

	if (value >= '0' && value <= '9')
		retval = value - '0';
	else if (value >= 'A' && value <= 'F')
		retval = value - 'A' + 0xA;
	else if (value >= 'a' && value <= 'f')
		retval = value - 'a' + 0xA;
	else
		retval = 0xF;

	return retval;
}
