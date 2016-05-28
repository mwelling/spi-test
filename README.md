# spi-test
Utility for accessing standard Linux spidev devices from userspace.

```
USAGE: ./spi_test device length mosi
	device: name of the spi device node
	length: length of spi transaction(bytes)
	mosi:   hex value to be transmitted
```

Example
```
./spi_test /dev/spidev0.0 4 12AB
```

