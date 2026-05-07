SCILAB=/usr/include/scilab

all:	sciclean.o

%.o: %.c
	$(CC) -c -O2 -I. $(addprefix -I,$(SCILAB)) $^

clean:
	$(RM) sciclean.o
