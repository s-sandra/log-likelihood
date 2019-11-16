
FLAGS = -Wall -Werror -pthread -O -lm

all: log_likelihood

clean:
	rm *.o log_likelihood

log_likelihood: log_likelihood.c threads.h hashtable.h rb_tree
	gcc -o log_likelihood log_likelihood.c $(FLAGS)

rb_tree: rb_tree.c rb_tree.h
	gcc -c rb_tree.c