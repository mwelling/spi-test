# spi-test
Utility for accessing standard Linux spidev devices from userspace.

```
USAGE: ./spi_test -d dev -l len -m mosi -s speed
	-d,--device dev: name of the spi device node
	-l,--length len: length of spi transaction(bytes)
	-m,--mosi mosi: hex value to be transmitted
	-s,--speed speed: speed of the transaction in Hz
```

Example
```
./spi_test -d /dev/spidev0.0 -l 4 -m 12AB
```

