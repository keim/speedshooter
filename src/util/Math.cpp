#include "Math.h"

//--------------------------------------------------------------------------------
// Log2
//--------------------------------------------------------------------------------
// i 以上で最小の 2^n を計算する
int Calc2N(int i)
{
	int n;
    for (n=1; n<i; n<<=1);
    return n;
}

//  i >= 2^n を満たす最大の n 求める(ただし，i=0→n=-1)
int CalcLog2(int i)
{
	int n;
    for (n=-1; i!=0; i>>=1, n++);
    return n;
}

//--------------------------------------------------------------------------------
// Mersenne Twister 乱数
//--------------------------------------------------------------------------------
// Period parameters
#define MTN 624
#define MTM 397
#define MATRIX_A 0x9908b0df     // constant vector a
#define UPPER_MASK 0x80000000   // most significant w-r bits
#define LOWER_MASK 0x7fffffff   // least significant r bits

// Tempering parameters
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

// Private values
static int      mti;                // index number
static DWORD    mt[MTN + 1];        // the array for the state vector
static DWORD    mtr[MTN];           // the array for the random number
static DWORD    bInitialized = 0;   // initializing flag

// generator
static void generateMT()
{
    int kk;
    unsigned long y;
    static unsigned long mag01[2] = {0x0, MATRIX_A}; /* mag01[x] = x * MATRIX_A  for x=0,1 */
    
    for(kk = 0; kk < MTN - MTM; kk++){
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + MTM] ^ (y >> 1) ^ mag01[y & 0x1];
    }

    mt[MTN] = mt[0];

    for(; kk < MTN; kk++){
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + (MTM - MTN)] ^ (y >> 1) ^ mag01[y & 0x1];
    }

    for(kk = 0; kk < MTN; kk++){
        y = mt[kk];
        y ^= TEMPERING_SHIFT_U(y);
        y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
        y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
        y ^= TEMPERING_SHIFT_L(y);
        mtr[kk] = y;
    }
    mti = 0;
}

// 乱数種を設定する
void srandMT(unsigned long seed)
{
    int i;

    for(i = 0; i < MTN; i++){
         mt[i] = seed & 0xffff0000;
         seed = 69069 * seed + 1;
         mt[i] |= (seed & 0xffff0000) >> 16;
         seed = 69069 * seed + 1;
    }
    bInitialized = 1;
    generateMT();
}

// 乱数を取り出す
DWORD randMT()
{
    if (mti >= MTN) {
        if (!bInitialized) srandMT(4357);
        generateMT();
    }
    return mtr[mti++];
}

// 乱数を取り出す
float randMTf()
{
    return (randMT() & 0xffffff) * 0.00000005960465f;
}

// フーリエ変換
void CalcDCT(int iTableSize, float* afTable, float* afCosTable)
{
    int   i, j, k, mh, m, irev;
    float v;

    // データ反転
    i = 0;
    for (j = 1; j < iTableSize-1; j++) {
        for (k = iTableSize>>1; k > (i^=k); k >>= 1);
        if (j < i) {SWAP(afTable[i], afTable[j], v)}
    }

    // DCT 計算
    for (mh = 1; (m=mh<<1) <= iTableSize; mh = m) {
        irev = 0;
        for (i = 0; i < iTableSize; i += m) {
            for (k = iTableSize>>2; k > (irev^=k); k >>= 1);
            for (j = i; j < mh + i; j++) {
                k = j + mh;
                v = afTable[j] - afTable[k];
                afTable[j] += afTable[k];
                afTable[k] = v * afCosTable[irev];
            }
        }
    }
}

