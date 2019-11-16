
FLAGS = -Wall -Werror -pthread -O -lm

all: log_likelihood

clean:
	rm log_likelihood

log_likelihood: log_likelihood.c
	gcc -DHASH_FUNCTION=HASH_OAT -o log_likelihood log_likelihood.c $(FLAGS)