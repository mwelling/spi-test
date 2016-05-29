# spi-test
Utility for accessing standard Linux spidev devices from userspace.

```
USAGE: ./spi_test -d dev -l len -m mosi
	-d,--device dev: name of the spi device node
	-l,--length len: length of spi transaction(bytes)
	-m,--mosi mosi: hex value to be transmitted
```

Example
```
./spi_test -d /dev/spidev0.0 -l 4 -m 12AB
```

