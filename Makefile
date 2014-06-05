CC=gcc
CFLAGS=-I.
OBJ=gdt_descriptor_decoder.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gdt_descriptor_decoder: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f gdt_descriptor_decoder *.o *~ *.out
