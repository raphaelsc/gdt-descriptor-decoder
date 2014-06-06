CC=gcc
CFLAGS=-I.
OBJ=gdt_descriptor_decoder.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gdt-dd: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f gdt-dd *.o *~ *.out
