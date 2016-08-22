//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CLight)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// GL::CLight;   OpenGL Light ���b�v�N���X
// �����o�ϐ��̓p�����[�^���̂��́D
// 0. glEnable(Lighting) / glLightModel() �� GL_SETTING ���Őݒ肵�Ă����D
// 1. Create()/CreateSpotLight()���C�g����
// 2. Set*()�Ŋe��ݒ� / �����o�ϐ��𒼐ڂ�����ꍇ�́CUpdateSetting()�Őݒ�X�V
// 3. On()�œ_��/Off()�ŏ���
// 4. Delete()�Ŕj���i�f�X�g���N�^�Ŏ����j���j 
//--------------------------------------------------------------------------------
#ifndef _CORE_GLLIGHT_INCLUDED
#define _CORE_GLLIGHT_INCLUDED

#include "Vector.h"
#pragma message ("<< CORE_MESSAGE >>  include GL::CLight")

namespace GL {
//--------------------------------------------------
// Global Light �o�^
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
    CVector     m_vPosition;        // �ʒu
    CVector     m_vAmbient;         // �����F
    CVector     m_vDiffuse;         // �g�U���ˌ��F
    CVector     m_vSpecular;        // ���ʔ��ˌ��F

    CVector     m_vSpotDirection;   // �X�|�b�g���C�g����
    float       m_fSpotCutoff;      // �X�|�b�g���C�g�p�x
    float       m_fSpotExponent;    // �X�|�b�g���C�g������
    float       m_fSpotAtt0;        // �����W��(constant)
    float       m_fSpotAtt1;        // �����W��(linear)
    float       m_fSpotAtt2;        // �����W��(quadoratic)

    GLenum      m_enLightNum;       // ���C�g�ԍ�

//--------------------------------------------------
// �R���X�g���N�^�^�f�X�g���N�^
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
// ����/�j��
//--------------------------------------------------
public:
    // ���C�g�쐬�i�o�^�j
    bool CLight::Create(const CVector &vcPos)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition = vcPos;
        m_vPosition.w = 0;
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // ���C�g�쐬�i�o�^�j
    bool CLight::Create(float x, float y, float z)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition.Set(x, y, z, 0);
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // �X�|�b�g���C�g�쐬�i�o�^�j
    bool CLight::CreateSpotLight(const CVector &vcPos)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition = vcPos;
        m_vPosition.w = 1.0f;
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // �X�|�b�g���C�g�쐬�i�o�^�j
    bool CLight::CreateSpotLight(float x, float y, float z)
    {
        m_enLightNum = GL::GenLight();
        if (m_enLightNum == 0) return false;
        m_vPosition.Set(x, y, z, 1);
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        return true;
    }

    // ���C�g�폜�i�f�X�g���N�^�ŌĂ΂��j
    void CLight::Delete()
    {
        if (m_enLightNum != 0) GL::DeleteLight(m_enLightNum);
    }

// �ȉ���Create()������ɍs�����D
//--------------------------------------------------
//  �ݒ聨�X�V
//--------------------------------------------------
public:
    // �ʒu�ݒ聨�X�V
    void CLight::SetPosition(float x, float y, float z)
    {
        m_vPosition.Set(x, y, z);
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
    }

    // ���̐F�ݒ聨�X�V
    void CLight::SetLight(const CVector &vcColor, float amb, float dif, float spec)
    {
        m_vAmbient.Scale(vcColor, amb);
        m_vDiffuse.Scale(vcColor, dif);
        m_vSpecular.Scale(vcColor, spec);
        glLightfv(m_enLightNum, GL_AMBIENT,  m_vAmbient);
        glLightfv(m_enLightNum, GL_DIFFUSE,  m_vDiffuse);
        glLightfv(m_enLightNum, GL_SPECULAR, m_vSpecular);
    }

    // �X�|�b�g���C�g�̐ݒ�i�����ƍL����j���X�V
    void CLight::SetSpotLight(const CVector &vcDirection, float angle, float exp)
    {
        m_vSpotDirection = vcDirection;
        m_fSpotCutoff = angle;
        m_fSpotExponent = exp;
        glLightfv(m_enLightNum, GL_SPOT_DIRECTION, m_vSpotDirection);
        glLightf(m_enLightNum, GL_SPOT_CUTOFF,    m_fSpotCutoff);
        glLightf(m_enLightNum, GL_SPOT_EXPONENT,  m_fSpotExponent);
    }

    // �X�|�b�g���C�g�̐ݒ�i�ʒu�ƕ����j���X�V
    void CLight::SetSpotLight(const CVector &vcPosition, const CVector &vcDirection)
    {
        m_vPosition.Set(vcPosition.x, vcPosition.y, vcPosition.z);
        m_vSpotDirection = vcDirection;
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        glLightfv(m_enLightNum, GL_SPOT_DIRECTION, m_vSpotDirection);
    }

    // �����W���̐ݒ聨�X�V
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
// �X�V
//--------------------------------------------------
public:
    // �ʒu,�����̍X�V�݂̂̏ꍇ
    void CLight::UpdatePosition()
    {
        glLightfv(m_enLightNum, GL_POSITION, m_vPosition);
        glLightfv(m_enLightNum, GL_SPOT_DIRECTION, m_vSpotDirection);
    }

    // �����o�ϐ����蓮�ł��������ꍇ�C�ݒ�̍X�V���K�v
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
// �_��/����
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
