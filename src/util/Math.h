//--------------------------------------------------------------------------------
//  Math Sub Routines
//    Copyright  (C)  2003-2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//  ���쌠�֘A�iMersenne Twister �����j 
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
//  ���쌠�֘A 
//--------------------------------------------------------------------------------


#ifndef _CORE_MATH_INCLUDED
#define _CORE_MATH_INCLUDED

// Windows standard files
#include "std_include.h"
#pragma message ("include Math.h")

//--------------------------------------------------------------------------------
//  �}�N��
//--------------------------------------------------------------------------------
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

// INTFLOAT�G 4-byte Valiable union
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

//�p�x�ϊ�
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

//LIMIT�i�ŏ��l�C�ő�l�C�l�j
#ifndef LIMIT
#define LIMIT(min, max, v)      (((v) > (max)) ? (max) : (((v) < (min)) ? (min) : (v)))
#endif

//WRAPAROUND�i�ŏ��l�C�͈́C�l�j
#ifndef WRAPAROUND
#define WRAPAROUND(min, rng, v) ((fmod(((v) - (min)), (rng))) + (min))
#endif

// �X���b�v
#ifndef SWAP
#define SWAP(a, b, buf)         buf=a;  a=b;    b=buf;
#endif

// ��������
#ifndef SIGN
#define SIGN(a)                 ((a>0) ? 1 : ((a<0) ? -1 : 0))
#endif

//--------------------------------------------------------------------------------
// Log2
//--------------------------------------------------------------------------------
// i �ȏ�ōŏ��� 2^n ���v�Z����
int Calc2N(int i);
// i >= 2^n �𖞂����ő�� n ���߂�(�������Ci=0��n=-1)
int CalcLog2(int i);

//--------------------------------------------------------------------------------
//  srandMT(ULONG seed), randMT(), randMTf();
//  Mersenne Twister ���������D
//--------------------------------------------------------------------------------
// �������ݒ肷��
void srandMT(unsigned long seed);
// ���������o��
DWORD randMT();
float randMTf();

//--------------------------------------------------------------------------------
// float�^���U�R�T�C���ϊ�
// int iTableSize;    �e�[�u���T�C�Y
// float* afTable;    �ϊ�����e�[�u��
// float* afCosTable; �R�T�C���e�[�u��
//--------------------------------------------------------------------------------
void CalcDCT(int iTableSize, float* afTable, float* afCosTable);

//--------------------------------------------------------------------------------
//  ��ԃ}�N��
//--------------------------------------------------------------------------------
// float�^ 1�������`��ԃ}�N���i�l1�C�l2�C�p�����[�^�j
#define FLOAT_INTERPOLATION(v1, v2, fx) \
        ((v1)*(1.0f-(fx))+(v2)*(fx))

// float�^ 2�������`��ԃ}�N���i�l11�C�l12�C�l21�C�l22�C�p�����[�^x, �p�����[�^y�j
#define FLOAT_INTERPOLATION_2D(v11, v12, v21, v22, fx, fy) \
        FLOAT_INTERPOLATION(FLOAT_INTERPOLATION(v11, v21, fx), FLOAT_INTERPOLATION(v12, v22, fx), fy)

// float�^ 3�������`��ԃ}�N���΁i�l111�C�l112�C�l121�C�l122�C�l211�C�l212�C�l221�C�l222�C�p�����[�^x, �p�����[�^y, �p�����[�^z�j
#define FLOAT_INTERPOLATION_3D(v111, v112, v121, v122, v211, v212, v221, v222, fx, fy, fz) \
        FLOAT_INTERPOLATION(FLOAT_INTERPOLATION_2D(v111, v121, v211, v221, fx, fy), FLOAT_INTERPOLATION_2D(v112, v122, v212, v222, fx, fy), fz)

// BYTE�^ 1�������`��ԃ}�N���i�l1�C�l2�C�p�����[�^�j
#define BYTE_INTERPOLATION(v1, v2, bx) \
        (((v1)*(256-(bx))+(v2)*(bx))>>8)

