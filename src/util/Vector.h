//--------------------------------------------------------------------------------
//  Vector/Matrix Classes
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  class VECTOR2D 2Dベクトル クラス，メンバ変数 float x, y
//  class CVector  3Dベクトル クラス，メンバ変数 float x, y, z, w (又は r, g, b, a)
//  class MATRIX2D 2x2行列 クラス，   メンバ変数 float a[2][2]
//  class CMatril  4x4行列 クラス，   メンバ変数 float a[4][4]
//  class CQuaternion : public CVector  
//      クォータニオン クラス．CVector との違いは演算子のオーバーライドだけ．
//  struct Segment  線分構造体（始点/終点）
//  struct Line     無限直線構造体（内包点/方向）
//  struct Fase     無限平面構造体（内包点/法線）
//--------------------------------------------------------------------------------
#ifndef _CORE_VECTOR_INCLUDED
#define _CORE_VECTOR_INCLUDED

#include "Math.h"
#pragma message ("<< CORE_MESSAGE >>  include CVector")

//--------------------------------------------------------------------------------
//  2Dベクトル
//--------------------------------------------------------------------------------
class VECTOR2D
{
//--------------------------------------------------
//  メンバ変数
//--------------------------------------------------
public:
    float x;
    float y;

//--------------------------------------------------
//  メンバ関数
//--------------------------------------------------
public:
    VECTOR2D()                                    {x=0;   y=0;}
    VECTOR2D(const float &fX, const float &fY)    {x=fX;  y=fY;}
    VECTOR2D(const VECTOR2D &v)                   {x=v.x; y=v.y;}

    inline void Set(const float &fX, const float &fY)    {x=fX;  y=fY;}
    inline operator float*() const                       {return (float*)&x;}

    // 比較
    inline bool operator == (const VECTOR2D &v) const   {return ((x==v.x) && (y==v.y));}
    inline bool operator != (const VECTOR2D &v) const   {return ((x!=v.x) || (y!=v.y));}
    inline bool operator == (const float &f) const      {return (Length2() == f*f);}
    inline bool operator != (const float &f) const      {return (Length2() != f*f);}
    inline bool operator <  (const VECTOR2D &v) const   {return (Length2() <  v.Length2());}
    inline bool operator <= (const VECTOR2D &v) const   {return (Length2() <= v.Length2());}
    inline bool operator >  (const VECTOR2D &v) const   {return (Length2() >  v.Length2());}
    inline bool operator >= (const VECTOR2D &v) const   {return (Length2() >= v.Length2());}
    inline bool operator <  (const float &f) const      {return (Length2() <  f*f);}
    inline bool operator <= (const float &f) const      {return (Length2() <= f*f);}
    inline bool operator >  (const float &f) const      {return (Length2() >  f*f);}
    inline bool operator >= (const float &f) const      {return (Length2() >= f*f);}
    inline bool IsZero() const                          {return ((x==0.0f) && (y==0.0f));}
    inline bool IsNormalized() const                    {return ((x*x + y*y) == 1.0f);}

    // 代入
    inline VECTOR2D& operator = (const VECTOR2D &v)      {x=v.x; y=v.y;  return *this;}

    // ネガ
    inline VECTOR2D operator - () const
    {
        return VECTOR2D(-x, -y);
    }

    // 加算
    inline VECTOR2D operator + (const VECTOR2D &v) const
    {
        VECTOR2D res;
        res.x = x + v.x;
        res.y = y + v.y;
        return res;
    }

