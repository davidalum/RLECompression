TARGETS = compressR_LOLS compressR_worker_LOLS

CC = gcc 
FLAGS = -Wall 

all: clean $(TARGETS) compressT_LOLS 

$(TARGETS):
	$(CC) $(FLAGS) $@.c -o $@ -std=c99

compressT_LOLS:
	$(CC) $(FLAGS) compressT_LOLS.c -o compressT_LOLS -lpthread -std=c99

clean:
	rm -f $(TARGETS) compressT_LOLS