// BYTE�^ 2�������`��ԃ}�N���i�l11�C�l12�C�l21�C�l22�C�p�����[�^x, �p�����[�^y�j
#define BYTE_INTERPOLATION_2D(v11, v12, v21, v22, bx, by) \
        ((((v11)*(256-(bx))+(v21)*(bx))*(256-(by))+((v12)*(256-(bx))+(v22)*(bx))*(by))>>16)

//--------------------------------------------------------------------------------
//  TInterpolation;  �񎟕�ԃN���X
//  1. Set*() �ŁC��ԕ��@�ƕ�Ԃ���_�̏���ݒ�
//  2. Calc(t) �ŁC�p�����[�^ t (0-1) �ł̕�Ԓl���v�Z
//--------------------------------------------------------------------------------
template <class type> class TInterpolation
{
private:
    type P, Q, R, S;       // ��ԌW��

public:
    // �R���X�g���N�^
    TInterpolation()
    {
    }

    // �R�s�[
    inline void Copy(TInterpolation* pInter)
    {
        P = pInter->P;
        Q = pInter->Q;
        R = pInter->R;
        S = pInter->S;
    }

    // Linear Interpolation
    // ���`��ԁD
    inline void SetLinear(const type &x1, const type &x2)
    {
        P = Q = 0;
        R = x2 - x1;
        S = x1;
    }

    // Bezier Interpolation
    // 2���x�W�F��ԁD�u�n�_ x1�v�Ɓu�I�_ x2�v�Ɓu����_ p�v�����2����ԁD
    inline void SetBezier(const type &x1, const type &p, const type &x2)
    {
        P = 0;
        Q = x1 + x2 - p * 2.0f;
        R = (p - x1) * 2.0f;
        S = x1;
        //         ��  1 -2  1���� x1 ��
        // [t2 t 1]�� -2  2  0���� p  �b
        //         ��  1  0  0���� x2 ��
    }

    // Bezier Interpolation
    // 3���x�W�F��ԁD�u�n�_ x1�v�Ɓu�I�_ x2�v�Ɓu����_ p1,p2�v�����3����ԁD
    inline void SetBezier(const type &x1, const type &p1, const type &x2, const type &p2)
    {
        P = - x1 + (p1 - p2) * 3.0f + x2;
        Q = ( x1 - p1 * 2.0f + p2) * 3.0f;
        R = (-x1 + p1) * 3.0f;
        S = x1;
        //            �� -1  3 -3  1���� x1 ��
        // [t3 t2 t 1]��  3 -6  3  0���� p1 ��
        //            �� -3  3  0  0���� p2 ��
        //            ��  1  0  0  0���� x2 ��
    }

    // Ferguson / Coons Interpolation
    // �u�ʒu x0,x1�v�Ɓu���̈ʒu�ł̑��x v0,v1�v����C���̊Ԃ��ԁD
    inline void SetFergusonCoons(const type &x0, const type &v0, const type &x1, const type &v1)
    {
        P = (x0 - x1) * 2.0f + v0 + v1;
        Q = - x0 + x1 - v0 - P;         // - (x0 * 3) + (x1 * 3) - (v0 * 2) - v1
        R = v0;
        S = x0;
        //            �� 2 -2  1  1���� x0 ��
        // [t3 t2 t 1]��-3  3 -2 -1���� x1 ��
        //            �� 0  0  1  0���� v0 ��
        //            �� 1  0  0  0���� v1 ��

    }

    // Lagrangian Interpolation
    // 3�����O�����W����ԁDx1 - x2 �Ԃɂ��āC���̑O��̓_�Cx0, x3 �����ɎO�����
    inline void SetLagrange(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = x3 - x2 - x0 + x1;  
        Q = x0 - x1 - P;        // (x0 * 2) - (x1 * 2) + x2 - x3
        R = x2 - x0;
        S = x1;
    }

    // Catmull Rom Interpolation
    // �_x1, x2 �ő��x��ۑ�����D�g�� Ferguson / Coons Interpolation
    inline void SetCatmullRom(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = (-x0 + x1 - x2 + x3) * 0.5f + x1 - x2;
        Q = x0 + (x2 - x1) * 2.0f - (x1 + x3) * 0.5f;
        R = (-x0 + x2) * 0.5f;
        S = x1;
        //  1             �� -1  3 -3  1���� x0 ��
        // --- [t3 t2 t 1]��  2 -5  4 -1���� x1 ��
        //  2             �� -1  0  1  0���� x2 ��
        //                ��  0  2  0  0���� x3 ��
    }

    // Catmull Rom Interpolation �̎n�_�v�Z
    // �_ x1-x2 ���x����1/2���n�_�̑��x�ɍ̗p
    inline void SetCatmullRomStart(const type &x1, const type &x2, const type &x3)
    {
        P = 0;
        Q = (x1 + x3) * 0.5f - x2;
        R = (x2 - x1) * 2.0f + (x1 - x3) * 0.5f;
        S = x1;
        //  1             ��  0  0  0  0���� 0  ��
        // --- [t3 t2 t 1]��  0  1 -2  1���� x1 ��
        //  2             ��  0 -3  4 -1���� x2 ��
        //                ��  0  2  0  0���� x3 ��
    }

    // Catmull Rom Interpolation �̏I�_�v�Z
    // �_ x1-x2 �ł̑��x����1/2���I�_�̑��x�ɍ̗p
    inline void SetCatmullRomEnd(const type &x0, const type &x1, const type &x2)
    {
        P = 0;
        Q = (x0 + x2) * 0.5f - x1;
        R = (x2 - x0) * 0.5f;
        S = x1;
        //  1             ��  0  0  0  0���� x0 ��
        // --- [t3 t2 t 1]��  1 -2  1  0���� x1 ��
        //  2             �� -1  0  1  0���� x2 ��
        //                ��  0  2  0  0���� 0  ��
    }

    // �v�Z���s
    inline type Calc(const float &t)
    {
        return (((P * t + Q) * t + R) * t) + S;
    }
};

