CC = cc
CFLAGS = -Wall -Wextra
LDLIBS = -lreadline

all: biceps1 biceps2 biceps3_1 biceps3_2 biceps4_1 biceps4_2

biceps1: biceps1.c
	$(CC) $(CFLAGS) biceps1.c -o biceps1 $(LDLIBS)

biceps2: biceps2.c
	$(CC) $(CFLAGS) biceps2.c -o biceps2 $(LDLIBS)

biceps3_1: biceps3_1.c
	$(CC) $(CFLAGS) biceps3_1.c -o biceps3_1 $(LDLIBS)
	
biceps3_2: biceps3_2.c
	$(CC) $(CFLAGS) biceps3_2.c -o biceps3_2 $(LDLIBS) -DTRACE

biceps4_1: biceps4_1.c
	$(CC) $(CFLAGS) biceps4_1.c -o biceps4_1 $(LDLIBS) -DTRACE
	
biceps4_2: biceps4_2.c
	$(CC) $(CFLAGS) biceps4_2.c -o biceps4_2 $(LDLIBS) -DTRACE

clean:
	rm -f biceps1 biceps2 biceps3_1 biceps3_2 biceps4_1
