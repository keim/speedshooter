//--------------------------------------------------------------------------------
//  Math Sub Routines
//    Copyright  (C)  2003-2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//  著作権関連（Mersenne Twister 乱数） 
//--------------------------------------------------------------------------------
/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/
//--------------------------------------------------------------------------------
//  著作権関連 
//--------------------------------------------------------------------------------


#ifndef _CORE_MATH_INCLUDED
#define _CORE_MATH_INCLUDED

// Windows standard files
#include "std_include.h"
#pragma message ("include Math.h")

//--------------------------------------------------------------------------------
//  マクロ
//--------------------------------------------------------------------------------
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

// INTFLOAT； 4-byte Valiable union
typedef union tagINTFLOAT {
    unsigned long   ul;
    long            l;
    float           f;
} INTFLOAT;

// DWORDBYTE;   4-byte MultiAccessable DWORD
typedef union tagDWORDBYTE {
    unsigned long   dw;
    unsigned char   by[4];
    char            ch[4];
} DWORDBYTE;

//float
#ifndef FLOAT_RANGE_MAX
#define FLOAT_RANGE_MAX     (3.402823466e+38f)
#endif

#ifndef DOUBLE_RANGE_MAX
#define DOUBLE_RANGE_MAX    (1.7976931348623158e+308)
#endif

//PI
#ifndef PI
#define PI    (3.1415926535897932384626433832795)
#endif

#ifndef RD
#define RD    (6.283185307179586476925286766559)
#endif

#ifndef fPI
#define fPI   (3.141593f)
#endif

#ifndef fRD
#define fRD   (6.283185f)
#endif

#ifndef dPI
#define dPI   (3.141592653589793)
#endif

#ifndef dRD
#define dRD   (6.283185307179586)
#endif

#ifndef __min
#define __min(a, b)     ((a) <= (b)) ? (a) : (b)
#endif

#ifndef __max
#define __max(a, b)     ((a) >= (b)) ? (a) : (b)
#endif

//byte(0-255) <-> float(0-1)
#define BYTEtoFLOAT(i)                  (float)((i) * 0.0039215686f)
#define FLOATtoBYTE(f)                  (BYTE)((f) * 255)

//角度変換
#ifndef DEGREEtoRADIAN
#define DEGREEtoRADIAN(v)               ((v) * 0.0174533f)
#endif

#ifndef RADIANtoDEGREE
#define RADIANtoDEGREE(v)               ((v) * 57.29578f)
#endif

#ifndef dDEGREEtoRADIAN
#define dDEGREEtoRADIAN(v)              ((v) * 0.01745329251994330)
#endif

#ifndef dRADIANtoDEGREE
#define dRADIANtoDEGREE(v)              ((v) * 57.295779513082321)
#endif

//LIMIT（最小値，最大値，値）
#ifndef LIMIT
#define LIMIT(min, max, v)      (((v) > (max)) ? (max) : (((v) < (min)) ? (min) : (v)))
#endif

//WRAPAROUND（最小値，範囲，値）
#ifndef WRAPAROUND
#define WRAPAROUND(min, rng, v) ((fmod(((v) - (min)), (rng))) + (min))
#endif

// スワップ
#ifndef SWAP
#define SWAP(a, b, buf)         buf=a;  a=b;    b=buf;
#endif

// 正負判定
#ifndef SIGN
#define SIGN(a)                 ((a>0) ? 1 : ((a<0) ? -1 : 0))
#endif

//--------------------------------------------------------------------------------
// Log2
//--------------------------------------------------------------------------------
// i 以上で最小の 2^n を計算する
int Calc2N(int i);
// i >= 2^n を満たす最大の n 求める(ただし，i=0→n=-1)
int CalcLog2(int i);

//--------------------------------------------------------------------------------
//  srandMT(ULONG seed), randMT(), randMTf();
//  Mersenne Twister 乱数発生．
//--------------------------------------------------------------------------------
// 乱数種を設定する
void srandMT(unsigned long seed);
// 乱数を取り出す
DWORD randMT();
float randMTf();

//--------------------------------------------------------------------------------
// float型離散コサイン変換
// int iTableSize;    テーブルサイズ
// float* afTable;    変換するテーブル
// float* afCosTable; コサインテーブル
//--------------------------------------------------------------------------------
void CalcDCT(int iTableSize, float* afTable, float* afCosTable);

//--------------------------------------------------------------------------------
//  補間マクロ
//--------------------------------------------------------------------------------
// float型 1次元線形補間マクロ（値1，値2，パラメータ）
#define FLOAT_INTERPOLATION(v1, v2, fx) \
        ((v1)*(1.0f-(fx))+(v2)*(fx))

