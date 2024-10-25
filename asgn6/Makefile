CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lcurl

SRC_FILES = pagedir.c hashtable.c curl.c crawler.c url.c bag.c
OBJ_FILES = $(SRC_FILES:.c=.o)

crawler_executable: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) crawler_executable

