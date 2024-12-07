CC = clang
LL = clang

TARGET = bin/AstroScript

BDIR = build
ODIR = $(BDIR)/obj
SDIR = src
IDIR = $(SDIR)/inc

CFLAGS = -O3 -Wall -Wextra -I$(IDIR) -fstack-protector-all -fpie -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3
LFLAGS = -fuse-ld=lld -flto -lm -s -pie -Wl,-z,nodlopen -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now

_HEADS = logger.h reader.h processing.h interpolation.h datetime.h config.h
HEADS = $(patsubst %,$(IDIR)/%,$(_HEADS))

_OBJS = main.o reader.o processing.o interpolation.o datetime.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(BDIR)/$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c $(HEADS) $(BDIR) $(BDIR)/bin $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/bin:
	mkdir $@

$(ODIR):
	mkdir $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(BDIR)/$(TARGET)
