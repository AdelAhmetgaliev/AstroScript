CC = clang
LL = clang

TARGET = bin/AstroScript

ODIR = obj
SDIR = src
IDIR = $(SDIR)/inc

CFLAGS = -O3 -Wall -Wextra -I$(IDIR) -DLOGGER -fstack-protector-all -fpie
LFLAGS = -fuse-ld=lld -flto -lm -s -Wl,-z,relro,-z,now

_HEADS = logger.h reader.h processing.h interpolation.h datetime.h config.h
HEADS = $(patsubst %,$(IDIR)/%,$(_HEADS))

_OBJS = main.o reader.o processing.o interpolation.o datetime.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c $(HEADS) bin $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

bin:
	mkdir $@

$(ODIR):
	mkdir $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
