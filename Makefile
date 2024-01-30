CC := gcc
name := my-cat

.PHONY: all, clean

all: $(name)

$(name): $(name).c
	$(CC) -g -o $@ $^ -lm

clean:
	rm -rf $(name)