    // 加算
    inline VECTOR2D& operator += (const VECTOR2D &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // 減算
    inline VECTOR2D operator - (const VECTOR2D &v) const
    {
        VECTOR2D res;
        res.x = x - v.x;
        res.y = y - v.y;
        return res;
    }

    // 減算
    inline VECTOR2D& operator -= (const VECTOR2D &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    // スケーリング
    inline VECTOR2D operator * (const float &f) const
    {
        VECTOR2D res;
        res.x = x * f;
        res.y = y * f;
        return res;
    }

    // スケーリング
    inline VECTOR2D& operator *= (const float &f)
    {
        x *= f;
        y *= f;
        return *this;
    }
	
    // スケーリング
    inline VECTOR2D operator / (const float &f) const
    {
        VECTOR2D res;
        float    invf = 1.0f / f;
        res.x = x * invf;
        res.y = y * invf;
        return res;
    }

    // スケーリング
    inline VECTOR2D& operator /= (const float &f)
    {
        float invf = 1.0f / f;
        x *= invf;
        y *= invf;
        return *this;
    }
	
    // 内積
    inline float operator * (const VECTOR2D &v) const
    {
        return x * v.x + y * v.y;
    }

    // 外積の z 成分
    inline float operator / (const VECTOR2D &v) const
    {
        return x * v.y - y * v.x;
    }

    // 角度から，単位ベクトル設定
    inline void SetAngle(float rad)
    {
        x = cosf(rad);
        y = sinf(rad);
    }

	// 曲座標から，ベクトル設定
    inline void SetRound(float len, float rad)
    {
        x = cosf(rad) * len;
        y = sinf(rad) * len;
    }

    // 長さ
    inline float Length() const 
    {
        return sqrtf(x * x + y * y);
    }

    // 長さの2乗
    inline float Length2() const 
    {
        return x * x + y * y;
    }

    // 角度算出
    inline float Atan() const
    {
        return atan2f(y, x);
    }

    // 正規化
    inline float Normalize()
    {
        float fLength = x * x + y * y;
        if (fLength == 0.0f) return 0;          //エラー
        fLength = sqrtf(fLength);
        float fScale = 1.0f/fLength;
        x *= fScale;
        y *= fScale;
        return fLength;
    }

    // 回転（rotv.SetAngle(rad) で設定した VECTOR2D で回転）
    inline void Rotate(const VECTOR2D &rotv)
    {
        float bufx = x;
        x = x * rotv.x + y * rotv.y;
        y = -bufx * rotv.y + y * rotv.x;
    }

    // 比較小
    inline void Min(const VECTOR2D& v)
    {
        if (x > v.x) x = v.x;
        if (y > v.y) y = v.y;
    }

    // 比較大
    inline void Max(const VECTOR2D& v)
    {
        if (x < v.x) x = v.x;
        if (y < v.y) y = v.y;
    }
};


//--------------------------------------------------------------------------------
//  3Dベクトル
//--------------------------------------------------------------------------------
class CVector
{
//--------------------------------------------------
//  メンバ変数
//--------------------------------------------------
public:
    union {float x; float r;};
    union {float y; float g;};
    union {float z; float b;};
    union {float w; float a;};

//--------------------------------------------------
//  メンバ関数
//--------------------------------------------------
public:
    //0で初期化
    CVector()                                        {x=0.0f; y=0.0f; z=0.0f; w=1.0f;}
    //コピーコンストラクタ
    CVector(const CVector &v)                        {x=v.x;  y=v.y;  z=v.z;  w=v.w; }
    //差分を代入
    CVector(const CVector &v1, const CVector &v2)    {x=v1.x-v2.x;  y=v1.y-v2.y;  z=v1.z-v2.z;  w=v1.w-v2.w; }
    //設定コンストラクタ
    CVector(float fX, float fY, float fZ, float fW)  {x=fX;   y=fY;   z=fZ;   w=fW;  }
    CVector(float fX, float fY, float fZ)            {x=fX;   y=fY;   z=fZ;   w=1.0f;}
    CVector(float fX, float fY)                      {x=fX;   y=fY;   z=0.0f; w=1.0f;}
    CVector(const VECTOR2D &v)                       {x=v.x;  y=v.y;  z=0.0f; w=1.0f;}

    inline operator float*() const                          {return (float*)&x;}
    inline operator VECTOR2D() const                        {return *(VECTOR2D*)this;}
    inline CVector& operator =  (const CVector &v)          {x=v.x;  y=v.y;  z=v.z;  w=v.w;  return *this;}
    inline CVector& operator =  (const VECTOR2D &v)         {x=v.x;  y=v.y;  return *this;}
    inline CVector  operator -  () const                    {return CVector(-x, -y, -z, w);}
    inline CVector  operator +  (const CVector &v) const    {CVector r(*this); r.Add(v);    return r;}
    inline CVector  operator -  (const CVector &v) const    {CVector r(*this); r.Sub(v);    return r;}
    inline CVector  operator *  (const float &f) const      {CVector r(*this); r.Scale(f);  return r;}
    inline float    operator *  (const CVector &v) const    {return Dot(v);}
    inline CVector  operator /  (const float &f) const      {CVector r(*this); float invf = 1.0f / f;  r.Scale(invf);  return r;}
    inline CVector  operator /  (const CVector &v) const    {CVector r;        r.Cross(*this, v); return r;}
    inline CVector& operator += (const CVector &v)          {Add(v);   return *this;}
    inline CVector& operator -= (const CVector &v)          {Sub(v);   return *this;}
    inline CVector& operator *= (const float &f)            {Scale(f); return *this;}
    inline CVector& operator /= (const float &f)            {float invf = 1.0f / f;  Scale(invf);  return *this;}
    inline CVector& operator /= (const CVector &v)          {Cross(v); return *this;}
    inline bool     operator == (const CVector &v)          {return ((x==v.x) && (y==v.y) && (z==v.z));}
    inline bool     operator != (const CVector &v)          {return ((x!=v.x) || (y!=v.y) || (z!=v.z));}
    inline bool     operator == (const float &f) const      {return (Length2() == f*f);}
    inline bool     operator != (const float &f) const      {return (Length2() != f*f);}
    inline bool     operator <  (const CVector &v) const    {return (Length2() <  v.Length2());}
    inline bool     operator <= (const CVector &v) const    {return (Length2() <= v.Length2());}
    inline bool     operator >  (const CVector &v) const    {return (Length2() >  v.Length2());}
    inline bool     operator >= (const CVector &v) const    {return (Length2() >= v.Length2());}
    inline bool     operator <  (const float &f) const      {return (Length2() <  f*f);}
    inline bool     operator <= (const float &f) const      {return (Length2() <= f*f);}
    inline bool     operator >  (const float &f) const      {return (Length2() >  f*f);}
    inline bool     operator >= (const float &f) const      {return (Length2() >= f*f);}

    inline void SetPosition(float fX, float fY, float fZ)   {x=fX;   y=fY;   z=fZ;   w=1.0f;}
    inline void SetDirection(float fX, float fY, float fZ)  {x=fX;   y=fY;   z=fZ;   w=0.0f;}
    inline void Set(float fX, float fY, float fZ, float fW) {x=fX;   y=fY;   z=fZ;   w=fW;}
    inline void Set(float fX, float fY, float fZ)           {x=fX;   y=fY;   z=fZ;}
    inline void Set(float fX, float fY)                     {x=fX;   y=fY;}
    inline void Copy(const CVector &v)                      {x=v.x;  y=v.y;  z=v.z;  w=v.w; }
    inline void SetDWORD(unsigned long &dw)                 {x = (dw&255)       * 0.0039215686f;
                                                             y = ((dw>>8)&255)  * 0.0039215686f;
                                                             z = ((dw>>16)&255) * 0.0039215686f;
                                                             w = ((dw>>24)&255) * 0.0039215686f;}
    inline unsigned long GetDWORD()                         {return (unsigned long)(((unsigned char)(x*255)) | 
                                                                                   (((unsigned char)(y*255)) << 8) | 
                                                                                   (((unsigned char)(z*255)) << 16) | 
                                                                                   (((unsigned char)(w*255)) << 24));}

    inline void Invert(const CVector &v)                    {x=-v.x; y=-v.y; z=-v.z; w=v.w; }
    inline void Invert()                                    {x=-x;   y=-y;   z=-z;}
    inline bool IsZero() const                              {return (((*(DWORD*)(&x)) | (*(DWORD*)(&y)) | (*(DWORD*)(&z))) == 0);}
    inline bool IsNormlized() const                         {return ((x * x + y * y + z * z) == 1.0f);}

//--------------------------------------------------
//  ３次元ベクトル計算
//--------------------------------------------------
    //  加算 (w は無変更)
    inline void Add(const CVector &vc)
    {
        x += vc.x;
        y += vc.y;
        z += vc.z;
    }

    //  加算結果代入(w = vc1.w)
    inline void Add(const CVector &vc1, const CVector &vc2)
    {
        x = vc1.x + vc2.x;
        y = vc1.y + vc2.y;
        z = vc1.z + vc2.z;
        w = vc1.w;
    }

    //  スケーリング後加算 (vcDirection * fScale / w は無変更)
    inline void Add(const CVector &vcDirection, const float fScale)
    {
        x += vcDirection.x * fScale;
        y += vcDirection.y * fScale;
        z += vcDirection.z * fScale;
    }

    //  スケーリング後加算の結果代入 (vcPoint + vcDirection * fScale / w = vcPoint.w)
    inline void Add(const CVector &vcPoint, const CVector &vcDirection, const float fScale)
    {
        x = vcPoint.x + vcDirection.x * fScale;
        y = vcPoint.y + vcDirection.y * fScale;
        z = vcPoint.z + vcDirection.z * fScale;
        w = vcPoint.w;
    }

    //  重み付け加算結果代入 (vc1 * fScale1 + vc2 * fScale / w = vc1.w2)
    inline void Add(const CVector &vc1, const float fScale1, const CVector &vc2, const float fScale2)
    {
        x = vc1.x * fScale1 + vc2.x * fScale2;
        y = vc1.y * fScale1 + vc2.y * fScale2;
        z = vc1.z * fScale1 + vc2.z * fScale2;
        w = vc1.w;
    }

    //  減算 (w は無変更)
    inline void Sub(const CVector &vc)
    {
        x -= vc.x;
        y -= vc.y;
        z -= vc.z;
    }

    //  減算結果代入 (w = vc1.w)
    inline void Sub(const CVector &vc1, const CVector &vc2)
    {
        x = vc1.x - vc2.x;
        y = vc1.y - vc2.y;
        z = vc1.z - vc2.z;
        w = vc1.w;
    }

    //  スケーリング (w は無変更)
    inline void Scale(const float fScale)
    {
        x *= fScale;
        y *= fScale;
        z *= fScale;
    }

    //  スケーリング結果代入 (w = vc.w)
    inline void Scale(const CVector &vc, const float fScale)
    {
        x = vc.x * fScale;
        y = vc.y * fScale;
        z = vc.z * fScale;
        w = vc.w;
    }

    //  外積 (w=0)
    inline void Cross(const CVector &vcForward)
    {
        CVector vcResult(y * vcForward.z - z * vcForward.y,
                         z * vcForward.x - x * vcForward.z,
                         x * vcForward.y - y * vcForward.x, 
                         0.0f);
        Copy(vcResult);
    }

    //  外積結果代入 (w=0)
    inline void Cross(const CVector &vcRight, const CVector &vcForward)
    {
        CVector vcResult(vcRight.y * vcForward.z - vcRight.z * vcForward.y,
                         vcRight.z * vcForward.x - vcRight.x * vcForward.z,
                         vcRight.x * vcForward.y - vcRight.y * vcForward.x, 
                         0.0f);
        Copy(vcResult);
    }

    //  内積
    inline float Dot(const CVector &vc) const
    {
        return x * vc.x + y * vc.y + z * vc.z;
    }

    //  長さの二乗
    inline float Length2() const
    {
        return x * x + y * y + z * z;
    }

    //  長さ
    inline float Length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }
	
    //  X軸周り角度
    inline float AtanX() const
    {
        return atan2f(z, y);
    }

    //  Y軸周り角度
    inline float AtanY() const
    {
        return atan2f(x, z);
    }

    //  Z軸周り角度
    inline float AtanZ() const
    {
        return atan2f(y, x);
    }

    //  正規化（元の長さを返す）
    inline float Normalize(float fLen=1.0f)
    {
        float fLength = Length2();
        if (fLength == 0.0f) return 0;          //エラー
        fLength = sqrtf(fLength);
        Scale(fLen/fLength);
        return fLength;
    }

    //  正規化結果代入（元の長さを返す）
    inline float Normalize(const CVector &vc, float fLen=1.0f)
    {
        float fLength = vc.Length2();
        if (fLength == 0.0f) return 0;          //エラー
        fLength = sqrtf(fLength);
        Scale(vc, fLen/fLength);
        return fLength;
    }

	// 回転（rotv.SetAngle(rad) で設定した VECTOR2D で回転）
    inline void RotateZ(const VECTOR2D &rotv)
    {
        float bufx = x;
        x = x * rotv.x - y * rotv.y;
        y = bufx * rotv.y + y * rotv.x;
    }
	
    // 比較小
    inline void Min(const CVector& v)
    {
        if (x > v.x) x = v.x;
        if (y > v.y) y = v.y;
        if (z > v.z) z = v.z;
    }

    // 比較大
    inline void Max(const CVector& v)
    {
        if (x < v.x) x = v.x;
        if (y < v.y) y = v.y;
        if (z < v.z) z = v.z;
    }
	
//--------------------------------------------------
//  クォータニオン計算
//--------------------------------------------------
    inline bool IsQZero() const      {return (((*(DWORD*)(&x)) | (*(DWORD*)(&y)) | (*(DWORD*)(&z)) | (*(DWORD*)(&w))) == 0);}
    inline bool IsQNormlized() const {return ((x * x + y * y + z * z + w * w) == 1.0f);}

    //  加算
    inline void QAdd(const CVector &vc)
    {
        x += vc.x;
        y += vc.y;
        z += vc.z;
        w += vc.w;
    }

    //  加算結果代入
    inline void QAdd(const CVector &vc1, const CVector &vc2)
    {
        x = vc1.x + vc2.x;
        y = vc1.y + vc2.y;
        z = vc1.z + vc2.z;
        w = vc1.w + vc2.w;
    }

    //  スケーリング後加算 (vcDirection * fScale)
    inline void QAdd(const CVector &vcDirection, const float fScale)
    {
        x += vcDirection.x * fScale;
        y += vcDirection.y * fScale;
        z += vcDirection.z * fScale;
        w += vcDirection.w * fScale;
    }

    //  スケーリング後加算の結果代入 (vcPoint + vcDirection * fScale)
    inline void QAdd(const CVector &vcPoint, const CVector &vcDirection, const float fScale)
    {
        x = vcPoint.x + vcDirection.x * fScale;
        y = vcPoint.y + vcDirection.y * fScale;
        z = vcPoint.z + vcDirection.z * fScale;
        w = vcPoint.w + vcDirection.w * fScale;
    }

    //  重み付け加算結果代入 (vc1 * fScale1 + vc2 * fScale2)
    inline void QAdd(const CVector &vc1, const float fScale1, const CVector &vc2, const float fScale2)
    {
        x = vc1.x * fScale1 + vc2.x * fScale2;
        y = vc1.y * fScale1 + vc2.y * fScale2;
        z = vc1.z * fScale1 + vc2.z * fScale2;
        w = vc1.w * fScale1 + vc2.w * fScale2;
    }

    //  減算
    inline void QSub(const CVector &vc)
    {
        x -= vc.x;
        y -= vc.y;
        z -= vc.z;
        w -= vc.w;
    }

    //  減算結果代入
    inline void QSub(const CVector &vc1, const CVector &vc2)
    {
        x = vc1.x - vc2.x;
        y = vc1.y - vc2.y;
        z = vc1.z - vc2.z;
        w = vc1.w - vc2.w;
    }

    //  スケーリング
    inline void QScale(const float fScale)
    {
        x *= fScale;
        y *= fScale;
        z *= fScale;
        w *= fScale;
    }

    //  スケーリング結果代入
    inline void QScale(const CVector &vc, const float fScale)
    {
        x = vc.x * fScale;
        y = vc.y * fScale;
        z = vc.z * fScale;
        w = vc.w * fScale;
    }

    //  クォータニオン乗算
    inline void QMult(const CVector &qt)
    {
        CVector me(x, y ,z, w);
        QMult(me, qt);
    }

    //  クォータニオン乗算結果代入
    inline void QMult(const CVector &qtL, const CVector &qtR)
    {
        x = qtL.w * qtR.x + qtL.x * qtR.w + qtL.y * qtR.z - qtL.z * qtR.y;
        y = qtL.w * qtR.y + qtL.y * qtR.w + qtL.z * qtR.x - qtL.x * qtR.z;
        z = qtL.w * qtR.z + qtL.z * qtR.w + qtL.x * qtR.y - qtL.y * qtR.x;
        w = qtL.w * qtR.w - qtL.Dot(qtR);
    }

    //  逆クォータニオン乗算（qt は，要QNormalize()済み）
    inline void QMultInvert(const CVector &qt)
    {
        CVector me(x, y ,z, w);
        QMultInvert(me, qt);
    }

    //  逆クォータニオン乗算結果代入（qtR は，要QNormalize()済み）
    inline void QMultInvert(const CVector &qtL, const CVector &qtR)
    {
        x = - qtL.w * qtR.x + qtL.x * qtR.w - qtL.y * qtR.z + qtL.z * qtR.y;
        y = - qtL.w * qtR.y + qtL.y * qtR.w - qtL.z * qtR.x + qtL.x * qtR.z;
        z = - qtL.w * qtR.z + qtL.z * qtR.w - qtL.x * qtR.y + qtL.y * qtR.x;
        w = qtL.w * qtR.w + qtL.Dot(qtR);
    }

    //  vcAxis 軸で rad[rad] 回転させる Quaternionを設定．QNormalize()済み
    inline void QLoadRotate(const CVector &vcAxis, float rad)
    {
        rad *= 0.5f;
        Normalize(vcAxis);
        Scale(sinf(rad));
        w = cosf(rad);
    }

    //  Bank;傾き(z)->Pitch;首上下(x)->Head;首左右(y) 回転 Quaternionを設定．QNormalize()済み
    inline void QLoadRotate(float fHead, float fPitch, float fBank)
    {
        fHead  *= 0.5f;
        fPitch *= 0.5f;
        fBank  *= 0.5f;
        float snH = sinf(fHead);
        float snP = sinf(fPitch);
        float snB = sinf(fBank);
        float csH = cosf(fHead);
        float csP = cosf(fPitch);
        float csB = cosf(fBank);
        x = csB * snP * csH - snB * csP * snH;
        y = csB * csP * snH + snB * snP * csH;
        z = snB * csP * csH - csB * snP * snH;
        w = csB * csP * csH + snB * snP * snH;
    }

    //  qtRotate * vc * QInvert(qtRotate);  ※qtRotate は，要QNormalize()済み
    inline void QRotate(const CVector &vc, const CVector &qtRotate)
    {
        CVector buf;
        buf.QMult(qtRotate, vc);
        QMultInvert(buf, qtRotate);
    }

    //  qtRotate * vc * QInvert(qtRotate);  ※qtRotate は，要QNormalize()済み
    inline void QRotate(const CVector &qtRotate)
    {
        CVector buf;
        buf.QMult(qtRotate, *this);
        QMultInvert(buf, qtRotate);
    }

    //  逆クォータニオン (IsQNormalize() == true なら Invert() := QInvert())
    inline bool QInvert()
    {
        float nlm = QLength2();
        if (nlm == 0) return false;
        nlm = 1/ nlm;
        Invert();
        Scale(nlm);
        return true;
    }

    //  逆クォータニオン (IsQNormalize() == true なら Invert() := QInvert())
    inline bool QInvert(const CVector &qt)
    {
        float nlm = qt.QLength2();
        if (nlm == 0) return false;
        nlm = 1/ nlm;
        Invert(qt);
        Scale(nlm);
        return true;
    }

    //  クォータニオン長さの二乗
    inline float QLength2() const
    {
        return x * x + y * y + z * z + w * w;
    }

    //  クォータニオン長さ
    inline float QLength() const
    {
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    //  クォータニオン正規化（長さを返す）
    inline float QNormalize()
    {
        float fLength = QLength2();
        if (fLength == 0.0f) return 0;          //エラー
        fLength = sqrtf(fLength);
        QScale(1.0f/fLength);
        return fLength;
    }

    //  クォータニオン正規化結果代入（長さを返す）
    inline float QNormalize(const CVector &vc)
    {
        float fLength = vc.QLength2();
        if (fLength == 0.0f) return 0;          //エラー
        fLength = sqrtf(fLength);
        QScale(vc, 1.0f/fLength);
        return fLength;
    }

//--------------------------------------------------
//  ４並列演算
//--------------------------------------------------
    //  ４並列乗算
    inline void Mult(const CVector &vc)
    {
        x *= vc.x;
        y *= vc.y;
        z *= vc.z;
        w *= vc.w;
    }

    //  ４並列乗算結果代入
    inline void Mult(const CVector &vc1, const CVector &vc2)
    {
        x = vc1.x * vc2.x;
        y = vc1.y * vc2.y;
        z = vc1.z * vc2.z;
        w = vc1.w * vc2.w;
    }
};

// 線分
struct Segment {
    CVector vcStart;
    CVector vcEnd;
};

// 無限直線
struct Line {
    CVector vcPoint;
    CVector vcDirection;
};

// 無限平面
struct Face {
    CVector vcPoint;
    CVector vcNormal;
};


//--------------------------------------------------------------------------------
//  クォータニオンクラス．
//  演算子のオーバーライドで CVector 内のクォータニオン計算を呼び出すだけ．
//--------------------------------------------------------------------------------
class CQuaternion : public CVector
{
public:
    CQuaternion(){}
    inline CVector  operator -  () const                    {return CVector(-x, -y, -z, -w);}
    inline CVector  operator +  (const CVector &v) const    {CVector r(*this); r.QAdd(v);    return r;}
    inline CVector  operator -  (const CVector &v) const    {CVector r(*this); r.QSub(v);    return r;}
    inline CVector  operator *  (const float &f) const      {CVector r(*this); r.QScale(f);  return r;}
    inline CVector  operator *  (const CVector &v) const    {CVector r; r.QMult(*this, v);       return r;}
    inline CVector  operator /  (const CVector &v) const    {CVector r; r.QMultInvert(*this, v); return r;}
    inline CVector& operator += (const CVector &v)          {QAdd(v);        return *this;}
    inline CVector& operator -= (const CVector &v)          {QSub(v);        return *this;}
    inline CVector& operator *= (const float &f)            {QScale(f);      return *this;}
    inline CVector& operator *= (const CVector &v)          {QMult(v);       return *this;}
    inline CVector& operator /= (const CVector &v)          {QMultInvert(v); return *this;}
    inline bool     operator == (const CVector &v)          {return ((x==v.x) && (y==v.y) && (z==v.z) && (w==v.w));}
    inline bool     operator != (const CVector &v)          {return ((x!=v.x) || (y!=v.y) || (z!=v.z) || (w!=v.w));}
};


//--------------------------------------------------------------------------------
//  2D行列
//--------------------------------------------------------------------------------
class MATRIX2D
{
//--------------------------------------------------
//  メンバ変数
//--------------------------------------------------
public:
    float a[2][2];

//--------------------------------------------------
//  メンバ関数
//--------------------------------------------------
public:
    MATRIX2D()
    {
        a[0][0] = 0.0f;     a[0][1] = 0.0f;
        a[1][0] = 0.0f;     a[1][1] = 0.0f;
    }
    MATRIX2D(const float &m11, const float &m12, const float &m21, const float &m22)
    {
        a[0][0] = m11;      a[0][1] = m21;
        a[1][0] = m12;      a[1][1] = m22;
    }
    
    MATRIX2D(const VECTOR2D &v1, const VECTOR2D &v2)
    {
        a[0][0] = v1.x;     a[0][1] = v2.x;
        a[1][0] = v1.y;     a[1][1] = v2.y;
    }

    inline operator float*() const
    {
        return (float*)a;
    }

    // 比較
    inline bool operator == (const MATRIX2D &m) const    {return (memcmp(a, m.a, sizeof(MATRIX2D)) == 0);}
    inline bool operator != (const MATRIX2D &m) const    {return (memcmp(a, m.a, sizeof(MATRIX2D)) != 0);}
    inline bool IsZero() const                           {return (((DWORD)a[0][0] | (DWORD)a[0][1] | (DWORD)a[1][0] | (DWORD)a[1][1]) == 0);}
    inline bool IsNormalized() const                     {return (ProperValue() == 1.0f);}

    // 代入
    inline MATRIX2D& operator = (const MATRIX2D &m)
    {
        Copy(m);
        return *this;
    }

    //  設定
    inline void Set(const float &m11, const float &m12, const float &m21, const float &m22)
    {
        a[0][0] = m11;      a[0][1] = m21;
        a[1][0] = m12;      a[1][1] = m22;
    }

    //  コピー
    inline void Copy(const MATRIX2D &m)
    {
        memcpy(this, &m, sizeof(MATRIX2D));
    }

    //  正規行列の設定
    inline void LoadIdentity()
    {
        a[0][0] = 1.0f;  a[1][0] = 0.0f;
        a[0][1] = 0.0f;  a[1][1] = 1.0f;
    }

    //  0行列の設定
    inline void LoadZero()
    {
        a[0][0] = 0.0f;  a[1][0] = 0.0f;
        a[0][1] = 0.0f;  a[1][1] = 0.0f;
    }

    //  スケーリング行列を設定
    inline void LoadScale(const float x, const float y)
    {
        a[0][0] = x;     a[1][0] = 0.0f;
        a[0][1] = 0.0f;  a[1][1] = y;
    }

    //  回転行列の設定
    inline void LoadRotate(const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[0][0] =  fCos;    a[1][0] = -fSin;
        a[0][1] =  fSin;    a[1][1] =  fCos;
    }
    
    // ネガ
    inline MATRIX2D operator - () const
    {
        return MATRIX2D(-a[0][0], -a[1][0], -a[0][1], -a[1][1]);
    }

    // 加算
    inline MATRIX2D operator + (const MATRIX2D &m) const
    {
        MATRIX2D res;
        res.a[0][0] = a[0][0] + m.a[0][0];
        res.a[1][0] = a[1][0] + m.a[1][0];
        res.a[0][1] = a[0][1] + m.a[0][1];
        res.a[1][1] = a[1][1] + m.a[1][1];
        return res;
    }

    // 加算
    inline MATRIX2D& operator += (const MATRIX2D &m)
    {
        a[0][0] += m.a[0][0];
        a[1][0] += m.a[1][0];
        a[0][1] += m.a[0][1];
        a[1][1] += m.a[1][1];
        return *this;
    }

    // 減算
    inline MATRIX2D operator - (const MATRIX2D &m) const
    {
        MATRIX2D res;
        res.a[0][0] = a[0][0] - m.a[0][0];
        res.a[1][0] = a[1][0] - m.a[1][0];
        res.a[0][1] = a[0][1] - m.a[0][1];
        res.a[1][1] = a[1][1] - m.a[1][1];
        return res;
    }

    // 減算
    inline MATRIX2D& operator -= (const MATRIX2D &m)
    {
        a[0][0] -= m.a[0][0];
        a[1][0] -= m.a[1][0];
        a[0][1] -= m.a[0][1];
        a[1][1] -= m.a[1][1];
        return *this;
    }

    // スケーリング
    inline MATRIX2D operator * (const float &f) const
    {
        MATRIX2D res;
        res.a[0][0] = a[0][0] * f;
        res.a[1][0] = a[1][0] * f;
        res.a[0][1] = a[0][1] * f;
        res.a[1][1] = a[1][1] * f;
        return res;
    }

    // スケーリング
    inline MATRIX2D& operator *= (const float &f)
    {
        a[0][0] *= f;
        a[1][0] *= f;
        a[0][1] *= f;
        a[1][1] *= f;
        return *this;
    }

    // 合成
    inline MATRIX2D operator * (const MATRIX2D &m) const
    {
        MATRIX2D res;
        res.Mult(*this, m);
        return res;
    }

    // 合成
    inline MATRIX2D& operator *= (const MATRIX2D &m)
    {
        MATRIX2D res(*this);
        Mult(res, m);
        return *this;
    }

    // 固有値
    inline float ProperValue() const
    {
        return a[0][0] * a[1][1] - a[0][1] * a[1][0];
    }

    // 逆行列
    inline bool Invert()
    {
        MATRIX2D m(*this);
        return Invert(m);
    }
    
    // 逆行列
    inline bool Invert(const MATRIX2D &m)
    {
        float prop = m.ProperValue();
        if (prop == 0) return false;
        prop = 1/prop;
        a[0][0] = -m.a[1][1] * prop;
        a[1][0] =  m.a[0][1] * prop;
        a[0][1] =  m.a[1][0] * prop;
        a[1][1] = -m.a[0][0] * prop;
		return true;
    }

    // 合成
    inline void Mult(const MATRIX2D &mR)
    {
        MATRIX2D res(*this);
        Mult(res, mR);
    }

    // 合成
    inline void Mult(const MATRIX2D &mL, const MATRIX2D &mR)
    {
        a[0][0] = mL.a[0][0] * mR.a[0][0] + mL.a[1][0] * mR.a[0][1];
        a[1][0] = mL.a[0][0] * mR.a[1][0] + mL.a[1][0] * mR.a[1][1];
        a[0][1] = mL.a[0][1] * mR.a[0][0] + mL.a[1][1] * mR.a[0][1];
        a[1][1] = mL.a[0][1] * mR.a[1][0] + mL.a[1][1] * mR.a[1][1];
    }

    // スケーリング
    inline void Scale(const float &f)
    {
        a[0][0] *= f;
        a[1][0] *= f;
        a[0][1] *= f;
        a[1][1] *= f;
    }

    // スケーリング
    inline void Scale(const MATRIX2D &mL, const float &f)
    {
        a[0][0] = mL.a[0][0] * f;
        a[1][0] = mL.a[1][0] * f;
        a[0][1] = mL.a[0][1] * f;
        a[1][1] = mL.a[1][1] * f;
    }

	// 回転
    inline void Rotate(const float &rad)
    {
		MATRIX2D m(*this);
		Rotate(m, rad);
	}

    // 回転
    inline void Rotate(const MATRIX2D &mL, const float &rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[0][0] =   mL.a[0][0] * fCos + mL.a[1][0] * fSin;
        a[1][0] = - mL.a[0][0] * fSin + mL.a[1][0] * fCos;
        a[0][1] =   mL.a[0][1] * fCos + mL.a[1][1] * fSin;
        a[1][1] = - mL.a[0][1] * fSin + mL.a[1][1] * fCos;
    }
    
    // 変換
    inline void Transform(VECTOR2D &vDst, const VECTOR2D &vRight) const
    {
        float x, y;
        x = a[0][0] * vRight.x + a[1][0] * vRight.y;
        y = a[0][1] * vRight.x + a[1][1] * vRight.y;
        vDst.x = x;
        vDst.y = y;
    }

    // 変換
    inline void Transform(CVector &vDst, const CVector &vRight) const
    {
        float x, y;
        x = a[0][0] * vRight.x + a[1][0] * vRight.y;
        y = a[0][1] * vRight.x + a[1][1] * vRight.y;
        vDst.x = x;
        vDst.y = y;
    }
};


//--------------------------------------------------------------------------------
//  行列
//--------------------------------------------------------------------------------
class CMatrix
{
//--------------------------------------------------
//  メンバ変数
//--------------------------------------------------
public:
    float a[4][4];
    
//--------------------------------------------------
//  メンバ関数
//--------------------------------------------------
public:
    CMatrix()
    {
    }

    CMatrix(const CMatrix &m)
    {
        memcpy(this, &m, sizeof(CMatrix));
    }

    CMatrix(const CVector &v1, const CVector &v2, const CVector &v3, const CVector &v4)
    {
        int i;
        for (i=0; i<4; i++) a[0][i] = ((float*)v1)[i];
        for (i=0; i<4; i++) a[1][i] = ((float*)v2)[i];
        for (i=0; i<4; i++) a[2][i] = ((float*)v3)[i];
        for (i=0; i<4; i++) a[3][i] = ((float*)v4)[i];
    }

    //  演算子
    inline operator float*() const
    {
        return (float*)a;
    }

    inline CMatrix& operator = (const CMatrix &m)
    {
        Copy(m);
        return *this;
    }

    //  設定
    //  コピー
    inline void Copy(const CMatrix &m)
    {
        memcpy(this, &m, sizeof(CMatrix));
    }

    //  正規行列
    inline void LoadIdentity()
    {
        a[0][0] = 1.0f;  a[0][1] = 0.0f;  a[0][2] = 0.0f;  a[0][3] = 0.0f;
        a[1][0] = 0.0f;  a[1][1] = 1.0f;  a[1][2] = 0.0f;  a[1][3] = 0.0f;
        a[2][0] = 0.0f;  a[2][1] = 0.0f;  a[2][2] = 1.0f;  a[2][3] = 0.0f;
        a[3][0] = 0.0f;  a[3][1] = 0.0f;  a[3][2] = 0.0f;  a[3][3] = 1.0f;
    }

    //  0行列
    inline void LoadZero()
    {
        memset(a[0], 0, 16);
        memset(a[1], 0, 16);
        memset(a[2], 0, 16);
        memset(a[3], 0, 16);
    }

    //  平行移動行列を設定
    inline void LoadTranslate(const float x, const float y, const float z)
    {
        a[0][0] = 1.0f;  a[0][1] = 0.0f;  a[0][2] = 0.0f;  a[0][3] = 0.0f;
        a[1][0] = 0.0f;  a[1][1] = 1.0f;  a[1][2] = 0.0f;  a[1][3] = 0.0f;
        a[2][0] = 0.0f;  a[2][1] = 0.0f;  a[2][2] = 1.0f;  a[2][3] = 0.0f;
        a[3][0] = x;     a[3][1] = y;     a[3][2] = z;     a[3][3] = 1.0f;
    }

    //  平行移動行列を設定
    inline void LoadTranslate(const CVector &vc)
    {
        a[0][0] = 1.0f;  a[0][1] = 0.0f;  a[0][2] = 0.0f;  a[0][3] = 0.0f;
        a[1][0] = 0.0f;  a[1][1] = 1.0f;  a[1][2] = 0.0f;  a[1][3] = 0.0f;
        a[2][0] = 0.0f;  a[2][1] = 0.0f;  a[2][2] = 1.0f;  a[2][3] = 0.0f;
        a[3][0] = vc.x;  a[3][1] = vc.y;  a[3][2] = vc.z;  a[3][3] = 1.0f;
    }

    //  スケーリング行列を設定
    inline void LoadScale(const float x, const float y, const float z)
    {
        a[0][0] = x;     a[0][1] = 0.0f;  a[0][2] = 0.0f;  a[0][3] = 0.0f;
        a[1][0] = 0.0f;  a[1][1] = y;     a[1][2] = 0.0f;  a[1][3] = 0.0f;
        a[2][0] = 0.0f;  a[2][1] = 0.0f;  a[2][2] = z;     a[2][3] = 0.0f;
        a[3][0] = 0.0f;  a[3][1] = 0.0f;  a[3][2] = 0.0f;  a[3][3] = 1.0f;
    }

    //  X軸回転行列の設定
    inline void LoadRotateX(const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[1][0] = 0;
        a[1][1] = fCos;
        a[1][2] = fSin;
        a[1][3] = 0;
        a[2][0] = 0;
        a[2][1] = -fSin;
        a[2][2] = fCos;
        a[2][3] = 0;
        a[0][0] = 1;  a[0][1] = 0;  a[0][2] = 0;  a[0][3] = 0;
        a[3][0] = 0;  a[3][1] = 0;  a[3][2] = 0;  a[3][3] = 1;
    }

    //  Y軸回転行列の設定
    inline void LoadRotateY(const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[0][0] = fCos;
        a[0][1] = 0;
        a[0][2] = -fSin;
        a[0][3] = 0;
        a[2][0] = fSin;
        a[2][1] = 0;
        a[2][2] = fCos;
        a[2][3] = 0;
        a[1][0] = 0;  a[1][1] = 1;  a[1][2] = 0;  a[1][3] = 0;
        a[3][0] = 0;  a[3][1] = 0;  a[3][2] = 0;  a[3][3] = 1;
    }

    //  Z軸回転行列の設定
    inline void LoadRotateZ(const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[0][0] =  fCos;
        a[0][1] =  fSin;
        a[0][2] =  0;
        a[0][3] =  0;
        a[1][0] = -fSin;
        a[1][1] =  fCos;
        a[1][2] =  0;
        a[1][3] =  0;
        a[2][0] = 0;  a[2][1] = 0;  a[2][2] = 1;  a[2][3] = 0;
        a[3][0] = 0;  a[3][1] = 0;  a[3][2] = 0;  a[3][3] = 1;
    }

    //  回転行列を設定
    inline void LoadRotate(const CVector &vcAxis, const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        CVector vc;
        vc.Normalize(vcAxis);
        
        a[0][0] = vc.x * vc.x * (1.0f-fCos) + fCos;
        a[0][1] = vc.x * vc.y * (1.0f-fCos) + vc.z * fSin;
        a[0][2] = vc.x * vc.z * (1.0f-fCos) - vc.y * fSin;
        a[0][3] = 0.0f;
        a[1][0] = vc.y * vc.x * (1.0f-fCos) - vc.z * fSin;
        a[1][1] = vc.y * vc.y * (1.0f-fCos) + fCos;
        a[1][2] = vc.y * vc.z * (1.0f-fCos) + vc.x * fSin;
        a[1][3] = 0.0f;
        a[2][0] = vc.z * vc.x * (1.0f-fCos) + vc.y * fSin;
        a[2][1] = vc.z * vc.y * (1.0f-fCos) - vc.x * fSin;
        a[2][2] = vc.z * vc.z * (1.0f-fCos) + fCos;
        a[2][3] = 0.0f;
        a[3][0] = 0.0f;  a[3][1] = 0.0f;  a[3][2] = 0.0f;  a[3][3] = 1.0f;
    }
    
    // 回転クォータニオンから行列を得る
    inline void LoadQuaternion(const CVector &vcQuat)
    {
        float xx = vcQuat.x * vcQuat.x * 2.0f;
        float yy = vcQuat.y * vcQuat.y * 2.0f;
        float zz = vcQuat.z * vcQuat.z * 2.0f;
        float xy = vcQuat.x * vcQuat.y * 2.0f;
        float yz = vcQuat.y * vcQuat.z * 2.0f;
        float zx = vcQuat.z * vcQuat.x * 2.0f;
        float wx = vcQuat.w * vcQuat.x * 2.0f;
        float wy = vcQuat.w * vcQuat.y * 2.0f;
        float wz = vcQuat.w * vcQuat.z * 2.0f;
        a[0][0] = 1 - (yy + zz);
        a[0][1] = xy + wz;
        a[0][2] = zx - wy;
        a[0][3] = 0.0f;
        a[1][0] = xy - wz;
        a[1][1] = 1 - (xx + zz);
        a[1][2] = yz + wx;
        a[1][3] = 0.0f;
        a[2][0] = zx + wy;
        a[2][1] = yz - wx;
        a[2][2] = 1 - (xx + yy);
        a[2][3] = 0.0f;
        a[3][0] = 0.0f;  a[3][1] = 0.0f;  a[3][2] = 0.0f;  a[3][3] = 1.0f;
    }

    // 視体積変換
    inline void LoadFrustum(const float fLeft, const float fRight, const float fBottom, const float fTop, const float fZNear, const float fZFar)
    {
        a[0][0] = (2.0f*fZNear)/(fRight-fLeft);  a[1][0] = 0.0f;  a[2][0] = (fRight+fLeft)/(fRight-fLeft);  a[3][0] = 0.0f;
        a[0][1] = 0.0f;  a[1][1] = (2.0f*fZNear)/(fTop-fBottom);  a[2][1] = (fTop+fBottom)/(fTop-fBottom);  a[3][1] = 0.0f;
        a[0][2] = 0.0f;  a[1][2] = 0.0f;  a[2][2] = -(fZFar+fZNear)/(fZFar-fZNear); a[3][2] = -(2.0f*fZFar*fZNear)/(fZFar-fZNear);
        a[0][3] = 0.0f;  a[1][3] = 0.0f;  a[2][3] = -1.0f;  a[3][3] = 0.0f;
    }

    // パースペクティブカメラ
    inline void LoadPerspective(const float fFovy, const float fAspect, const float fZNear, const float fZFar)
    {
        float f = cosf(fFovy*0.5f) / sinf(fFovy*0.5f);

        a[0][0] = f/fAspect; a[1][0] = 0.0f; a[2][0] = 0.0f;  a[3][0] = 0.0f;
        a[0][1] = 0.0f;      a[1][1] = f;    a[2][1] = 0.0f;  a[3][1] = 0.0f;
        a[0][2] = 0.0f;      a[1][2] = 0.0f; a[2][2] = -(fZFar+fZNear)/(fZFar-fZNear);      a[3][2] = -(2.0f*fZFar*fZNear)/(fZFar-fZNear);
        a[0][3] = 0.0f;      a[1][3] = 0.0f;  a[2][3] = -1.0f;  a[3][3] = 0.0f;
    }

	// MATRIX2D から設定
	inline void LoadXMATRIX2D(const MATRIX2D &m2d)
	{
        a[0][0] = 1.0f;  a[0][1] = 0.0f;  a[0][2] = 0.0f;  a[0][3] = 0.0f;
        a[1][0] = 0.0f;  a[1][3] = 0.0f;
        a[2][0] = 0.0f;  a[2][3] = 0.0f;
        a[3][0] = 0.0f;  a[3][1] = 0.0f;  a[3][2] = 0.0f;  a[3][3] = 1.0f;
		a[1][1] = m2d.a[0][0];
		a[1][2] = m2d.a[0][1];
		a[2][1] = m2d.a[1][0];
		a[2][2] = m2d.a[1][1];
	}

	inline void LoadYMATRIX2D(const MATRIX2D &m2d)
	{
        a[0][1] = 0.0f;	 a[0][3] = 0.0f;
        a[1][0] = 0.0f;  a[1][1] = 1.0f;  a[1][2] = 0.0f;  a[1][3] = 0.0f;
        a[2][1] = 0.0f;  a[2][3] = 0.0f;
        a[3][0] = 0.0f;  a[3][1] = 0.0f;  a[3][2] = 0.0f;  a[3][3] = 1.0f;
		a[2][2] = m2d.a[0][0];
		a[2][0] = m2d.a[0][1];
		a[0][2] = m2d.a[1][0];
		a[0][0] = m2d.a[1][1];
	}

	inline void LoadZMATRIX2D(const MATRIX2D &m2d)
	{
        a[0][2] = 0.0f;  a[0][3] = 0.0f;
        a[1][2] = 0.0f;  a[1][3] = 0.0f;
        a[2][0] = 0.0f;  a[2][1] = 0.0f;  a[2][2] = 1.0f;  a[2][3] = 0.0f;
        a[3][0] = 0.0f;  a[3][1] = 0.0f;  a[3][2] = 0.0f;  a[3][3] = 1.0f;
		a[0][0] = m2d.a[0][0];
		a[0][1] = m2d.a[0][1];
		a[1][0] = m2d.a[1][0];
		a[1][1] = m2d.a[1][1];
	}

	// MATRIX2D の取得
	inline void GetXMATRIX2D(MATRIX2D &m2d) const 
	{
		m2d.a[0][0] = a[1][1];
		m2d.a[0][1] = a[1][2];
		m2d.a[1][0] = a[2][1];
		m2d.a[1][1] = a[2][2];
	}

	inline void GetYMATRIX2D(MATRIX2D &m2d) const 
	{
		m2d.a[0][0] = a[2][2];
		m2d.a[0][1] = a[2][0];
		m2d.a[1][0] = a[0][2];
		m2d.a[1][1] = a[0][0];
	}

	inline void GetZMATRIX2D(MATRIX2D &m2d) const 
	{
		m2d.a[0][0] = a[0][0];
		m2d.a[0][1] = a[0][1];
		m2d.a[1][0] = a[1][0];
		m2d.a[1][1] = a[1][1];
	}

    //  固有値(笑)
    inline float ProperValue() const
    {
        return   a[0][0]* (a[1][1]* (-a[2][2]*a[3][3] + a[2][3]*a[3][2]) + a[1][2]* ( a[2][1]*a[3][3] - a[3][1]*a[2][3]) + a[1][3]* (-a[2][1]*a[3][2] + a[2][2]*a[3][1]))
               + a[0][1]* (a[1][0]* ( a[2][2]*a[3][3] - a[2][3]*a[3][2]) + a[2][0]* (-a[1][2]*a[3][3] + a[1][3]*a[3][2]) + a[3][0]* ( a[1][2]*a[2][3] - a[1][3]*a[2][2]))
               + a[1][0]* (a[0][2]* (-a[2][1]*a[3][3] + a[3][1]*a[2][3]) + a[0][3]* ( a[2][1]*a[3][2] - a[2][2]*a[3][1]))
               + a[0][2]* (a[1][1]* ( a[2][0]*a[3][3] - a[3][0]*a[2][3]) + a[1][3]* (-a[2][0]*a[3][1] + a[2][1]*a[3][0]))
               + a[1][1]*a[0][3]* (-a[2][0]*a[3][2] + a[3][0]*a[2][2])
               + a[0][3]*a[1][2]* ( a[2][0]*a[3][1] - a[2][1]*a[3][0]);
    }

    //  逆行列
    inline bool Invert()
    {
        CMatrix m(*this);
        return Invert(m);
    }

    //  逆行列結果代入(笑)
    inline bool Invert(const CMatrix &m)
    {
        float fProp = m.ProperValue();
        if (fProp == 0) return false;
        float fInvProp = 1 / fProp;
        a[0][0] = (m.a[1][1]* (-m.a[2][2]*m.a[3][3] + m.a[2][3]*m.a[3][2]) + m.a[1][2]* ( m.a[2][1]*m.a[3][3] - m.a[3][1]*m.a[2][3]) + m.a[1][3]* (-m.a[2][1]*m.a[3][2] + m.a[2][2]*m.a[3][1])) * fInvProp;
        a[0][1] = (m.a[0][1]* ( m.a[2][2]*m.a[3][3] - m.a[2][3]*m.a[3][2]) + m.a[0][2]* (-m.a[2][1]*m.a[3][3] + m.a[3][1]*m.a[2][3]) + m.a[0][3]* ( m.a[2][1]*m.a[3][2] - m.a[2][2]*m.a[3][1])) * fInvProp;
        a[0][2] = (m.a[0][1]* (-m.a[1][2]*m.a[3][3] + m.a[1][3]*m.a[3][2]) + m.a[0][2]* ( m.a[1][1]*m.a[3][3] - m.a[1][3]*m.a[3][1]) + m.a[0][3]* (-m.a[1][1]*m.a[3][2] + m.a[1][2]*m.a[3][1])) * fInvProp;
        a[0][3] = (m.a[0][1]* ( m.a[1][2]*m.a[2][3] - m.a[1][3]*m.a[2][2]) + m.a[0][2]* (-m.a[1][1]*m.a[2][3] + m.a[2][1]*m.a[1][3]) + m.a[0][3]* ( m.a[1][1]*m.a[2][2] - m.a[1][2]*m.a[2][1])) * fInvProp;
        a[1][0] = (m.a[1][0]* ( m.a[2][2]*m.a[3][3] - m.a[2][3]*m.a[3][2]) + m.a[2][0]* (-m.a[1][2]*m.a[3][3] + m.a[1][3]*m.a[3][2]) + m.a[3][0]* ( m.a[1][2]*m.a[2][3] - m.a[1][3]*m.a[2][2])) * fInvProp;
        a[1][1] = (m.a[0][0]* (-m.a[2][2]*m.a[3][3] + m.a[2][3]*m.a[3][2]) + m.a[0][2]* ( m.a[2][0]*m.a[3][3] - m.a[3][0]*m.a[2][3]) + m.a[0][3]* (-m.a[2][0]*m.a[3][2] + m.a[3][0]*m.a[2][2])) * fInvProp;
        a[1][2] = (m.a[0][0]* ( m.a[1][2]*m.a[3][3] - m.a[1][3]*m.a[3][2]) + m.a[1][0]* (-m.a[0][2]*m.a[3][3] + m.a[0][3]*m.a[3][2]) + m.a[3][0]* ( m.a[0][2]*m.a[1][3] - m.a[0][3]*m.a[1][2])) * fInvProp;
        a[1][3] = (m.a[0][0]* (-m.a[1][2]*m.a[2][3] + m.a[1][3]*m.a[2][2]) + m.a[1][0]* ( m.a[0][2]*m.a[2][3] - m.a[0][3]*m.a[2][2]) + m.a[2][0]* (-m.a[0][2]*m.a[1][3] + m.a[0][3]*m.a[1][2])) * fInvProp;
        a[2][0] = (m.a[1][0]* (-m.a[2][1]*m.a[3][3] + m.a[3][1]*m.a[2][3]) + m.a[1][1]* ( m.a[2][0]*m.a[3][3] - m.a[3][0]*m.a[2][3]) + m.a[1][3]* (-m.a[2][0]*m.a[3][1] + m.a[2][1]*m.a[3][0])) * fInvProp;
        a[2][1] = (m.a[0][0]* ( m.a[2][1]*m.a[3][3] - m.a[3][1]*m.a[2][3]) + m.a[0][1]* (-m.a[2][0]*m.a[3][3] + m.a[3][0]*m.a[2][3]) + m.a[0][3]* ( m.a[2][0]*m.a[3][1] - m.a[2][1]*m.a[3][0])) * fInvProp;
        a[2][2] = (m.a[0][0]* (-m.a[1][1]*m.a[3][3] + m.a[1][3]*m.a[3][1]) + m.a[0][1]* ( m.a[1][0]*m.a[3][3] - m.a[3][0]*m.a[1][3]) + m.a[0][3]* (-m.a[1][0]*m.a[3][1] + m.a[1][1]*m.a[3][0])) * fInvProp;
        a[2][3] = (m.a[0][0]* ( m.a[1][1]*m.a[2][3] - m.a[2][1]*m.a[1][3]) + m.a[0][1]* (-m.a[1][0]*m.a[2][3] + m.a[2][0]*m.a[1][3]) + m.a[0][3]* ( m.a[1][0]*m.a[2][1] - m.a[1][1]*m.a[2][0])) * fInvProp;
        a[3][0] = (m.a[1][0]* ( m.a[2][1]*m.a[3][2] - m.a[2][2]*m.a[3][1]) + m.a[1][1]* (-m.a[2][0]*m.a[3][2] + m.a[3][0]*m.a[2][2]) + m.a[1][2]* ( m.a[2][0]*m.a[3][1] - m.a[2][1]*m.a[3][0])) * fInvProp;
        a[3][1] = (m.a[0][0]* (-m.a[2][1]*m.a[3][2] + m.a[2][2]*m.a[3][1]) + m.a[0][1]* ( m.a[2][0]*m.a[3][2] - m.a[3][0]*m.a[2][2]) + m.a[0][2]* (-m.a[2][0]*m.a[3][1] + m.a[2][1]*m.a[3][0])) * fInvProp;
        a[3][2] = (m.a[0][0]* ( m.a[1][1]*m.a[3][2] - m.a[1][2]*m.a[3][1]) + m.a[0][1]* (-m.a[1][0]*m.a[3][2] + m.a[1][2]*m.a[3][0]) + m.a[0][2]* ( m.a[1][0]*m.a[3][1] - m.a[1][1]*m.a[3][0])) * fInvProp;
        a[3][3] = (m.a[0][0]* (-m.a[1][1]*m.a[2][2] + m.a[1][2]*m.a[2][1]) + m.a[0][1]* ( m.a[1][0]*m.a[2][2] - m.a[2][0]*m.a[1][2]) + m.a[0][2]* (-m.a[1][0]*m.a[2][1] + m.a[1][1]*m.a[2][0])) * fInvProp;
        return true;
    }

    //  転置
    inline void Transpose()
    {
        CMatrix m(*this);
        Transpose(m);
    }

    //  転置結果代入
    inline void Transpose(const CMatrix &m)
    {
        a[0][0] = m.a[0][0];  a[0][1] = m.a[1][0];  a[0][2] = m.a[2][0];  a[0][3] = m.a[3][0];
        a[1][0] = m.a[0][1];  a[1][1] = m.a[1][1];  a[1][2] = m.a[2][1];  a[1][3] = m.a[3][1];
        a[2][0] = m.a[0][2];  a[2][1] = m.a[1][2];  a[2][2] = m.a[2][2];  a[2][3] = m.a[3][2];
        a[3][0] = m.a[0][3];  a[3][1] = m.a[1][3];  a[3][2] = m.a[2][3];  a[3][3] = m.a[3][3];
    }

    //  合成
    inline void Mult(const CMatrix &mR)
    {
        CMatrix m(*this);
        Mult(m, mR);        
    }

    //  合成結果代入
    inline void Mult(const CMatrix &mL, const CMatrix &mR)
    {
        a[0][0] = mR.a[0][0] * mL.a[0][0] + mR.a[0][1] * mL.a[1][0] + mR.a[0][2] * mL.a[2][0] + mR.a[0][3] * mL.a[3][0];
        a[0][1] = mR.a[0][0] * mL.a[0][1] + mR.a[0][1] * mL.a[1][1] + mR.a[0][2] * mL.a[2][1] + mR.a[0][3] * mL.a[3][1];
        a[0][2] = mR.a[0][0] * mL.a[0][2] + mR.a[0][1] * mL.a[1][2] + mR.a[0][2] * mL.a[2][2] + mR.a[0][3] * mL.a[3][2];
        a[0][3] = mR.a[0][0] * mL.a[0][3] + mR.a[0][1] * mL.a[1][3] + mR.a[0][2] * mL.a[2][3] + mR.a[0][3] * mL.a[3][3];

        a[1][0] = mR.a[1][0] * mL.a[0][0] + mR.a[1][1] * mL.a[1][0] + mR.a[1][2] * mL.a[2][0] + mR.a[1][3] * mL.a[3][0];
        a[1][1] = mR.a[1][0] * mL.a[0][1] + mR.a[1][1] * mL.a[1][1] + mR.a[1][2] * mL.a[2][1] + mR.a[1][3] * mL.a[3][1];
        a[1][2] = mR.a[1][0] * mL.a[0][2] + mR.a[1][1] * mL.a[1][2] + mR.a[1][2] * mL.a[2][2] + mR.a[1][3] * mL.a[3][2];
        a[1][3] = mR.a[1][0] * mL.a[0][3] + mR.a[1][1] * mL.a[1][3] + mR.a[1][2] * mL.a[2][3] + mR.a[1][3] * mL.a[3][3];

        a[2][0] = mR.a[2][0] * mL.a[0][0] + mR.a[2][1] * mL.a[1][0] + mR.a[2][2] * mL.a[2][0] + mR.a[2][3] * mL.a[3][0];
        a[2][1] = mR.a[2][0] * mL.a[0][1] + mR.a[2][1] * mL.a[1][1] + mR.a[2][2] * mL.a[2][1] + mR.a[2][3] * mL.a[3][1];
        a[2][2] = mR.a[2][0] * mL.a[0][2] + mR.a[2][1] * mL.a[1][2] + mR.a[2][2] * mL.a[2][2] + mR.a[2][3] * mL.a[3][2];
        a[2][3] = mR.a[2][0] * mL.a[0][3] + mR.a[2][1] * mL.a[1][3] + mR.a[2][2] * mL.a[2][3] + mR.a[2][3] * mL.a[3][3];

        a[3][0] = mR.a[3][0] * mL.a[0][0] + mR.a[3][1] * mL.a[1][0] + mR.a[3][2] * mL.a[2][0] + mR.a[3][3] * mL.a[3][0];
        a[3][1] = mR.a[3][0] * mL.a[0][1] + mR.a[3][1] * mL.a[1][1] + mR.a[3][2] * mL.a[2][1] + mR.a[3][3] * mL.a[3][1];
        a[3][2] = mR.a[3][0] * mL.a[0][2] + mR.a[3][1] * mL.a[1][2] + mR.a[3][2] * mL.a[2][2] + mR.a[3][3] * mL.a[3][2];
        a[3][3] = mR.a[3][0] * mL.a[0][3] + mR.a[3][1] * mL.a[1][3] + mR.a[3][2] * mL.a[2][3] + mR.a[3][3] * mL.a[3][3];
    }

    //  平行移動行列を合成
    inline void Translate(const CVector &vc)
    {
        a[3][0] += vc.x * a[0][0] + vc.y * a[1][0] + vc.z * a[2][0];
        a[3][1] += vc.x * a[0][1] + vc.y * a[1][1] + vc.z * a[2][1];
        a[3][2] += vc.x * a[0][2] + vc.y * a[1][2] + vc.z * a[2][2];
        a[3][3] += vc.x * a[0][3] + vc.y * a[1][3] + vc.z * a[2][3];
    }

    //  平行移動行列を合成
    inline void Translate(const float x, const float y, const float z)
    {
        a[3][0] += x * a[0][0] + y * a[1][0] + z * a[2][0];
        a[3][1] += x * a[0][1] + y * a[1][1] + z * a[2][1];
        a[3][2] += x * a[0][2] + y * a[1][2] + z * a[2][2];
        a[3][3] += x * a[0][3] + y * a[1][3] + z * a[2][3];
    }

    //  平行移動行列の合成結果代入
    inline void Translate(const CMatrix &mL, const CVector &vc)                           {Copy(mL);  Translate(vc);}
    inline void Translate(const CMatrix &mL, const float x, const float y, const float z) {Copy(mL);  Translate(x, y, z);}

    //  スケーリング行列を合成
    inline void Scale(const CVector &vc)
    {
        a[0][0] *= vc.x;  a[0][1] *= vc.x;  a[0][2] *= vc.x;
        a[1][0] *= vc.y;  a[1][1] *= vc.y;  a[1][2] *= vc.y;
        a[2][0] *= vc.z;  a[2][1] *= vc.z;  a[2][2] *= vc.z;
    }

    //  スケーリング行列を合成
    inline void Scale(const float x, const float y, const float z)
    {
        a[0][0] *= x;  a[0][1] *= x;  a[0][2] *= x;
        a[1][0] *= y;  a[1][1] *= y;  a[1][2] *= y;
        a[2][0] *= z;  a[2][1] *= z;  a[2][2] *= z;
    }

    //  スケーリング行列の合成結果代入
    inline void Scale(const CMatrix &mL, const CVector &vc)                           {Copy(mL);  Scale(vc);}
    inline void Scale(const CMatrix &mL, const float x, const float y, const float z) {Copy(mL);  Scale(x, y, z);}

    //  回転を合成
    inline void RotateX(const float rad)   {CMatrix m(*this);  RotateX(m, rad);}
    inline void RotateY(const float rad)   {CMatrix m(*this);  RotateY(m, rad);}
    inline void RotateZ(const float rad)   {CMatrix m(*this);  RotateZ(m, rad);}
    inline void Rotate(const CVector &vcAxis, const float rad)  {CMatrix m(*this);  Rotate(m, vcAxis, rad);}
    inline void Rotate(const float head, const float pitch, const float bank)
    {
        RotateZ(head);
        RotateY(pitch);
        RotateX(bank);
    }
    //  クォータニオン変換
    inline void Rotate(const CVector &vcQuat)
    {
        CMatrix mat;
        mat.LoadQuaternion(vcQuat);
        Mult(mat);
    }
    
    //  X軸回転行列の合成結果代入
    //  ※ 自分を引数にすると正しい答えを返さない．その場合RotateX(const float rad)を使う事
    inline void RotateX(const CMatrix &mL, const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[1][0] =  fCos * mL.a[1][0] + fSin * mL.a[2][0];
        a[1][1] =  fCos * mL.a[1][1] + fSin * mL.a[2][1];
        a[1][2] =  fCos * mL.a[1][2] + fSin * mL.a[2][2];
        a[1][3] =  fCos * mL.a[1][3] + fSin * mL.a[2][3];
        a[2][0] = -fSin * mL.a[1][0] + fCos * mL.a[2][0];
        a[2][1] = -fSin * mL.a[1][1] + fCos * mL.a[2][1];
        a[2][2] = -fSin * mL.a[1][2] + fCos * mL.a[2][2];
        a[2][3] = -fSin * mL.a[1][3] + fCos * mL.a[2][3];
        a[0][0] = mL.a[0][0];  a[0][1] = mL.a[0][1];  a[0][2] = mL.a[0][2];  a[0][3] = mL.a[0][3];
        a[3][0] = mL.a[3][0];  a[3][1] = mL.a[3][1];  a[3][2] = mL.a[3][2];  a[3][3] = mL.a[3][3];
    }

    //  Y軸回転行列の合成結果代入
    //  ※ 自分を引数にすると正しい答えを返さない．その場合RotateY(const float rad)を使う事
    inline void RotateY(const CMatrix &mL, const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[0][0] = fCos * mL.a[0][0] - fSin * mL.a[2][0];
        a[0][1] = fCos * mL.a[0][1] - fSin * mL.a[2][1];
        a[0][2] = fCos * mL.a[0][2] - fSin * mL.a[2][2];
        a[0][3] = fCos * mL.a[0][3] - fSin * mL.a[2][3];
        a[2][0] = fSin * mL.a[0][0] + fCos * mL.a[2][0];
        a[2][1] = fSin * mL.a[0][1] + fCos * mL.a[2][1];
        a[2][2] = fSin * mL.a[0][2] + fCos * mL.a[2][2];
        a[2][3] = fSin * mL.a[0][3] + fCos * mL.a[2][3];
        a[1][0] = mL.a[1][0];  a[1][1] = mL.a[1][1];  a[1][2] = mL.a[1][2];  a[1][3] = mL.a[1][3];
        a[3][0] = mL.a[3][0];  a[3][1] = mL.a[3][1];  a[3][2] = mL.a[3][2];  a[3][3] = mL.a[3][3];
    }

    //  Z軸回転行列の合成結果代入
    //  ※ 自分を引数にすると正しい答えを返さない．その場合RotateZ(const float rad)を使う事
    inline void RotateZ(const CMatrix &mL, const float rad)
    {
        float fSin = sinf(rad);
        float fCos = cosf(rad);
        a[0][0] =  fCos * mL.a[0][0] + fSin * mL.a[1][0];
        a[0][1] =  fCos * mL.a[0][1] + fSin * mL.a[1][1];
        a[0][2] =  fCos * mL.a[0][2] + fSin * mL.a[1][2];
        a[0][3] =  fCos * mL.a[0][3] + fSin * mL.a[1][3];
        a[1][0] = -fSin * mL.a[0][0] + fCos * mL.a[1][0];
        a[1][1] = -fSin * mL.a[0][1] + fCos * mL.a[1][1];
        a[1][2] = -fSin * mL.a[0][2] + fCos * mL.a[1][2];
        a[1][3] = -fSin * mL.a[0][3] + fCos * mL.a[1][3];
        a[2][0] = mL.a[2][0];  a[2][1] = mL.a[2][1];  a[2][2] = mL.a[2][2];  a[2][3] = mL.a[2][3];
        a[3][0] = mL.a[3][0];  a[3][1] = mL.a[3][1];  a[3][2] = mL.a[3][2];  a[3][3] = mL.a[3][3];
    }

    //  任意軸回転行列の合成結果代入
    //  ※ 自分を引数にすると正しい答えを返さない．その場合Rotate(const float rad)を使う事
    inline void Rotate(const CMatrix &mL, const CVector &vcAxis, const float rad)
    {
        CMatrix mR;
        mR.LoadRotate(vcAxis, rad);
        a[0][0] = mR.a[0][0] * mL.a[0][0] + mR.a[0][1] * mL.a[1][0] + mR.a[0][2] * mL.a[2][0];
        a[0][1] = mR.a[0][0] * mL.a[0][1] + mR.a[0][1] * mL.a[1][1] + mR.a[0][2] * mL.a[2][1];
        a[0][2] = mR.a[0][0] * mL.a[0][2] + mR.a[0][1] * mL.a[1][2] + mR.a[0][2] * mL.a[2][2];
        a[0][3] = mR.a[0][0] * mL.a[0][3] + mR.a[0][1] * mL.a[1][3] + mR.a[0][2] * mL.a[2][3];
        a[1][0] = mR.a[1][0] * mL.a[0][0] + mR.a[1][1] * mL.a[1][0] + mR.a[1][2] * mL.a[2][0];
        a[1][1] = mR.a[1][0] * mL.a[0][1] + mR.a[1][1] * mL.a[1][1] + mR.a[1][2] * mL.a[2][1];
        a[1][2] = mR.a[1][0] * mL.a[0][2] + mR.a[1][1] * mL.a[1][2] + mR.a[1][2] * mL.a[2][2];
        a[1][3] = mR.a[1][0] * mL.a[0][3] + mR.a[1][1] * mL.a[1][3] + mR.a[1][2] * mL.a[2][3];
        a[2][0] = mR.a[2][0] * mL.a[0][0] + mR.a[2][1] * mL.a[1][0] + mR.a[2][2] * mL.a[2][0];
        a[2][1] = mR.a[2][0] * mL.a[0][1] + mR.a[2][1] * mL.a[1][1] + mR.a[2][2] * mL.a[2][1];
        a[2][2] = mR.a[2][0] * mL.a[0][2] + mR.a[2][1] * mL.a[1][2] + mR.a[2][2] * mL.a[2][2];
        a[2][3] = mR.a[2][0] * mL.a[0][3] + mR.a[2][1] * mL.a[1][3] + mR.a[2][2] * mL.a[2][3];
        a[3][0] = mL.a[3][0];  a[3][1] = mL.a[3][1];  a[3][2] = mL.a[3][2];  a[3][3] = mL.a[3][3];
    }
    
    //  クォータニオン変換結果代入
    inline void Rotate(const CMatrix &mL, const CVector &vcQuat)
    {
        CMatrix mat;
        mat.LoadQuaternion(vcQuat);
        Mult(mL, mat);
    }

    //Vector変換
    inline void Transform(CVector &vcDst, const CVector &vcRight) const
    {
        CVector vcResult;
        vcResult.x = a[0][0]*vcRight.x + a[1][0]*vcRight.y + a[2][0]*vcRight.z + a[3][0]*vcRight.w;
        vcResult.y = a[0][1]*vcRight.x + a[1][1]*vcRight.y + a[2][1]*vcRight.z + a[3][1]*vcRight.w;
        vcResult.z = a[0][2]*vcRight.x + a[1][2]*vcRight.y + a[2][2]*vcRight.z + a[3][2]*vcRight.w;
        vcResult.w = a[0][3]*vcRight.x + a[1][3]*vcRight.y + a[2][3]*vcRight.z + a[3][3]*vcRight.w;
        vcDst.Copy(vcResult);
    }

    //線分変換
    inline void Transform(Segment &segDst, const Segment &segRight) const
    {
        Transform(segDst.vcStart, segRight.vcStart);
        Transform(segDst.vcEnd,   segRight.vcEnd);
    }

    //無限直線変換
    inline void Transform(Line &lnDst, const Line &lnRight) const
    {
        Transform(lnDst.vcPoint,     lnRight.vcPoint);
        Transform(lnDst.vcDirection, lnRight.vcDirection);
    }

    //無限平面変換
    inline void Transform(Face &fcDst, const Face &fcRight) const
    {
        Transform(fcDst.vcPoint,  fcRight.vcPoint);
        Transform(fcDst.vcNormal, fcRight.vcNormal);
    }
};

#endif //_CORE_VECTOR_INCLUDED