// float型 2次元線形補間マクロ（値11，値12，値21，値22，パラメータx, パラメータy）
#define FLOAT_INTERPOLATION_2D(v11, v12, v21, v22, fx, fy) \
        FLOAT_INTERPOLATION(FLOAT_INTERPOLATION(v11, v21, fx), FLOAT_INTERPOLATION(v12, v22, fx), fy)

// float型 3次元線形補間マクロ笑（値111，値112，値121，値122，値211，値212，値221，値222，パラメータx, パラメータy, パラメータz）
#define FLOAT_INTERPOLATION_3D(v111, v112, v121, v122, v211, v212, v221, v222, fx, fy, fz) \
        FLOAT_INTERPOLATION(FLOAT_INTERPOLATION_2D(v111, v121, v211, v221, fx, fy), FLOAT_INTERPOLATION_2D(v112, v122, v212, v222, fx, fy), fz)

// BYTE型 1次元線形補間マクロ（値1，値2，パラメータ）
#define BYTE_INTERPOLATION(v1, v2, bx) \
        (((v1)*(256-(bx))+(v2)*(bx))>>8)

// BYTE型 2次元線形補間マクロ（値11，値12，値21，値22，パラメータx, パラメータy）
#define BYTE_INTERPOLATION_2D(v11, v12, v21, v22, bx, by) \
        ((((v11)*(256-(bx))+(v21)*(bx))*(256-(by))+((v12)*(256-(bx))+(v22)*(bx))*(by))>>16)

//--------------------------------------------------------------------------------
//  TInterpolation;  二次補間クラス
//  1. Set*() で，補間方法と補間する点の情報を設定
//  2. Calc(t) で，パラメータ t (0-1) での補間値を計算
//--------------------------------------------------------------------------------
template <class type> class TInterpolation
{
private:
    type P, Q, R, S;       // 補間係数

public:
    // コンストラクタ
    TInterpolation()
    {
    }

    // コピー
    inline void Copy(TInterpolation* pInter)
    {
        P = pInter->P;
        Q = pInter->Q;
        R = pInter->R;
        S = pInter->S;
    }

    // Linear Interpolation
    // 線形補間．
    inline void SetLinear(const type &x1, const type &x2)
    {
        P = Q = 0;
        R = x2 - x1;
        S = x1;
    }

    // Bezier Interpolation
    // 2次ベジェ補間．「始点 x1」と「終点 x2」と「制御点 p」からの2次補間．
    inline void SetBezier(const type &x1, const type &p, const type &x2)
    {
        P = 0;
        Q = x1 + x2 - p * 2.0f;
        R = (p - x1) * 2.0f;
        S = x1;
        //         ┌  1 -2  1┐┌ x1 ┐
        // [t2 t 1]│ -2  2  0││ p  ｜
        //         └  1  0  0┘└ x2 ┘
    }

    // Bezier Interpolation
    // 3次ベジェ補間．「始点 x1」と「終点 x2」と「制御点 p1,p2」からの3次補間．
    inline void SetBezier(const type &x1, const type &p1, const type &x2, const type &p2)
    {
        P = - x1 + (p1 - p2) * 3.0f + x2;
        Q = ( x1 - p1 * 2.0f + p2) * 3.0f;
        R = (-x1 + p1) * 3.0f;
        S = x1;
        //            ┌ -1  3 -3  1┐┌ x1 ┐
        // [t3 t2 t 1]│  3 -6  3  0││ p1 │
        //            │ -3  3  0  0││ p2 │
        //            └  1  0  0  0┘└ x2 ┘
    }

    // Ferguson / Coons Interpolation
    // 「位置 x0,x1」と「その位置での速度 v0,v1」から，その間を補間．
    inline void SetFergusonCoons(const type &x0, const type &v0, const type &x1, const type &v1)
    {
        P = (x0 - x1) * 2.0f + v0 + v1;
        Q = - x0 + x1 - v0 - P;         // - (x0 * 3) + (x1 * 3) - (v0 * 2) - v1
        R = v0;
        S = x0;
        //            ┌ 2 -2  1  1┐┌ x0 ┐
        // [t3 t2 t 1]│-3  3 -2 -1││ x1 │
        //            │ 0  0  1  0││ v0 │
        //            └ 1  0  0  0┘└ v1 ┘

    }

    // Lagrangian Interpolation
    // 3次ラグランジュ補間．x1 - x2 間について，その前後の点，x0, x3 を元に三次補間
    inline void SetLagrange(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = x3 - x2 - x0 + x1;  
        Q = x0 - x1 - P;        // (x0 * 2) - (x1 * 2) + x2 - x3
        R = x2 - x0;
        S = x1;
    }

    // Catmull Rom Interpolation
    // 点x1, x2 で速度を保存する．拡張 Ferguson / Coons Interpolation
    inline void SetCatmullRom(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = (-x0 + x1 - x2 + x3) * 0.5f + x1 - x2;
        Q = x0 + (x2 - x1) * 2.0f - (x1 + x3) * 0.5f;
        R = (-x0 + x2) * 0.5f;
        S = x1;
        //  1             ┌ -1  3 -3  1┐┌ x0 ┐
        // --- [t3 t2 t 1]│  2 -5  4 -1││ x1 │
        //  2             │ -1  0  1  0││ x2 │
        //                └  0  2  0  0┘└ x3 ┘
    }

    // Catmull Rom Interpolation の始点計算
    // 点 x1-x2 速度差の1/2を始点の速度に採用
    inline void SetCatmullRomStart(const type &x1, const type &x2, const type &x3)
    {
        P = 0;
        Q = (x1 + x3) * 0.5f - x2;
        R = (x2 - x1) * 2.0f + (x1 - x3) * 0.5f;
        S = x1;
        //  1             ┌  0  0  0  0┐┌ 0  ┐
        // --- [t3 t2 t 1]│  0  1 -2  1││ x1 │
        //  2             │  0 -3  4 -1││ x2 │
        //                └  0  2  0  0┘└ x3 ┘
    }

    // Catmull Rom Interpolation の終点計算
    // 点 x1-x2 での速度差の1/2を終点の速度に採用
    inline void SetCatmullRomEnd(const type &x0, const type &x1, const type &x2)
    {
        P = 0;
        Q = (x0 + x2) * 0.5f - x1;
        R = (x2 - x0) * 0.5f;
        S = x1;
        //  1             ┌  0  0  0  0┐┌ x0 ┐
        // --- [t3 t2 t 1]│  1 -2  1  0││ x1 │
        //  2             │ -1  0  1  0││ x2 │
        //                └  0  2  0  0┘└ 0  ┘
    }

    // 計算実行
    inline type Calc(const float &t)
    {
        return (((P * t + Q) * t + R) * t) + S;
    }
};

