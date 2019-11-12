
FLAGS = -Wall -Werror -pthread -O -lm

all: log_likelihood

clean:
	rm *.o log_likelihood

log_likelihood: log_likelihood.c threads.h hashtable.h
	gcc -o log_likelihood log_likelihood.c $(FLAGS)