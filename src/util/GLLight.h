//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CLight)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// GL::CLight;   OpenGL Light ラップクラス
// メンバ変数はパラメータそのもの．
// 0. glEnable(Lighting) / glLightModel() は GL_SETTING 内で設定しておく．
// 1. Create()/CreateSpotLight()ライト生成
// 2. Set*()で各種設定 / メンバ変数を直接いじる場合は，UpdateSetting()で設定更新
// 3. On()で点灯/Off()で消灯
// 4. Delete()で破棄（デストラクタで自動破棄） 
//--------------------------------------------------------------------------------
#ifndef _CORE_GLLIGHT_INCLUDED
#define _CORE_GLLIGHT_INCLUDED

#include "Vector.h"
#pragma message ("<< CORE_MESSAGE >>  include GL::CLight")

namespace GL {
//--------------------------------------------------
// Global Light 登録
//--------------------------------------------------
static bool   g_bLight[8] = {false, false, false, false, false, false, false, false};
static inline GLenum GenLight()
{
	for (int i=0; i<8; i++) {
		if (!g_bLight[i]) {
			g_bLight[i] = true;
			return (GLenum)(GL_LIGHT0 + i);
		}
	}
    return 0;
}

static inline void DeleteLight(GLenum LightNum)
{
    g_bLight[LightNum - GL_LIGHT0] = false;
}

//--------------------------------------------------------------------------------
// GL::CLight
//--------------------------------------------------------------------------------
class CLight
{
public:
    CVector     m_vPosition;        // 位置
    CVector     m_vAmbient;         // 環境光色
    CVector     m_vDiffuse;         // 拡散反射光色
    CVector     m_vSpecular;        // 鏡面反射光色

    CVector     m_vSpotDirection;   // スポットライト方向
    float       m_fSpotCutoff;      // スポットライト角度
    float       m_fSpotExponent;    // スポットライト減衰率
    float       m_fSpotAtt0;        // 減衰係数(constant)
    float       m_fSpotAtt1;        // 減衰係数(linear)
    float       m_fSpotAtt2;        // 減衰係数(quadoratic)

    GLenum      m_enLightNum;       // ライト番号

//--------------------------------------------------
// コンストラクタ／デストラクタ
//--------------------------------------------------
public:
    CLight()
    {
        m_enLightNum = 0;

        m_vPosition.Set(0, 0, 0, 1);
        m_vAmbient.Set(1, 1, 1, 1);
        m_vDiffuse.Set(1, 1, 1, 1);
        m_vSpecular.Set(1, 1, 1, 1);

        m_vSpotDirection.SetDirection(0, 0, -1);
        m_fSpotExponent = 0;
        m_fSpotCutoff = 180.0;
        m_fSpotAtt0 = 1.0f;
        m_fSpotAtt1 = 0;
        m_fSpotAtt2 = 0;
    }

    ~CLight()
    {
        Delete();
    }

//--------------------------------------------------
// 生成/破棄
//--------------------------------------------------
public:
    // ライト作成（登録）
    bool CLight::Create(const CVector &vcPos)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition = vcPos;
        m_vPosition.w = 0;
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // ライト作成（登録）
    bool CLight::Create(float x, float y, float z)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition.Set(x, y, z, 0);
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // スポットライト作成（登録）
    bool CLight::CreateSpotLight(const CVector &vcPos)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition = vcPos;
        m_vPosition.w = 1.0f;
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // スポットライト作成（登録）
    bool CLight::CreateSpotLight(float x, float y, float z)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition.Set(x, y, z, 1);
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // ライト削除（デストラクタで呼ばれる）
    void CLight::Delete()
    {
        if (m_enLightNum != 0) GL::DeleteLight(m_enLightNum);
    }

// 以下はCreate()成功後に行う事．
//--------------------------------------------------
//  設定→更新
//--------------------------------------------------
public:
    // 位置設定→更新
    void CLight::SetPosition(float x, float y, float z)
    {
        m_vPosition.Set(x, y, z);
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
    }