//--------------------------------------------------------------------------------
//  TInterpolationByte; Byte型 二次補間クラス．
//  ※整数値限定テンプレートクラス
//  1. Set*() で，補間方法と補間する点の情報を設定．扱えるデータはShortまで．
//  2. Calc(t) で，パラメータ t (0-255) での補間値を計算
//--------------------------------------------------------------------------------
template<class type> class TInterpolationByte
{
private:
    long P, Q, R, S;

public:
    // コンストラクタ
    TInterpolationByte()
    {
        P = Q = R = S = 0;
    }

    // Linear Interpolation
    // 線形補間．
    inline void SetLinear(const type &x1, const type &x2)
    {
        P = Q = 0;
        R = x2 - x1;
        S = x1;
    }

    // Bezier Interpolation
    // 2次ベジェ補間．「始点 x1」と「終点 x2」と「制御点 p」からの2次補間．
    inline void SetBezier(const type &x1, const type &p, const type &x2)
    {
        P = 0;
        Q = x1 + x2 - (p << 1);
        R = (p - x1) << 1;
        S = x1;
    }

    // Bezier Interpolation
    // 3次ベジェ補間．「始点 x1」と「終点 x2」と「制御点 p1,p2」からの3次補間．
    inline void SetBezier(const type &x1, const type &p1, const type &x2, const type &p2)
    {
        P = - x1 + (p1 - p2) * 3 + x2;
        Q = ( x1 + p2 - (p1 << 1)) * 3;
        R = (-x1 + p1) * 3;
        S = x1;
    }

    // Ferguson / Coons Interpolation
    // 「位置 x0,x1」と「その位置での速度 v0,v1」から，その間を補間．
    inline void SetFergusonCoons(const type &x0, const type &v0, const type &x1, const type &v1)
    {
        P = ((x0 - x1) << 1) + v0 + v1;
        Q = - x0 + x1 - v0 - P;         // - (x0 * 3) + (x1 * 3) - (v0 * 2) - v1
        R = v0;
        S = x0;
    }

    // Lagrangian Interpolation
    // 3次ラグランジュ補間．x1 - x2 間について，その前後の点，x0, x3 を元に三次補間
    inline void SetLagrange(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = x3 - x2 - x0 + x1;  
        Q = x0 - x1 - P;        // (x0 * 2) - (x1 * 2) + x2 - x3
        R = x2 - x0;
        S = x1;
    }

    // Catmull Rom Interpolation
    // 点x1, x2 で速度を保存する．拡張 Ferguson / Coons Interpolation
    inline void SetCatmullRom(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = ((-x0 + x1 - x2 + x3) >> 1) + x1 - x2;
        Q = x0 + ((x2 - x1) << 1) - ((x1 + x3) >> 1);
        R = (-x0 + x2) >> 1;
        S = x1;
    }

    // Catmull Rom Interpolation の始点計算
    // 点 x1-x2 速度差の1/2を始点の速度に採用
    inline void SetCatmullRomStart(const type &x1, const type &x2, const type &x3)
    {
        P = 0;
        Q = ((x1 + x3) >> 1) - x2;
        R = ((x2 - x1) << 1) + ((x1 - x3) >> 1);
        S = x1;
    }

    // Catmull Rom Interpolation の終点計算
    // 点 x1-x2 での速度差の1/2を終点の速度に採用
    inline void SetCatmullRomEnd(const type &x0, const type &x1, const type &x2)
    {
        P = 0;
        Q = ((x0 + x2) >> 1) - x1;
        R = ((x2 - x0) >> 1);
        S = x1;
    }

    // 計算実行
    inline type Calc(const unsigned char &t)
    {
        return ((((((P * t + Q) * t) + R) * t) >> 24) + S) >> 8;
    }
};

