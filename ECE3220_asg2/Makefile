### how to use
# 
#
# make   	-- compiles
# make f 	-- runs fifo option with in1 and out1
# make s 	-- runs shortest job first (in1 and out1)
# make r 	-- runs round robin (in1 and out1)

# make f2   -- fifo (in2 and out2)
# make s2   -- shortest job first (in2 and out2)
# make r2   -- round robin (in2 and out2)




CC		:= gcc
FILE	:= asg2.c
CFLAGS	:= -g
RUN		:= ./a.out
VALGR	:= valgrind --leak-check=full

FILE_R1	:= in1
FILE_W1 := out1

FILE_R2	:= in2
FILE_W2 := out2

F  		:= -fifo
S		:= -sjf
R 		:= -rr


make:
	$(CC) $(FILE) $(CFLAGS)

f: make
	$(RUN) $(F) $(FILE_R1) $(FILE_W1)

s: make
	$(RUN) $(S) $(FILE_R1) $(FILE_W1)

r: make
	$(RUN) $(R) $(FILE_R1) $(FILE_W1)


# These are for in2 and out2
f2: make
	$(RUN) $(F) $(FILE_R2) $(FILE_W2)

s2: make
	$(RUN) $(S) $(FILE_R2) $(FILE_W2)

r2: make
	$(RUN) $(R) $(FILE_R2) $(FILE_W2)


mem: make
	valgrind --leak-check=full $(RUN) $(F) $(FILE_R1) $(FILE_W1)
