#ifndef COMMON
#define COMMON

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stack>
#include <list>
#include <string>
#include <cstring>
#include <unordered_map>
#include <time.h>
#include <math.h>

#define NUM_FRAMES 256
#define NUM_PAGES 256
#define TLB_NUM_ENTRIES 16
#define PAGE_FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE 65536
// PHYSICAL_MEMORY_SIZE = NUM_FRAMES*PAGE_FRAME_SIZE = 256*256;

#define TLB_MODE 0 // 0:FIFO, 1:LRU


using namespace std;


static int leastSignificantByte(unsigned int n, int nth=1) {
    return (n >> (8 * (nth - 1))) & 0xFF;
}

// http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
static timespec diff(timespec start, timespec end) {
    timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}        

#endif // COMMON
