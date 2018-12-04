LDFLAGS = -lopengl32 -lglu32 -lfreeglut
CFLAGS=-g -Wall -std=c++11
CC=g++
EXEEXT=
RM=rm
PROGRAM_NAME = unfinished
run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEEXT)
$(PROGRAM_NAME): unfinished.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
clean:
	$(RM) *.o $(PROGRAM_NAME)$(EXEEXT)
