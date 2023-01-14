CC		:= gcc
CFLAGS	:= -Wall -g
T1		:= btrfs_1
T2 		:= btrfs_2
D1		:= btrfs_driver1
D2		:= btrfs_driver2
FILE	:= asg3


make: btrfs.h
	rm -f $(T1).o $(T2).o $(T3).o $(FILE)
	$(CC) $(CFLAGS) -c $(T1).c
	$(CC) $(CFLAGS) -c $(T2).c
	$(CC) $(CFLAGS) -c $(D1).c
	$(CC) $(CFLAGS) $(T1).o $(T2).o $(D1).o -o $(FILE)

run: make
	./$(FILE)

clean : 
	rm -f *.o asg3 a.out