
#include <malloc.h>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
 
#include <xmmintrin.h>
#include <immintrin.h>

 
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <sched.h>
#include <unistd.h>

#include <iomanip>

static const int N = 4
static const int M = 3
static const int K = 3

#define RDTSC(X) asm volatile ("rdtsc; shlq $32, %%rdx; orq %%rdx,%%rax" : "=a" (X) :: "%rdx")

using namespace std;

int main(){

    return 0;
}
