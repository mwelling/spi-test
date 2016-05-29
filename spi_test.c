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
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <linux/spi/spidev.h>

#define MAX_LENGTH 64

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

void string2hex(char *str, __u8 *hex, int length)
{
	int i;
	__u8 nibble;

	for (i = 0; i < (length * 2); i++) {
		if (i < strlen(str))
			nibble = char2hex(str[i]);
		else
			nibble = 0xf;
		if (i % 2 == 0)
			hex[i / 2] = nibble;
		else
			hex[i / 2] = (hex[i / 2] << 4) | nibble;
	}
}

void print_spi_transaction(__u8 *miso, __u8 *mosi, __u32 length)
{
	int i;

	printf("MOSI  MISO\n");
	for (i = 0; i < length; i++)
		printf("%.2X  : %.2X\n", mosi[i], miso[i]);
}

int main(int argc, char *argv[])
{
	__u8 bits = 8;
	__u32 speed = 800000;
	__u16 delay = 1;
	__u8 miso[MAX_LENGTH];
	__u8 mosi[MAX_LENGTH];
	__u32 length;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)mosi,
		.rx_buf = (unsigned long)miso,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};
	int fd;
	int ret;

	if (argc < 4) {
		printf("USAGE: %s device length mosi\n", argv[0]);
		printf("      device: name of the spi device node\n"
		       "      length: length of spi transaction(bytes)\n"
		       "      mosi:   hex value to be transmitted\n\n"
		       "EX: %s /dev/spidev0.0 4 12AB\n\n"
		       "Note: mosi will be padded or truncated\n"
		       "      to the length speficied.\n"
		       "      %d bytes maximum length.\n", argv[0], MAX_LENGTH);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "main: opening device file: %s: %s\n",
		       argv[1], strerror(errno));
		return -1;
	}

	tr.len = MIN(atoi(argv[2]), MAX_LENGTH);
	string2hex(argv[3], mosi, tr.len);

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret == -1)
		fprintf(stderr, "main: ioctl SPI_IOC_MESSAGE: %s\n",
			strerror(errno));
	else
		print_spi_transaction(miso, mosi, tr.len);

	close(fd);

	return ret;
}
