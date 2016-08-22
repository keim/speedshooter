//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (Initialize, others)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// - DWORD �F�ݒ� 
//  void GL::ClearColordw(DWORD dwColor)  // glClearColor for DWORD
//  void GL::Colordw(DWORD dwColor)       // glColor* for DWORD
// 
// - �����֐��̐ݒ�
//  void GL::SetAlphaBlend()    // �A���t�@���� 
//  void GL::SetAddBlend()      // ���Z���� 
//  void GL::SetMultBlend()     // ��Z�����i�A���t�@�l�����j 
//  void GL::SetScreenBlend()   // �X�N���[�������i�A���t�@�l�����j
//  void GL::SetXorBlend()      // �r���I�_���a�����i�A���t�@�l�����j
//  void GL::SetInvertBlend()   // ���]�����i�A���t�@�l�����j
// 
// - Fog�̐ݒ�
//  �֐���
//    void GL::SetFog()
//  ����
//    GLfloat fFogDensity;  ���x 
//    GLfloat fFogStart;    �J�n���� 
//    GLfloat fFogEnd;      �I������ 
//    GLuint  dwFogColor;   �F 
//    GLint   iFogMode;     ���[�h(�ȗ��� GL_LINEAR) 
// 
// - DisplayList �̐���
//  �֐���
//    GLint GL::ComposeDisplayList()
//    void  GL::ComposeEnd()
//  �T�v 
//    ComposeDisplayList() �̕Ԓl�Ƃ��āC��������� DisplayList �� ID ���擾����D 
//    DisplayList ���쐬����͈͂��CComposeDisplayList() �` ComposeEnd() �ŋ��ށD
// 
// - "���ƍ����w�肷��" Projection Matrix �ݒ�
//  �֐���
//    void GL::Screen(float fWidth, float fHeight, float fZnear, float fZfar)
//    void GL::ScreenPers(float fPersAngle, float fWidth, float fHeight, float fZnear, float fZfar)
//    void GL::ScreenFrustum(float fZcamera, float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
//    void GL::ScreenOrtho(float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
//  ����
//    fWidth / fHeight; ��ʂ̍����ƕ�
//    fZnear / fZfar;   z�����̕`��͈�
//    x / y;            ��ʍ����̍��W
//    fPersAngle;       ��ʉ������̎���p
//    fZcamera;         ��ʂ���J�����܂ł�z��������
//--------------------------------------------------------------------------------

#ifndef _CORE_NAMESPACEGL_INCLUDED
#define _CORE_NAMESPACEGL_INCLUDED

#include "Math.h"
#include "GLMatrixSaver.h"
#pragma message ("<< CORE_MESSAGE >>  include namespace GL")

//--------------------------------------------------------------------------------
//  �}�N��
//--------------------------------------------------------------------------------
// DWORD->GLfloat Color
#define GLGetRValuef(dwColor)   (GLclampf)(((BYTE)(dwColor)) * 0.0039215686f)
#define GLGetGValuef(dwColor)   (GLclampf)(((BYTE)((dwColor)>>8)) * 0.0039215686f)
#define GLGetBValuef(dwColor)   (GLclampf)(((BYTE)((dwColor)>>16)) * 0.0039215686f)
#define GLGetAValuef(dwColor)   (GLclampf)(((BYTE)((dwColor)>>24)) * 0.0039215686f)
#define GLGetRValue(dwColor)    (GLubyte)(dwColor)
#define GLGetGValue(dwColor)    (GLubyte)((dwColor) >> 8)
#define GLGetBValue(dwColor)    (GLubyte)((dwColor) >> 16)
#define GLGetAValue(dwColor)    (GLubyte)((dwColor) >> 24)

// GLfloat Color->DWORD
#define GLRGBAf(r, g, b, a)     (DWORD)((BYTE)((r)*255) | (((BYTE)((g)*255)) << 8) | (((BYTE)((b)*255)) << 16) | (((BYTE)((a)*255)) << 24))

