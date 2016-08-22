//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CMaterial)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// GL::CMaterial;   OpenGL Material ラップクラス
// メンバ変数はパラメータそのもの．
// 1. SetColor()で色設定→CalcParam()で光設定
// 2. Draw()で設定したMaterialを呼び出し
//--------------------------------------------------------------------------------
#ifndef _CORE_GLMATERIAL_INCLUDED
#define _CORE_GLMATERIAL_INCLUDED

#include "Vector.h"
#include "GLTexture.h"
#pragma message ("<< CORE_MESSAGE >>  include GL::CMaterial")

namespace GL {
class CMaterial
{
public:
    CVector     m_vColor;       // 材料色
    CVector     m_vAmbient;     // 環境光色
    CVector     m_vDiffuse;     // 拡散反射光色
    CVector     m_vSpecular;    // 鏡面反射光色
    float       m_fPhong;       // 鏡面反射の強さ
    CVector     m_vEmission;    // 発光色
    CTexture*   m_pTexture;     // テクスチャ（NULLで無指定）
    
public:
    // コンストラクタ
    CMaterial()
    {
        m_fPhong = 0.0f;
        m_pTexture = NULL;
    }

    // 材料色設定
    inline void SetColor(float red, float green, float blue, float alph)
    {
        m_vColor.Set(red, green, blue, alph);
    }

    // 設定された材料色から各光パラメータを計算
    inline void CalcParam(float amb, float diff, float spec, float phong, float emi)
    {
        m_vAmbient = m_vColor * amb;
        m_vDiffuse = m_vColor * diff;
        m_vSpecular = m_vColor * spec;
        m_fPhong = phong * 128.0f;
        m_vEmission = m_vColor * emi;
    }

    // BYTE[9] = {a,g,b,a,amb,dif,spec,phong,emi} を設定
    inline void SetByByte(BYTE* data)
    {
        SetColor(BYTEtoFLOAT(data[0]), BYTEtoFLOAT(data[1]), BYTEtoFLOAT(data[2]), BYTEtoFLOAT(data[3]));
        CalcParam(BYTEtoFLOAT(data[4]), BYTEtoFLOAT(data[5]), BYTEtoFLOAT(data[6]), BYTEtoFLOAT(data[7]), BYTEtoFLOAT(data[8]));
    }

    // BYTE[9] = {a,g,b,a,amb,dif,spec,phong,emi} を取得
    inline void GetByByte(BYTE* data)
    {
        float amb, diff, spec, phong, emi;
        GetParam(&amb, &diff, &spec, &phong, &emi);
        data[0] = FLOATtoBYTE(m_vColor.x);
        data[1] = FLOATtoBYTE(m_vColor.y);
        data[2] = FLOATtoBYTE(m_vColor.z);
        data[3] = FLOATtoBYTE(m_vColor.w);
        data[4] = FLOATtoBYTE(amb);
        data[5] = FLOATtoBYTE(diff);
        data[6] = FLOATtoBYTE(spec);
        data[7] = FLOATtoBYTE(phong);
        data[8] = FLOATtoBYTE(emi);
    }

    // Float値を取得
    inline void GetParam(float *amb, float *diff, float *spec, float *phong, float *emi)
    {
        float fColorLength = m_vColor.Length();
        if (fColorLength == 0.0f) {
            *amb = 0.0f;
            *diff = 0.0f;
            *spec = 0.0f;
            *phong = 0.0f;
            *emi = 0.0f;
        } else {
            *amb = m_vAmbient.Length() / fColorLength;
            *diff = m_vDiffuse.Length() / fColorLength;
            *spec = m_vSpecular.Length() / fColorLength;
            *phong = m_fPhong / 128.0f;
            *emi = m_vEmission.Length() / fColorLength;
        }
    }
    

    // glMaterial*() を呼び出す
    inline void Draw(GLenum front_back = GL_FRONT)
    {
        glColor4fv(m_vColor);
        glMaterialfv(front_back, GL_AMBIENT,   m_vAmbient);
        glMaterialfv(front_back, GL_DIFFUSE,   m_vDiffuse);
        glMaterialfv(front_back, GL_SPECULAR,  m_vSpecular);
        glMaterialf (front_back, GL_SHININESS, m_fPhong);
        glMaterialfv(front_back, GL_EMISSION,  m_vEmission);
        if (m_pTexture != NULL) m_pTexture->Bind();
        else                    glBindTexture(GL_TEXTURE_2D, 0);
    }

    // glMaterial() を呼び出す（重複設定は省く）
    inline void Compile(GLenum front_back = GL_FRONT)
    {
        CVector vc;
        if (m_pTexture != NULL) m_pTexture->Bind();

        glColor4fv(m_vColor);
        glGetMaterialfv(front_back, GL_AMBIENT,   vc);
        if (vc != m_vAmbient) glMaterialfv(front_back,  GL_AMBIENT,   m_vAmbient);
        glGetMaterialfv(front_back, GL_AMBIENT,   vc);
        if (vc != m_vDiffuse) glMaterialfv(front_back,  GL_DIFFUSE,   m_vDiffuse);
        glGetMaterialfv(front_back, GL_AMBIENT,   vc);
        if (vc != m_vSpecular) glMaterialfv(front_back, GL_SPECULAR,  m_vSpecular);
        glGetMaterialfv(front_back, GL_AMBIENT,   vc);
        if (vc.x != m_fPhong) glMaterialf (front_back,  GL_SHININESS, m_fPhong);
        glGetMaterialfv(front_back, GL_AMBIENT,   vc);
        if (vc != m_vEmission) glMaterialfv(front_back, GL_EMISSION,  m_vEmission);
    }

    // テクスチャを登録
    inline bool RegisterTexture()
    {
        if (m_pTexture != NULL) return m_pTexture->Register();
        return true;
    }

    // テクスチャの登録を抹消
    inline void UnRegisterTexture()
    {
        if (m_pTexture != NULL) m_pTexture->UnRegister();
    }
};
} //namespace GL

#endif  // _CORE_GLMATERIAL_INCLUDED
