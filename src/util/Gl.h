//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (Initialize, others)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// - DWORD 色設定 
//  void GL::ClearColordw(DWORD dwColor)  // glClearColor for DWORD
//  void GL::Colordw(DWORD dwColor)       // glColor* for DWORD
// 
// - 合成関数の設定
//  void GL::SetAlphaBlend()    // アルファ合成 
//  void GL::SetAddBlend()      // 加算合成 
//  void GL::SetMultBlend()     // 乗算合成（アルファ値無効） 
//  void GL::SetScreenBlend()   // スクリーン合成（アルファ値無効）
//  void GL::SetXorBlend()      // 排他的論理和合成（アルファ値無効）
//  void GL::SetInvertBlend()   // 反転合成（アルファ値無効）
// 
// - Fogの設定
//  関数名
//    void GL::SetFog()
//  引数
//    GLfloat fFogDensity;  密度 
//    GLfloat fFogStart;    開始距離 
//    GLfloat fFogEnd;      終了距離 
//    GLuint  dwFogColor;   色 
//    GLint   iFogMode;     モード(省略時 GL_LINEAR) 
// 
// - DisplayList の生成
//  関数名
//    GLint GL::ComposeDisplayList()
//    void  GL::ComposeEnd()
//  概要 
//    ComposeDisplayList() の返値として，生成される DisplayList の ID を取得する． 
//    DisplayList を作成する範囲を，ComposeDisplayList() ～ ComposeEnd() で挟む．
// 
// - "幅と高さ指定する" Projection Matrix 設定
//  関数名
//    void GL::Screen(float fWidth, float fHeight, float fZnear, float fZfar)
//    void GL::ScreenPers(float fPersAngle, float fWidth, float fHeight, float fZnear, float fZfar)
//    void GL::ScreenFrustum(float fZcamera, float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
//    void GL::ScreenOrtho(float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
//  引数
//    fWidth / fHeight; 画面の高さと幅
//    fZnear / fZfar;   z方向の描画範囲
//    x / y;            画面左下の座標
//    fPersAngle;       画面横方向の視野角
//    fZcamera;         画面からカメラまでのz方向距離
//--------------------------------------------------------------------------------

#ifndef _CORE_NAMESPACEGL_INCLUDED
#define _CORE_NAMESPACEGL_INCLUDED

#include "Math.h"
#include "GLMatrixSaver.h"
#pragma message ("<< CORE_MESSAGE >>  include namespace GL")

//--------------------------------------------------------------------------------
//  マクロ
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
//  関数 
//--------------------------------------------------------------------------------
namespace GL {
// DWORD 版 glClearColor 
inline void ClearColordw(DWORD dwColor)
{
    glClearColor(GLGetRValuef(dwColor), 
                 GLGetGValuef(dwColor), 
                 GLGetBValuef(dwColor),
                 GLGetAValuef(dwColor));
}

// DWORD 版 glColor* 
inline void Colordw(DWORD dwColor)
{
    glColor4ub(GLGetRValue(dwColor), 
               GLGetGValue(dwColor), 
               GLGetBValue(dwColor),
               GLGetAValue(dwColor));
}

// アルファ合成に設定
inline void SetAlphaBlend()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// 加算合成に設定
inline void SetAddBlend()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

// 乗算合成に設定（アルファ値無効）
inline void SetMultBlend()
{
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
}

// スクリーン合成に設定（アルファ値無効）
inline void SetScreenBlend()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
}

// XOR合成に設定（アルファ値無効）
inline void SetXorBlend()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
}

// 反転合成に設定（アルファ値無効）
inline void SetInvertBlend()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
}

// glFog() のラップ
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
// DisplayList を作成する範囲を，Compile() ～ CompileEnd() で挟む．
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

// 幅高さ指定正方投影
inline void Screen(float fWidth, float fHeight, float fZnear, float fZfar)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float w_half = fWidth * 0.5f;
    float h_half = fHeight * 0.5f;
    glOrtho(-w_half, w_half, -h_half, h_half, fZnear, fZfar);
    glMatrixMode(GL_MODELVIEW);
}

// z=0平面における幅高さ指定透視投影
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

// z=0平面における幅高さ指定視体積投影
inline void ScreenFrustum(float fZcamera, float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float r = (fZcamera - fZnear) / fZcamera;
    glFrustum(x*r, y*r, (x+fWidth)*r, (y+fHeight)*r, fZnear - fZcamera, fZfar - fZcamera);
    glTranslatef(0, 0, fZcamera);
    glMatrixMode(GL_MODELVIEW);
}

// 幅高さ指定正方投影
inline void ScreenOrtho(float x, float y, float fWidth, float fHeight, float fZnear, float fZfar)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(x, x+fWidth, y, y+fHeight, fZnear, fZfar);
    glMatrixMode(GL_MODELVIEW);
}

}; // namespace GL

#endif //_CORE_NAMESPACEGL_INCLUDED
