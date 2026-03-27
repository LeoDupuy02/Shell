CC = cc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lreadline
EXEC = biceps

all: $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC) : biceps.o gescom.o 
	$(CC) $(CFLAGS) -o biceps biceps.o gescom.o $(LDLIBS) -DTRACE	

clean:
	rm -f biceps biceps.o gescom.o
