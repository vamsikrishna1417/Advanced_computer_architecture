CC = gcc

C_FLAGS = -g -Wall -O0

APP1 = linear_write_mountain_tester
APP2 = linear_read_mountain_tester
APP3 = random_write_mountain_tester
APP4 = random_read_mountain_tester

obj-m+= linear_write_memory_mountain.o linear_read_memory_mountain.o random_write_memory_mountain.o random_read_memory_mountain.o
file1 = linear_write_memory_mountain
file2 = linear_read_memory_mountain
file3 = random_write_memory_mountain
file4 = random_read_memory_mountain

all:
	$(CC) $(C_FLAGS) -o $(APP1) $(file1).c
	$(CC) $(C_FLAGS) -o $(APP2) $(file2).c
	$(CC) $(C_FLAGS) -o $(APP3) $(file3).c
	$(CC) $(C_FLAGS) -o $(APP4) $(file4).c

clean:
	rm -f *.o
