TARGET=spi_test
CFILES=spi_test.c

OBJS=$(CFILES:.c=.o)
DEPS=$(OBJS:.o=.d)


all: $(TARGET)

$(TARGET): $(OBJS) Makefile
	$(CC) $(VERBOSE) $(OBJS) $(OFLAGS) $(LIBFLAGS) $(SLIBS) -o $@

%.o: %.c
	$(CC) $(VERBOSE) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) *.o *.gdb $(TARGET) $(DEPS)

-include $(DEPS)