//--------------------------------------------------------------------------------
//  �֐� 
//--------------------------------------------------------------------------------
namespace GL {
// DWORD �� glClearColor 
inline void ClearColordw(DWORD dwColor)
{
    glClearColor(GLGetRValuef(dwColor), 
                 GLGetGValuef(dwColor), 
                 GLGetBValuef(dwColor),
                 GLGetAValuef(dwColor));
}

// DWORD �� glColor* 
inline void Colordw(DWORD dwColor)
{
    glColor4ub(GLGetRValue(dwColor), 
               GLGetGValue(dwColor), 
               GLGetBValue(dwColor),
               GLGetAValue(dwColor));
}

// �A���t�@�����ɐݒ�
inline void SetAlphaBlend()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ���Z�����ɐݒ�
inline void SetAddBlend()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

// ��Z�����ɐݒ�i�A���t�@�l�����j
inline void SetMultBlend()
{
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
}

// �X�N���[�������ɐݒ�i�A���t�@�l�����j
inline void SetScreenBlend()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
}

// XOR�����ɐݒ�i�A���t�@�l�����j
inline void SetXorBlend()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
}

// ���]�����ɐݒ�i�A���t�@�l�����j
inline void SetInvertBlend()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
}

// glFog() �̃��b�v
inline void SetFog(GLfloat fFogDensity, GLfloat fFogStart, GLfloat fFogEnd, GLuint dwFogColor, GLint iFogMode = GL_LINEAR)
{
    float v[4] = {GLGetRValuef(dwFogColor), 
                  GLGetGValuef(dwFogColor), 
                  GLGetBValuef(dwFogColor), 
                  GLGetAValuef(dwFogColor)};
    glFogi(GL_FOG_MODE,     iFogMode);
    glFogf(GL_FOG_DENSITY,  fFogDensity);
    glFogf(GL_FOG_START,    fFogStart);
    glFogf(GL_FOG_END,      fFogEnd);
    glFogfv(GL_FOG_COLOR,   v);
}

// glGenLists() -> glNewList(GL_COMPILE) -> glEndList()
// DisplayList ���쐬����͈͂��CCompile() �` CompileEnd() �ŋ��ށD
inline GLint ComposeDisplayList()
{
    GLint iListIndex = glGenLists(1);
    glNewList(iListIndex, GL_COMPILE);
    return iListIndex;
}

inline void ComposeEnd()
{
    glEndList();
}

// �������w�萳�����e
inline void Screen(float fWidth, float fHeight, float fZnear, float fZfar)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float w_half = fWidth * 0.5f;
    float h_half = fHeight * 0.5f;
    glOrtho(-w_half, w_half, -h_half, h_half, fZnear, fZfar);
    glMatrixMode(GL_MODELVIEW);
}

// z=0���ʂɂ����镝�����w�蓧�����e
inline void ScreenPers(float fPersAngle, float fWidth, float fHeight, float fZnear, float fZfar)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float dist = fWidth * 0.5f / tanf(DEGREEtoRADIAN(fPersAngle) * 0.5f);
    float ratio = (dist + fZnear) / dist * 0.5f;
    float w_half = fWidth * ratio;
    float h_half = fHeight * ratio;

    glFrustum(-w_half, w_half, -h_half, h_half, dist + fZnear, dist + fZfar);
    glTranslatef(0, 0, -dist);
    glMatrixMode(GL_MODELVIEW);
}

// z=0���ʂɂ����镝�����w�莋�̐ϓ��e
inline void ScreenFrustum(float fZcamera, float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float r = (fZcamera - fZnear) / fZcamera;
    glFrustum(x*r, y*r, (x+fWidth)*r, (y+fHeight)*r, fZnear - fZcamera, fZfar - fZcamera);
    glTranslatef(0, 0, fZcamera);
    glMatrixMode(GL_MODELVIEW);
}

// �������w�萳�����e
inline void ScreenOrtho(float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(x, x+fWidth, y, y+fHeight, fZnear, fZfar);
    glMatrixMode(GL_MODELVIEW);
}

}; // namespace GL

#endif //_CORE_NAMESPACEGL_INCLUDED