//--------------------------------------------------------------------------------
//  TInterpolationByte; Byte�^ �񎟕�ԃN���X�D
//  �������l����e���v���[�g�N���X
//  1. Set*() �ŁC��ԕ��@�ƕ�Ԃ���_�̏���ݒ�D������f�[�^��Short�܂ŁD
//  2. Calc(t) �ŁC�p�����[�^ t (0-255) �ł̕�Ԓl���v�Z
//--------------------------------------------------------------------------------
template<class type> class TInterpolationByte
{
private:
    long P, Q, R, S;

public:
    // �R���X�g���N�^
    TInterpolationByte()
    {
        P = Q = R = S = 0;
    }

    // Linear Interpolation
    // ���`��ԁD
    inline void SetLinear(const type &x1, const type &x2)
    {
        P = Q = 0;
        R = x2 - x1;
        S = x1;
    }

    // Bezier Interpolation
    // 2���x�W�F��ԁD�u�n�_ x1�v�Ɓu�I�_ x2�v�Ɓu����_ p�v�����2����ԁD
    inline void SetBezier(const type &x1, const type &p, const type &x2)
    {
        P = 0;
        Q = x1 + x2 - (p << 1);
        R = (p - x1) << 1;
        S = x1;
    }

    // Bezier Interpolation
    // 3���x�W�F��ԁD�u�n�_ x1�v�Ɓu�I�_ x2�v�Ɓu����_ p1,p2�v�����3����ԁD
    inline void SetBezier(const type &x1, const type &p1, const type &x2, const type &p2)
    {
        P = - x1 + (p1 - p2) * 3 + x2;
        Q = ( x1 + p2 - (p1 << 1)) * 3;
        R = (-x1 + p1) * 3;
        S = x1;
    }

    // Ferguson / Coons Interpolation
    // �u�ʒu x0,x1�v�Ɓu���̈ʒu�ł̑��x v0,v1�v����C���̊Ԃ��ԁD
    inline void SetFergusonCoons(const type &x0, const type &v0, const type &x1, const type &v1)
    {
        P = ((x0 - x1) << 1) + v0 + v1;
        Q = - x0 + x1 - v0 - P;         // - (x0 * 3) + (x1 * 3) - (v0 * 2) - v1
        R = v0;
        S = x0;
    }

    // Lagrangian Interpolation
    // 3�����O�����W����ԁDx1 - x2 �Ԃɂ��āC���̑O��̓_�Cx0, x3 �����ɎO�����
    inline void SetLagrange(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = x3 - x2 - x0 + x1;  
        Q = x0 - x1 - P;        // (x0 * 2) - (x1 * 2) + x2 - x3
        R = x2 - x0;
        S = x1;
    }

    // Catmull Rom Interpolation
    // �_x1, x2 �ő��x��ۑ�����D�g�� Ferguson / Coons Interpolation
    inline void SetCatmullRom(const type &x0, const type &x1, const type &x2, const type &x3)
    {
        P = ((-x0 + x1 - x2 + x3) >> 1) + x1 - x2;
        Q = x0 + ((x2 - x1) << 1) - ((x1 + x3) >> 1);
        R = (-x0 + x2) >> 1;
        S = x1;
    }

    // Catmull Rom Interpolation �̎n�_�v�Z
    // �_ x1-x2 ���x����1/2���n�_�̑��x�ɍ̗p
    inline void SetCatmullRomStart(const type &x1, const type &x2, const type &x3)
    {
        P = 0;
        Q = ((x1 + x3) >> 1) - x2;
        R = ((x2 - x1) << 1) + ((x1 - x3) >> 1);
        S = x1;
    }

    // Catmull Rom Interpolation �̏I�_�v�Z
    // �_ x1-x2 �ł̑��x����1/2���I�_�̑��x�ɍ̗p
    inline void SetCatmullRomEnd(const type &x0, const type &x1, const type &x2)
    {
        P = 0;
        Q = ((x0 + x2) >> 1) - x1;
        R = ((x2 - x0) >> 1);
        S = x1;
    }

    // �v�Z���s
    inline type Calc(const unsigned char &t)
    {
        return ((((((P * t + Q) * t) + R) * t) >> 24) + S) >> 8;
    }
};

