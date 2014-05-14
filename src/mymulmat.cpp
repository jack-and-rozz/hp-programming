#include "mymulmat.h"
#include <iostream>

#include <stdlib.h>
#include <malloc.h>

#include <immintrin.h>
#include <xmmintrin.h>


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
    //*la = k+1; *lb = m; *lc = m;
    *la = k +  (7 - (k-1)%8); 
    *lb = m +  (7 - (m-1)%8); 
    *lc = m +  (7 - (m-1)%8);
    cout << "k=" << k << endl; 
    cout << "m=" << m << endl; 
    cout << "la=" << *la << endl; 
    cout << "lb=" << *lb << endl; 
    /*
    *A = new float[n*k]();
    *B = new float[k*m]();
    *C = new float[n*m]();
    */


    *A = (float*)_mm_malloc( sizeof(float) * (*lb)*(*la), 32);
    *B = (float*)_mm_malloc( sizeof(float) * (*la)*m, 32);
    *C = (float*)_mm_malloc( sizeof(float) * n*(*lb), 32);

    cout << "   a % 32 = " << (long)(*A)%32 <<endl;
    cout << "   b % 32 = " << (long)(*B)%32 <<endl;
    cout << "   c % 32 = " << (long)(*C)%32 <<endl;

    this->n = n; this->m = *lb; this->k = *la;
    this->A = *A; this->B = *B; this->C = *C;
    return;
}

void MyMulMat::multiply()
{
    std::cout << "mymul multiply" << std::endl;

    float* tB = transpose(B,k,m);
    __m256 VA,VB,VC;
    
    /*
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int l = 0; l < k; l++ ) {
	      C[i*m+j] += A[i*k+l] * B[l*m+j];
	    }
        }
    }

    */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int l = 0; l < k; l+=8 ) {
	      VA = _mm256_load_ps(&A[i*k +l]);
	      VB = _mm256_load_ps(&tB[j*k+l]);
	      VC = _mm256_load_ps(&C[i*m +j]);
	      VC = _mm256_mul_ps(VA,VB);
	      _mm256_store_ps((float*)&C[i*m+j],VC);
	    }
        }
    }
    return;
}
