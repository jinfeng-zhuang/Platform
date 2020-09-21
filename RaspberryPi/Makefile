TARGET = spi_client

CFLAGS = -Wall -I ./

OBJS = hexdump.o spi.o client.o

.PHONY: all clean $(TARGET)

all: $(TARGET)

spi_client: $(OBJS)
	gcc $^ -o $@

%.o: %.c *.h
	gcc $(CFLAGS) -c $?

clean:
	rm *.o *.bin $(TARGET) -f
