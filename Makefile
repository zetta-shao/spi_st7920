FILES = app.o st7920.o swspi_port_ch341_linux.o swspi.o lcd_font8.o lcd_fonts.o 

CFLAGS = -O2 -Wall -fomit-frame-pointer

all: $(FILES)
	$(CC) $(FILES) -lpthread -o app

clean:
	rm -f $(FILES) app *~ core