//--------------------------------------------------------------------------------
//  SSinTable (Singleton); Sin/Cos Table クラス
//  0.SSinTable::GetInstance() で実体確保
//  1.Alloc(TableBase) でテーブル作成．2^TableBase 個のテーブルを作製する
//  2.Sin(deg)/Cos(deg) でテーブル参照
//  3.FloatToIndex(deg) で角度→インデックス変換
//  4.Free() で解放（静的インスタンスのデストラクタで自動解放される）
//--------------------------------------------------------------------------------
class CSinTable
{
public:
    float* s_aTable;
    float* s_pCosTable;
    
private:
    int    s_iTableCount;
    int    s_iTableFilter;
    float  s_fFloatToIndex;
    float  s_fIndexToFloat;
        
public:
    CSinTable()
    {
        s_aTable = NULL;
        s_iTableCount = 0;
        s_iTableFilter = 0;
        s_pCosTable = NULL;
        s_fFloatToIndex = 0.0f;
        s_fIndexToFloat = 0.0f;
    }

    ~CSinTable()
    {
        Free();
    }

    // テーブル作成（2^iTableBase 個のテーブルを作製する）
    inline bool Alloc(int iTableBase = 10)
    {
        Free();
        
        int isize = (1 << iTableBase) + (1 << (iTableBase-2));    // 配列サイズ．size * 1.25
        s_aTable = new float[isize];
        if (s_aTable == NULL) return false;
        
        s_iTableCount = 1 << iTableBase;                         // サンプリング数
        s_iTableFilter = s_iTableCount - 1;                     // 0°= 360°で周回させるフィルタ
        s_pCosTable = &(s_aTable[s_iTableCount>>2]);            // コサインテーブルとして参照する位置
        s_fFloatToIndex = (float)s_iTableCount / 360.0f;
        s_fIndexToFloat = 360.0f / (float)s_iTableCount;
        for (int i=0; i<isize; i++) {
            s_aTable[i] = sinf(DEGREEtoRADIAN((float)i * s_fIndexToFloat));
        }
        return true;
    }
    
    // テーブル破棄
    inline void Free()
    {
        if (s_aTable != NULL) delete[] s_aTable;
        s_aTable = NULL;
        s_iTableCount = 0;
        s_iTableFilter = 0;
        s_pCosTable = NULL;
        s_fFloatToIndex = 0.0f;
        s_fIndexToFloat = 0.0f;
    }
    
    // インデックス変換
    inline int   FloatToIndex(float f) {return (((int)(f * s_fFloatToIndex)) & s_iTableFilter);}
    inline float IndexToFloat(int   i) {return (float)i * s_fIndexToFloat;}
    // テーブル参照
    inline float Sin(float f) {return s_aTable[FloatToIndex(f)];}
    inline float Cos(float f) {return s_pCosTable[FloatToIndex(f)];}
    inline float Sin(int idx) {return s_aTable[idx & s_iTableFilter];}
    inline float Cos(int idx) {return s_pCosTable[idx & s_iTableFilter];}
};

#endif //_CORE_MATH_INCLUDED

