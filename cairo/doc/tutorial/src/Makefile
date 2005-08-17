MYCFLAGS=$(shell pkg-config --cflags --libs cairo gtk+-2.0) -Wall -g -Wpointer-arith -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing

# If you don't want to/can't compile all of these targets, then trim
# this list.
all: gtk xlib pdf png

GTK_EXAMPLES=$(patsubst %.c,%-gtk,$(wildcard *.c))
gtk: $(GTK_EXAMPLES)
%-gtk:%.c cairo-tutorial-gtk.h
	$(CC) -DCAIRO_TUTORIAL_GTK $(CFLAGS) $(MYCFLAGS) -o $@ $<

XLIB_EXAMPLES=$(patsubst %.c,%-xlib,$(wildcard *.c))
xlib: $(XLIB_EXAMPLES)
%-xlib:%.c cairo-tutorial-xlib.h
	$(CC) -DCAIRO_TUTORIAL_XLIB $(CFLAGS) $(MYCFLAGS) -o $@ $<

PDF_EXAMPLES=$(patsubst %.c,%-pdf,$(wildcard *.c))
pdf: $(PDF_EXAMPLES)
%-pdf:%.c cairo-tutorial-pdf.h
	$(CC) -DCAIRO_TUTORIAL_PDF $(CFLAGS) $(MYCFLAGS) -o $@ $<

PNG_EXAMPLES=$(patsubst %.c,%-png,$(wildcard *.c))
png: $(PNG_EXAMPLES)
%-png:%.c cairo-tutorial-png.h
	$(CC) -DCAIRO_TUTORIAL_PNG $(CFLAGS) $(MYCFLAGS) -o $@ $<

clean:
	rm -f $(GTK_EXAMPLES) $(XLIB_EXAMPLES) $(PDF_EXAMPLES) $(PNG_EXAMPLES)

.PHONY: all gtk xlib pdf png clean