//--------------------------------------------------------------------------------
//  SSinTable (Singleton); Sin/Cos Table �N���X
//  0.SSinTable::GetInstance() �Ŏ��̊m��
//  1.Alloc(TableBase) �Ńe�[�u���쐬�D2^TableBase �̃e�[�u�����쐻����
//  2.Sin(deg)/Cos(deg) �Ńe�[�u���Q��
//  3.FloatToIndex(deg) �Ŋp�x���C���f�b�N�X�ϊ�
//  4.Free() �ŉ���i�ÓI�C���X�^���X�̃f�X�g���N�^�Ŏ�����������j
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

    // �e�[�u���쐬�i2^iTableBase �̃e�[�u�����쐻����j
    inline bool Alloc(int iTableBase = 10)
    {
        Free();
        
        int isize = (1 << iTableBase) + (1 << (iTableBase-2));    // �z��T�C�Y�Dsize * 1.25
        s_aTable = new float[isize];
        if (s_aTable == NULL) return false;
        
        s_iTableCount = 1 << iTableBase;                         // �T���v�����O��
        s_iTableFilter = s_iTableCount - 1;                     // 0��= 360���Ŏ��񂳂���t�B���^
        s_pCosTable = &(s_aTable[s_iTableCount>>2]);            // �R�T�C���e�[�u���Ƃ��ĎQ�Ƃ���ʒu
        s_fFloatToIndex = (float)s_iTableCount / 360.0f;
        s_fIndexToFloat = 360.0f / (float)s_iTableCount;
        for (int i=0; i<isize; i++) {
            s_aTable[i] = sinf(DEGREEtoRADIAN((float)i * s_fIndexToFloat));
        }
        return true;
    }
    
    // �e�[�u���j��
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
    
    // �C���f�b�N�X�ϊ�
    inline int   FloatToIndex(float f) {return (((int)(f * s_fFloatToIndex)) & s_iTableFilter);}
    inline float IndexToFloat(int   i) {return (float)i * s_fIndexToFloat;}
    // �e�[�u���Q��
    inline float Sin(float f) {return s_aTable[FloatToIndex(f)];}
    inline float Cos(float f) {return s_pCosTable[FloatToIndex(f)];}
    inline float Sin(int idx) {return s_aTable[idx & s_iTableFilter];}
    inline float Cos(int idx) {return s_pCosTable[idx & s_iTableFilter];}
};

#endif //_CORE_MATH_INCLUDED

