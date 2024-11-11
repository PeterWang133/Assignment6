/**
 * Multi-threaded Merge Sort
 *
 * A multi-threaded implementation of merge sort that uses POSIX threads to
 * recursively divide and sort a large array of integers. The number of threads
 * used can be set through an environment variable MSORT_THREADS.
 *
 * This program reads an array of integers from standard input, sorts them in
 * ascending order using merge sort, and outputs the sorted array to standard output.
 *
 * Assumptions:
 * - The program takes one argument, an integer -> represents number of elements
 * - The input is a series of integers separated by whitespace.
 * - The environment variable MSORT_THREADS specifies the max number of threads.
 * - Uses up to thread_count threads to divide and conquer the sorting task.
 *
 * 2 Global Variables:
 * - thread_count sets the total number of threads allowed.
 * - active_threads keeps track of currently active threads.
 *
 * Compilation: gcc -pthread -o tmsort tmsort.c
 * Usage: ./tmsort <num_elements> -> ./tmsort 50000
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#define tty_printf(...) (isatty(1) && isatty(0) ? printf(__VA_ARGS__) : 0)

#ifndef SHUSH
#define log(...) (fprintf(stderr, __VA_ARGS__))
#else 
#define log(...)
#endif

#define MIN_THREAD_SIZE 10000000  // Further increase segment size for thread creation

int thread_count = 1;   // Default number of threads

// Thread pool to manage threads
pthread_t *thread_pool;
int pool_size = 0;
pthread_mutex_t pool_lock = PTHREAD_MUTEX_INITIALIZER;

// Struct to pass arguments to each thread
struct thread_args {
    long *nums;       // Array of integers to sort
    long *target;     // Target array to store sorted results
    int from;         // Starting index for sorting
    int to;           // Ending index for sorting 
};

/**
 * Function to calculate time difference
 * @param begin: Start time.
 * @param end: End time.
 * @return Time difference in seconds as a double.
 */
double time_in_secs(const struct timeval *begin, const struct timeval *end) {
    long s = end->tv_sec - begin->tv_sec;
    long ms = end->tv_usec - begin->tv_usec;
    return s + ms * 1e-6;
}

/**
 * Print the given array of longs, an element per line
 * @param array: The array of integers.
 * @param count: Number of elements in the array.
 */
void print_long_array(const long *array, int count) {
    for (int i = 0; i < count; ++i) {
        printf("%ld\n", array[i]);
    }
}

/**
 * Merging two sorted halves
 * @param nums: Source array containing sorted halves.
 * @param from: Starting index of the first half.
 * @param mid: Middle index, starting point of the second half.
 * @param to: Ending index of the second half.
 * @param target: Target array to store merged result.
 */
void merge(long nums[], int from, int mid, int to, long target[]) {
    int left = from;
    int right = mid;
    int i = from;

    while (left < mid && right < to) {
        if (nums[left] <= nums[right]) {
            target[i++] = nums[left++];
        } else {
            target[i++] = nums[right++];
        }
    }

    while (left < mid) target[i++] = nums[left++];
    while (right < to) target[i++] = nums[right++];
}

/**
 * Recursive threaded merge sort function
 * splits the array and assigns sorting tasks to threads when below the threshold.
 * @param args: Arguments struct for passing data to thread function.
 */
void *threaded_merge_sort(void *args);

/**
 * Recursively sorts an array of integers using merge sort and multi-threading.
 * Divides the array and creates threads for each half as allowed.
 * 
 * @param nums: Source array to sort.
 * @param from: Starting index of the array section.
 * @param to: Ending index of the array section.
 * @param target: Target array to store the sorted result.
 */
void merge_sort_aux(long nums[], int from, int to, long target[]) {
    if (to - from <= 1) return; // Base case for recursion

    int mid = (from + to) / 2;

    pthread_t left_thread, right_thread;
    struct thread_args left_args = {target, nums, from, mid};
    struct thread_args right_args = {target, nums, mid, to};

    int created_left = 0, created_right = 0;
    
    if (pool_size < thread_count && (mid - from) >= MIN_THREAD_SIZE) {
        pthread_mutex_lock(&pool_lock);
        created_left = pthread_create(&left_thread, NULL, threaded_merge_sort, &left_args) == 0;
        if (created_left) thread_pool[pool_size++] = left_thread;
        pthread_mutex_unlock(&pool_lock);
    }
    if (pool_size < thread_count && (to - mid) >= MIN_THREAD_SIZE) {
        pthread_mutex_lock(&pool_lock);
        created_right = pthread_create(&right_thread, NULL, threaded_merge_sort, &right_args) == 0;
        if (created_right) thread_pool[pool_size++] = right_thread;
        pthread_mutex_unlock(&pool_lock);
    }

    if (created_left) pthread_join(left_thread, NULL);
    else merge_sort_aux(target, from, mid, nums);

    if (created_right) pthread_join(right_thread, NULL);
    else merge_sort_aux(target, mid, to, nums);

    merge(nums, from, mid, to, target);
}

/**
 * Thread function to start recursive merge sort
 * @param args: Pointer to thread_args struct.
 */
void *threaded_merge_sort(void *args) {
    struct thread_args *data = (struct thread_args *)args;
    merge_sort_aux(data->nums, data->from, data->to, data->target);
    return NULL;
}

/**
 * Entry point for merge sort with threading
 * @param nums: Array to be sorted.
 * @param count: Number of elements in the array.
 * @return Pointer to a sorted array.
 */ 
long *merge_sort(long nums[], int count) {
    long *result = calloc(count, sizeof(long));
    assert(result != NULL);

    memmove(result, nums, count * sizeof(long));

    pthread_mutex_lock(&pool_lock);
    thread_pool = malloc(thread_count * sizeof(pthread_t));
    pool_size = 0;
    pthread_mutex_unlock(&pool_lock);

    struct thread_args args = {nums, result, 0, count};
    merge_sort_aux(nums, 0, count, result);

    free(thread_pool);
    return result;
}

/**
 * Function to allocate and load input array
 * @param argc: Argument count (should be > 1).
 * @param argv: Argument vector.
 * @param array: Pointer to store loaded array.
 * @return Number of elements read.
 */
int allocate_load_array(int argc, char **argv, long **array) {
    assert(argc > 1);
    int count = atoi(argv[1]);

    *array = calloc(count, sizeof(long));
    assert(*array != NULL);

    long element;
    tty_printf("Enter %d elements, separated by whitespace\n", count);
    int i = 0;
    while (i < count && scanf("%ld", &element) != EOF) {
        (*array)[i++] = element;
    }

    return count;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    struct timeval begin, end;

    if (getenv("MSORT_THREADS") != NULL)
        thread_count = atoi(getenv("MSORT_THREADS"));

    log("Running with %d thread(s). Reading input.\n", thread_count);

    gettimeofday(&begin, 0);
    long *array = NULL;
    int count = allocate_load_array(argc, argv, &array);
    gettimeofday(&end, 0);

    log("Array read in %f seconds, beginning sort.\n", time_in_secs(&begin, &end));

    gettimeofday(&begin, 0);
    long *result = merge_sort(array, count);
    gettimeofday(&end, 0);
  
    log("Sorting completed in %f seconds.\n", time_in_secs(&begin, &end));

    gettimeofday(&begin, 0);
    print_long_array(result, count);
    gettimeofday(&end, 0);
  
    log("Array printed in %f seconds.\n", time_in_secs(&begin, &end));

    free(array);
    free(result);

    return 0;
}
