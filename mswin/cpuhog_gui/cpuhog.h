#pragma once 

#define MAX_THREADS 32

bool start_cpu_hog(int threads, int seconds);

bool cpu_hog_is_end();

bool done_cpu_hog();