    // 光の色設定→更新
    void CLight::SetLight(const CVector &vcColor, float amb, float dif, float spec)
    {
        m_vAmbient.Scale(vcColor, amb);
        m_vDiffuse.Scale(vcColor, dif);
        m_vSpecular.Scale(vcColor, spec);
        glLightfv(m_enLightNum, GL_AMBIENT,  m_vAmbient);
        glLightfv(m_enLightNum, GL_DIFFUSE,  m_vDiffuse);
        glLightfv(m_enLightNum, GL_SPECULAR, m_vSpecular);
    }

    // スポットライトの設定（方向と広がり）→更新
    void CLight::SetSpotLight(const CVector &vcDirection, float angle, float exp)
    {
        m_vSpotDirection = vcDirection;
        m_fSpotCutoff = angle;
        m_fSpotExponent = exp;
        glLightfv(m_enLightNum, GL_SPOT_DIRECTION, m_vSpotDirection);
        glLightf(m_enLightNum, GL_SPOT_CUTOFF,    m_fSpotCutoff);
        glLightf(m_enLightNum, GL_SPOT_EXPONENT,  m_fSpotExponent);
    }

    // スポットライトの設定（位置と方向）→更新
    void CLight::SetSpotLight(const CVector &vcPosition, const CVector &vcDirection)
    {
        m_vPosition.Set(vcPosition.x, vcPosition.y, vcPosition.z);
        m_vSpotDirection = vcDirection;
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        glLightfv(m_enLightNum, GL_SPOT_DIRECTION, m_vSpotDirection);
    }

    // 減衰係数の設定→更新
    void CLight::SetSpotAtt(float t0, float t1, float t2)
    {
        m_fSpotAtt0 = t0;
        m_fSpotAtt1 = t1;
        m_fSpotAtt2 = t2;
        glLightf(m_enLightNum, GL_CONSTANT_ATTENUATION,  m_fSpotAtt0);
        glLightf(m_enLightNum, GL_LINEAR_ATTENUATION,    m_fSpotAtt1);
        glLightf(m_enLightNum, GL_QUADRATIC_ATTENUATION, m_fSpotAtt2);
    }

//--------------------------------------------------
// 更新
//--------------------------------------------------
public:
    // 位置,方向の更新のみの場合
    void CLight::UpdatePosition()
    {
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        glLightfv(m_enLightNum, GL_SPOT_DIRECTION, m_vSpotDirection);
    }

    // メンバ変数を手動でいじった場合，設定の更新が必要
    void CLight::UpdateSetting()
    {
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        glLightfv(m_enLightNum, GL_AMBIENT,  m_vAmbient);
        glLightfv(m_enLightNum, GL_DIFFUSE,  m_vDiffuse);
        glLightfv(m_enLightNum, GL_SPECULAR, m_vSpecular);
        if (m_vPosition.w != 0) {
            glLightfv(m_enLightNum, GL_SPOT_DIRECTION, m_vSpotDirection);
            glLightf (m_enLightNum, GL_SPOT_CUTOFF,    m_fSpotCutoff);
            glLightf (m_enLightNum, GL_SPOT_EXPONENT,  m_fSpotExponent);
            glLightf (m_enLightNum, GL_CONSTANT_ATTENUATION,  m_fSpotAtt0);
            glLightf (m_enLightNum, GL_LINEAR_ATTENUATION,    m_fSpotAtt1);
            glLightf (m_enLightNum, GL_QUADRATIC_ATTENUATION, m_fSpotAtt2);
        }
    }

//--------------------------------------------------
// 点灯/消灯
//--------------------------------------------------
    inline void CLight::On()
    {
        glEnable(m_enLightNum);
    }

    inline void CLight::Off()
    {
        glDisable(m_enLightNum);
    }
};
} // namespace GL

#endif  // _CORE_GLLIGHT_INCLUDED
