# Log Likelihood Calculator
Log likelihood is a common statistic used to compare two texts, essentially allowing us to determine whether the observed differences in 
word frequencies are statistically significant. This multithreaded C program tallies all unique words across two corpuses, and calculates 
the frequency and log likelihood ratio of each word occurring in both texts. Ratios are outputted in descending order, with most 
statistically significant words listed first.

## Implementation

### Counting Words
I parallelized this task, assigning an equal amount of threads to process each text. The number of threads chosen
depends on the number of configured processors divided by two. If that number is less than 0, then each text is given 
one thread. Threads are placed in an array, and created in the body of a for-loop. I wrote wrapper functions for each 
of the POSIX thread functions, found in `threads.h`.

The threads then take turns reading in words from their respective texts. I separated words by whitespace and punctuation, 
with case sensitivity. I also considered special UTF-8 characters, such as curly quotations and em dashes. To make file
access more efficient, each text is loaded into virtual memory using `mmap()`. I used a mutex lock to ensure that each 
thread finished reading an entire word without getting interrupted. 

Words are stored in a shared hashtable, which contains separate word count variables for each of the texts, as well 
as the log likelihood. To create the hashtable, I used the [`uthash`](https://troydhanson.github.io/uthash/) 
external library implemented by Troy D. Hanson, and created wrapper functions for it in `hashtable.h`. The library offers 
multiple hash functions. I experimented with several and found that the one-at-a-time (OAT) method had the best performance.

### Calculating Log Likelihood
After computing the word counts, I created the same number of threads as configured processors, which independently calculate 
the log likelihood of words that occur more than 35 times in the hashtable, given that the text is long enough. The process 
is similar to the word count computation, using mutual exclusion. Each thread then inserts its computed log likelihood in a 
red-black binary search tree, which is more optimal because it is self-balancing. To create the BST, I used an 
[external library]( https://github.com/mirek/rb_tree) implemented by Mirek Rusin.

### Sorting
Once all of the log likelihoods are computed, the program traverses the BST in descending order and prints out the node values.
