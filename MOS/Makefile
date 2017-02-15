PATH=/home/auj/git/cc65/bin:$PATH

all:	mos.bin

%.o:	%.asm
	ca65 --target none -l $*.lst -o $@ $^

%.bin:	%.o
	ld65 --target none -o $@ $^

