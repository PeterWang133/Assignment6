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

int thread_count = 1;   // Default number of threads
int active_threads = 0; // Track active threads
pthread_mutex_t thread_count_mutex = PTHREAD_MUTEX_INITIALIZER;

// Struct to pass arguments to each thread
struct thread_args {
    long *nums;
    long *target;
    int from;
    int to;
};

// Function to calculate time difference
double time_in_secs(const struct timeval *begin, const struct timeval *end) {
    long s = end->tv_sec - begin->tv_sec;
    long ms = end->tv_usec - begin->tv_usec;
    return s + ms * 1e-6;
}

// Print the given array of longs, an element per line
void print_long_array(const long *array, int count) {
    for (int i = 0; i < count; ++i) {
        printf("%ld\n", array[i]);
    }
}

// Merging two sorted halves
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

// Recursive threaded merge sort function
void *threaded_merge_sort(void *args);

void merge_sort_aux(long nums[], int from, int to, long target[]) {
    if (to - from <= 1) return; // Base case for recursion

    int mid = (from + to) / 2;

    // Create left and right thread arguments
    pthread_t left_thread, right_thread;
    struct thread_args left_args = {target, nums, from, mid};
    struct thread_args right_args = {target, nums, mid, to};

    int created_left = 0, created_right = 0;

    // Lock for updating active thread count safely
    pthread_mutex_lock(&thread_count_mutex);
    if (active_threads < thread_count) {
        // Create left thread
        active_threads++;
        created_left = pthread_create(&left_thread, NULL, threaded_merge_sort, &left_args) == 0;
    }
    if (active_threads < thread_count) {
        // Create right thread
        active_threads++;
        created_right = pthread_create(&right_thread, NULL, threaded_merge_sort, &right_args) == 0;
    }
    pthread_mutex_unlock(&thread_count_mutex);

    // If threads were created, wait for them
    if (created_left) pthread_join(left_thread, NULL);
    else merge_sort_aux(target, from, mid, nums);  // Fall back to sequential

    if (created_right) pthread_join(right_thread, NULL);
    else merge_sort_aux(target, mid, to, nums);  // Fall back to sequential

    // Merge sorted halves
    merge(nums, from, mid, to, target);

    // Decrement active thread count
    pthread_mutex_lock(&thread_count_mutex);
    if (created_left) active_threads--;
    if (created_right) active_threads--;
    pthread_mutex_unlock(&thread_count_mutex);
}

// Thread function to start recursive merge sort
void *threaded_merge_sort(void *args) {
    struct thread_args *data = (struct thread_args *)args;
    merge_sort_aux(data->nums, data->from, data->to, data->target);
    return NULL;
}

// Entry point for merge sort with threading
long *merge_sort(long nums[], int count) {
    long *result = calloc(count, sizeof(long));
    assert(result != NULL);

    memmove(result, nums, count * sizeof(long));  // Copy input array

    struct thread_args args = {nums, result, 0, count};
    merge_sort_aux(nums, 0, count, result);       // Start sorting

    return result;
}

// Function to allocate and load input array
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

    // Get number of threads from environment variable MSORT_THREADS
    if (getenv("MSORT_THREADS") != NULL)
        thread_count = atoi(getenv("MSORT_THREADS"));

    log("Running with %d thread(s). Reading input.\n", thread_count);

    // Read input array
    gettimeofday(&begin, 0);
    long *array = NULL;
    int count = allocate_load_array(argc, argv, &array);
    gettimeofday(&end, 0);

    log("Array read in %f seconds, beginning sort.\n", time_in_secs(&begin, &end));

    // Sort the array using multi-threaded merge sort
    gettimeofday(&begin, 0);
    long *result = merge_sort(array, count);
    gettimeofday(&end, 0);
  
    log("Sorting completed in %f seconds.\n", time_in_secs(&begin, &end));

    // Print the sorted result
    gettimeofday(&begin, 0);
    print_long_array(result, count);
    gettimeofday(&end, 0);
  
    log("Array printed in %f seconds.\n", time_in_secs(&begin, &end));

    free(array);
    free(result);

    return 0;
}
