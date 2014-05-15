#include "mymulmat.h"
#include <iostream>

#include <stdlib.h>
#include <malloc.h>

#include <immintrin.h>
#include <xmmintrin.h>

#include <omp.h>

using namespace std;

MyMulMat::MyMulMat()
{
    std::cout << "mymul constructed" << std::endl;
}

MyMulMat::~MyMulMat()
{
  _mm_free(A);
  _mm_free(B);
  _mm_free(C);
  _mm_free(tB);
  _mm_free(tmp);
  std::cout << "mymul destructed" << std::endl;
}

using std::cout;
using std::endl;

float* MyMulMat::transpose(float *M,int row,int col){
  float *tM = (float*)_mm_malloc( sizeof(float) * row * col, 32);
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      tM[j*row+i] = M[i*col+j];
    } 
  }
  return tM;
}

void MyMulMat::init(int n, int m, int k,
          int *la, int *lb, int *lc,
          float **A, float **B, float **C)
{
    std::cout << "mymul init" << std::endl;
    /* 横の長さを8の倍数に*/
    *la = k +  (7 - (k-1)%8); 
    *lb = m +  (7 - (m-1)%8); 
    *lc = m +  (7 - (m-1)%8);
    //cout << "la=" << *la <<endl;
    //cout << "lb=" << *lb <<endl;
    
    *A = (float*)_mm_malloc( sizeof(float) * n*(*la), 32);
    *B = (float*)_mm_malloc( sizeof(float) * k*(*lb), 32);
    *C = (float*)_mm_malloc( sizeof(float) * n*(*lc), 32);
    int i,j =0;

    /* 使わない所0埋め。これ意味あるのかな...*/
     for (i = 0; i < n; i++) {
        for (j = k-1; j < *la; j++) {
	  (*A)[i*k+j] = 0;
	}
     }
     for (i = 0; i < k; i++) {
        for (j = m-1; j < *lb; j++) {
	  (*B)[i*m+j] = 0;
	}
     }
     for (i = 0; i < n; i++) {
        for (j = m-1; j < *lc; j++) {
	  (*C)[i*m+j] = 0;
	}
     }
    

    cout << "   a % 32 = " << (long)(*A)%32 <<endl;
    cout << "   b % 32 = " << (long)(*B)%32 <<endl;
    cout << "   c % 32 = " << (long)(*C)%32 <<endl;
    this->tB = tB;
    this->tmp = (float*)_mm_malloc( sizeof(float) * 8, 32);
    
    this->n = n; this->m = m; this->k = k;
    this->A = *A; this->B = *B; this->C = *C;
    return;
}

void MyMulMat::multiply()
{
  
    std::cout << "mymul multiply" << std::endl;
    tB = transpose(B,k,m);
    __m256* VA = (__m256*)A;
    __m256* VB = (__m256*)tB;
    //__m256* VC = (__m256*)C;
  
    // 200_200_200 でtransposeに0.452ms 計算に26.294ms　transposeのオーバーヘッドは気にしなくてよさそう
    
    int i,j,l = 0;
    int k2 = k/8;
    {
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            for (l = 0; l < k2; l+=1 ) {
	      //mul_psまでで9.651 [ms]
	      //store までで15.402 [ms]
	      //Cに代入するので20.355 [ms] 0.786048 [GFLOPS]
	      _mm256_mul_ps(VA[i*k2+l],VB[j*k2+l]);
	      _mm256_store_ps(tmp,_mm256_mul_ps(VA[i*k2+l],VB[j*k2+l])); 
	      C[i*m+j] += tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] ;
	      //ここの処理をstore_psで出来ないかな
	    }
        }
      }
    }
    /*
     for (i = 0; i < n; i++) {
       for (j = 0; j < m; j+=8) {
	 for (l = 0; l < k; l+=8 ) {
	   //Cも8つ同時に扱えないか？
	   _mm256_add_ps(VC[i*m+j], _mm256_mul_ps(VA[],VB[]));
	 }
       }
     }
    */
    return;
}
