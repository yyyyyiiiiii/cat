CC := gcc
name := my-cat
src := $(wildcard *.c) $(wildcard *.h)

.PHONY: all, clean

all: $(name)

$(name): $(src)
	$(CC) -g -o $@ $^ -lm

clean:
	rm -rf $(name)
