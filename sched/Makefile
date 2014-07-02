.PHONY: clean all sched

LIB = libsched.o
SRC = sched.c
OBJ = $(SRC:.c=.o)
INC = $(patsubst %,-I../%,$(INCDIR))
CPFLAGS += -DNOHW_H

all: $(LIB)

$(LIB): $(OBJ)
	$(LD) -r $(OBJ) -o $@

%.o: %.c
	$(CC) -c $(CPFLAGS) -Wa,-ahlms=$(<:.c=.lst) -I. $(INC) $< -o $@

clean:
	-rm -f $(OBJ)
	-rm -f $(LIB)
	-rm -f $(OBJ:.o=.lst)

