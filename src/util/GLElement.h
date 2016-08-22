//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (小ネタ集)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  GL::CElement, GL::CTextureSprite, GL::CCharTable, GL::CPlane
//--------------------------------------------------------------------------------
#ifndef _CORE_GLELEMENT_INCLUDED
#define _CORE_GLELEMENT_INCLUDED

#include "GLArray.h"
#include "GLTexture.h"
#pragma message ("<< CORE_MESSAGE >>  include GL::CElement")

namespace GL {
//--------------------------------------------------------------------------------
// GL::CElement
//  CArray と IndexArray によって，Elementを管理する
//  大した事はしてないので，適当にいじる．
//--------------------------------------------------------------------------------
class CElement
{
public:
    CArray  m_Array;        // 頂点配列．公開なので適当にいじる
private:
    GLint*  m_aIndex;       // インデックス配列．非公開だけどGetIndexArray() でアクセス可能

public:
    CElement()
    {
        m_aIndex = NULL;
    }

    ~CElement()
    {
        Free();
    }

//--------------------------------------------------
// 描画 (glDrawElements())
//--------------------------------------------------
    void SetInterleavedArrays()
    {
        m_Array.SetInterleavedArrays();
    }

    void Draw(GLenum enMode)
    {
    	Draw(enMode, 0, GetIndexCount());
    }
	
    void Draw(GLenum enMode, int iStartIndex, int iCount)
    {
        //glDrawElements(enMode, iCount, GL_INT, &(m_aIndex[iStartIndex]));
        glBegin(enMode);
        for (int i=iStartIndex; i<iStartIndex+iCount; i++) {
            glVertex3fv(*(m_Array.GetVertex(m_aIndex[i])));
        }
        glEnd();
    }

//--------------------------------------------------
// インデックス管理
//--------------------------------------------------
    GLint& operator [](int idx)
    {
        return m_aIndex[idx];
    }

    bool AllocIndex(int isize)
    {
        memRealloc(m_aIndex, isize);
        if (m_aIndex == NULL) return false;
        return true;
    }

    bool ExpandIndex(int isize)
    {
        memExpand(m_aIndex, isize);
        if (m_aIndex == NULL) return false;
        return true;
    }

    void Free()
    {
        memDelete(m_aIndex);
    }

    GLint* GetIndexArray()
    {
        return m_aIndex;
    }

    int GetIndexCount()
    {
        return memLength(m_aIndex);
    }
};





//--------------------------------------------------------------------------------
// GL::CTexutreSprite
//  CTexture を四角形ポリゴン(GL_TRIANGLE_FAN)に貼り付けて出力
//--------------------------------------------------------------------------------
class CTextureSprite
{
public:
    CArray      m_Rect;
    CTexture*   m_pTexture;
    CVector*    m_apVertex[4];
    VECTOR2D*   m_apTexCoord[4];

public:
    CTextureSprite()
    {
        m_Rect.SetFormat(true, false, false);
		m_Rect.Alloc(4);
        for (int i=0; i<4; i++) {
            m_apVertex[i]   = m_Rect.GetVertex(i);
            m_apTexCoord[i] = m_Rect.GetTexCoord(i);
        }
		m_pTexture = NULL;
    }

    // コピー
    void Copy(CTextureSprite* pSrc)
    {
        for (int i=0; i<4; i++) {
            m_apVertex[i]->Copy(*(pSrc->m_apVertex[i]));
            *(m_apTexCoord[i]) = *(pSrc->m_apTexCoord[i]);
        }
        m_pTexture = pSrc->m_pTexture;
    }

    // NULL ?
    inline bool IsNull()
    {
        return (m_pTexture == NULL);
    }

	// テクスチャ設定
    bool SetTexture(CTexture* pTex)
    {
        m_pTexture = pTex;
        if (pTex != NULL && !pTex->IsRegistered()) return pTex->Register();
		return true;
    }

    // テクスチャ座標の設定
    void SetTexRect(float x, float y, float w, float h)
    {
        m_apTexCoord[0]->Set(x,   1.0f-y);
        m_apTexCoord[1]->Set(x,   1.0f-y-h);
        m_apTexCoord[2]->Set(x+w, 1.0f-y-h);
        m_apTexCoord[3]->Set(x+w, 1.0f-y);
    }

    // テクスチャ座標の設定
    void TranslateTexCoord(float x, float y)
    {
        m_apTexCoord[0]->x += x;
		m_apTexCoord[0]->y -= y;
        m_apTexCoord[1]->x += x;
		m_apTexCoord[1]->y -= y;
        m_apTexCoord[2]->x += x;
		m_apTexCoord[2]->y -= y;
        m_apTexCoord[3]->x += x;
		m_apTexCoord[3]->y -= y;
    }

    // Z平面上の四角設定
    void SetRect(float x, float y, float w, float h, float z=0.0f)
    {
        m_apVertex[0]->Set(x,   y+h, z);
        m_apVertex[1]->Set(x,   y,   z);
        m_apVertex[2]->Set(x+w, y,   z);
        m_apVertex[3]->Set(x+w, y+h, z);
    }

    // Z平面上の四角設定
    void SetRect(float w, float h)
    {
        float half_w = w * 0.5f;
        float half_h = h * 0.5f;
        m_apVertex[0]->Set(-half_w,  half_h, 0);
        m_apVertex[1]->Set(-half_w, -half_h, 0);
        m_apVertex[2]->Set( half_w, -half_h, 0);
        m_apVertex[3]->Set( half_w,  half_h, 0);
    }

    // vcPos を起点にして vcDir にそって 幅 fWidth の長方形を設定
    void SetPathRect(const CVector &vcPos, const CVector &vcDir, float fWidth)
    {
        CVector vcEnd, vc;
        vcEnd.Add(vcPos, vcDir);
		vc.SetDirection(-vcDir.y, vcDir.x, 0);
        vc.Normalize(fWidth);
        m_apVertex[0]->Sub(vcPos, vc);
        m_apVertex[1]->Sub(vcEnd, vc);
        m_apVertex[2]->Add(vcEnd, vc);
        m_apVertex[3]->Add(vcPos, vc);
    }

    // vcPos を起点にして vcDir にそって vcNormal に垂直な，幅 fWidth の長方形を設定
    void SetPathRect(const CVector &vcPos, const CVector &vcDir, float fWidth, const CVector &vcNormal)
    {
        CVector vcEnd, vc;
        vcEnd.Add(vcPos, vcDir);
        vc.Cross(vcDir, vcNormal);
        vc.Normalize(fWidth);
        m_apVertex[0]->Sub(vcPos, vc);
        m_apVertex[1]->Sub(vcEnd, vc);
        m_apVertex[2]->Add(vcEnd, vc);
        m_apVertex[3]->Add(vcPos, vc);
    }
    
    // vcPos を中心にして 法線 vcNormal に垂直で vcUp方向を上とする，幅fWidth/高fHeight の長方形を設定
    void SetPlaneNormal(const CVector &vcPos, const CVector &vcNormal, const CVector &vcUp, float fWidth, float fHeight)
    {
        CVector vcHori, vcVert;
        vcHori.Cross(vcUp, vcNormal);
    	vcVert.Cross(vcHori, vcNormal);
        vcHori.Normalize(fWidth);
    	vcVert.Normalize(fHeight);
        m_apVertex[0]->Copy(vcPos);
        m_apVertex[0]->Sub(vcHori*0.5f);
        m_apVertex[0]->Sub(vcVert*0.5f);
        m_apVertex[1]->Add(*m_apVertex[0], vcHori);
        m_apVertex[2]->Add(*m_apVertex[1], vcVert);
        m_apVertex[3]->Add(*m_apVertex[0], vcVert);
    }
	
    // Z平面上の回転
    void Rotate(float fCos, float fSin)
    {
        VECTOR2D rot(fCos, fSin);
        m_apVertex[0]->RotateZ(rot);
        m_apVertex[1]->RotateZ(rot);
        m_apVertex[2]->RotateZ(rot);
        m_apVertex[3]->RotateZ(rot);
    }

    // xy拡大
    void Scale(float magx, float magy)
    {
        m_apVertex[0]->x *= magx;
        m_apVertex[1]->x *= magx;
        m_apVertex[2]->x *= magx;
        m_apVertex[3]->x *= magx;
        m_apVertex[0]->y *= magy;
        m_apVertex[1]->y *= magy;
        m_apVertex[2]->y *= magy;
        m_apVertex[3]->y *= magy;
    }

    // 移動
    void Translate(CVector &vc)
    {
        m_apVertex[0]->Add(vc);
        m_apVertex[1]->Add(vc);
        m_apVertex[2]->Add(vc);
        m_apVertex[3]->Add(vc);
    }

    // 移動
    void Translate(float x, float y)
    {
        m_apVertex[0]->x += x;
        m_apVertex[1]->x += x;
        m_apVertex[2]->x += x;
        m_apVertex[3]->x += x;
        m_apVertex[0]->y += y;
        m_apVertex[1]->y += y;
        m_apVertex[2]->y += y;
        m_apVertex[3]->y += y;
    }

    // 行列を掛ける
    void MultMatrix(const CMatrix &mat)
    {
        mat.Transform(*m_apVertex[0], *m_apVertex[0]);
        mat.Transform(*m_apVertex[1], *m_apVertex[1]);
        mat.Transform(*m_apVertex[2], *m_apVertex[2]);
        mat.Transform(*m_apVertex[3], *m_apVertex[3]);
    }
    
    // 描画
    void Draw()
    {
        m_Rect.SetInterleavedArrays();
        m_pTexture->Bind();
        m_Rect.Draw(GL_TRIANGLE_FAN);
        m_pTexture->UnBind();
    }
};






//--------------------------------------------------------------------------------
// GL::CCharTable
//  テクスチャをテーブルにして，文字列から描画するクラス
//--------------------------------------------------------------------------------
class CCharTable
{
public:
    CTextureSprite      m_OutputRect;   // 表示に用いるSprite
    VECTOR2D            m_vcTexWidth;   // 文字の大きさ[UV座標上]
    VECTOR2D            m_vcCharWidth;  // 文字の大きさ[Pixel上]
    int                 m_iTableWidth;  // 表上の横一列の文字数
    int                 m_iCharOffset;  // 先頭Ascii文字
    
public:
    CCharTable()
    {
        m_iCharOffset = 0;
        m_iTableWidth = 1;
    }
    
    // Table Texture の設定．1セルの幅/高さ，先頭Ascii文字 を指定．
    bool SetTexture(CTexture* pTex, float char_w, float char_h, int char_offset)
    {
        if (!m_OutputRect.SetTexture(pTex)) return false;
        m_iCharOffset = char_offset;
        m_vcCharWidth.Set(char_w, char_h);
        m_vcTexWidth.x = char_w/ (float)pTex->GetWidth();
        m_vcTexWidth.y = char_h/ (float)pTex->GetHeight();
        m_iTableWidth = (int)((float)pTex->GetWidth() / char_w + 0.5f);
        return true;
    }
    
    // 文字大きさを変更
    void Scale(float fScaleX, float fScaleY)
    {
        m_vcCharWidth.x *= fScaleX;
        m_vcCharWidth.y *= fScaleY;
    }
    
    // 描画（終端文字列判定）
    void Draw(float x, float y, float dx, char* aChar)
    {
        int   idx, ch, col, row;
        float tx, ty;
        m_OutputRect.m_Rect.SetInterleavedArrays();
        m_OutputRect.m_pTexture->Bind();
        for (idx = 0; aChar[idx] != '\0'; idx++) {
            ch = *(BYTE*)(&aChar[idx]) - m_iCharOffset;
            col = (int)(ch / m_iTableWidth);
            row = ch - col * m_iTableWidth;
            ty = col * m_vcTexWidth.y;
            tx = row * m_vcTexWidth.x;
            m_OutputRect.SetTexRect(tx, ty, m_vcTexWidth.x, m_vcTexWidth.y);
            m_OutputRect.SetRect(x, y, m_vcCharWidth.x, m_vcCharWidth.y);
            m_OutputRect.m_Rect.Draw(GL_TRIANGLE_FAN);
            x += dx;
        }
        m_OutputRect.m_pTexture->UnBind();
    }

    // 描画（文字数指定）
    void Draw(float x, float y, float dx, char* aChar, int length)
    {
        int   idx, ch, col, row;
        float tx, ty;
        for (idx = 0; idx < length; idx++) {
            ch = *(BYTE*)(&aChar[idx]) - m_iCharOffset;
            if (ch >= 0) {
                col = (int)(ch / m_iTableWidth);
                row = ch - col * m_iTableWidth;
                ty = col * m_vcTexWidth.y;
                tx = row * m_vcTexWidth.x;
                m_OutputRect.SetTexRect(tx, ty, m_vcTexWidth.x, m_vcTexWidth.y);
                m_OutputRect.SetRect(x, y, m_vcCharWidth.x, m_vcCharWidth.y);
    			m_OutputRect.Draw();
            }
            x += dx;
        }
    }
};






//--------------------------------------------------------------------------------
// GL::CPlane
//  CElement で平面モデルを管理．1Cellで1つの GL_TRIANGLE_FAN として管理
//--------------------------------------------------------------------------------
class CPlane
{
public:
    int         cx, cy;         // x,y 分割数

private:
    int         m_iFaceCount;   // 面数
    CElement    m_Element;      // 頂点配列

public:
    CPlane()
    {
        cx = 0;
        cy = 0;
        m_iFaceCount = 0;
    }

    // 作成
    // fLeft,  fBottom で平面左下座標
    // fWidth, fHeight で平面大きさ
    // divx, divy で平面分割数
    // umax, vmax でuv座標最大値．
    // bVertexColor で頂点色を管理するか
    int Alloc(float fLeft, 
              float fBottom, 
              float fWidth, 
              float fHeight, 
              int divx, 
              int divy, 
              float umax=1.0f, 
              float vmax=1.0f, 
              bool bVertexColor=false)
    {
        int     vcnt = (divx+1) * (divy+1) + divx * divy;   // 頂点数
        int     icnt = divx * divy * 6;                     // インデックス数

        // 頂点領域確保
        CArray* pArray = &(m_Element.m_Array);  // 頂点データ
        pArray->SetFormat(true, (!bVertexColor), bVertexColor);
        if (!pArray->Alloc(vcnt)) return 0;
        // インデックス領域確保
        if (!m_Element.AllocIndex(icnt)) {
            pArray->Free();
            return 0;
        }
        int     *aIndex = m_Element.GetIndexArray(); // インデックス配列

        // 頂点初期化
        int i, j, add, cidx, vidx;
        float x, y, u, v;
        float dx = fWidth  / divx;
        float dy = fHeight / divx;
        float du = umax    / divx;
        float dv = vmax    / divy;
        add = 0;
        for (i=0, y=fBottom+dy*0.5f, v=dv*0.5f; i<divy; i++, y+=dy, v+=dv) {
            for (j=0, x=fLeft+dx*0.5f, u=du*0.5f; j<divx; j++, x+=dx, u+=du) {
                // 面中心点位置
                pArray->GetVertex(add)->SetPosition(x, y, 0);
                // UV座標
                if (pArray->IsSupportTexCoord()) pArray->GetTexCoord(add)->Set(u, v);
                // 法線
                if (pArray->IsSupportNormal())   (pArray->GetNormal(add))[2] = 1.0f;
                // 頂点色
                else                             *(pArray->GetColor(add)) = 0xffffffff;
                add++;
            }
        }
        for (i=0, y=fBottom, v=0; i<=divy; i++, y+=dy, v+=dv) {
            for (j=0, x=fLeft, u=0; j<=divx; j++, x+=dx, u+=du) {
                // 頂点位置
                pArray->GetVertex(add)->SetPosition(x, y, 0);
                // UV座標
                if (pArray->IsSupportTexCoord()) pArray->GetTexCoord(add)->Set(u, v);
                // 法線
                if (pArray->IsSupportNormal())   (pArray->GetNormal(add))[2] = 1.0f;
                // 頂点色
                else                             *(pArray->GetColor(add)) = 0xffffffff;
                add++;
            }
        }

        // インデックス初期化
        add = 0;
        cidx = 0;
        vidx = divx * divy;
        for (i=0; i<divy; i++) {
            for (j=0; j<divx; j++) {
                aIndex[add++] = cidx;
                aIndex[add++] = vidx;
                aIndex[add++] = vidx + 1;
                aIndex[add++] = vidx + divx + 2;
                aIndex[add++] = vidx + divx + 1;
                aIndex[add++] = vidx;
                cidx++;
                vidx++;
            }
            vidx++;
        }

        cx = divx;
        cy = divy;
        m_iFaceCount = cx * cy;
        return m_Element.GetIndexCount();
    }

    // 頂点変換(法線は再計算しない)
    void MultMatrix(CMatrix &mat)
    {
        CArray *pArray = &(m_Element.m_Array);
        int imax = pArray->GetDataCount();
        CVector *pv;
        for (int i=0; i<imax; i++) {
            pv = pArray->GetVertex(i);
            mat.Transform(*pv, *pv);
        }
    }

    // 中心点取得
    inline CVector* GetCenterPoint(int x, int y)
    {
        return m_Element.m_Array.GetVertex(y*cx+x);
    }

    // 頂点取得
    inline CVector* GetVertex(int x, int y)
    {
        return m_Element.m_Array.GetVertex(m_iFaceCount+y*(cx+1)+x);
    }

    // 描画
    void Draw()
    {
        int i;
        int imax = m_iFaceCount * 6;
/**/        
/*
        動かない... (´・ω・｀）
        m_Element.SetInterleavedArrays();
        for (i=0; i<imax; i+=6) m_Element.Draw(GL_TRIANGLE_FAN, i, 6);
*/

        CArray* pArray = &(m_Element.m_Array);
        int*    aIndex = m_Element.GetIndexArray();
        m_Element.SetInterleavedArrays();
        if (pArray->IsSupportNormal()) {
            for (i=0; i<imax; i+=6) {
                glBegin(GL_TRIANGLE_FAN);
                    glTexCoord2fv(*(pArray->GetTexCoord(aIndex[i])));
                    glNormal3fv(    pArray->GetNormal  (aIndex[i]) );
                    glVertex3fv(  *(pArray->GetVertex  (aIndex[i])));

                    glTexCoord2fv(*(pArray->GetTexCoord(aIndex[i+1])));
                    glNormal3fv(    pArray->GetNormal  (aIndex[i+1]) );
                    glVertex3fv(  *(pArray->GetVertex  (aIndex[i+1])));

                    glTexCoord2fv(*(pArray->GetTexCoord(aIndex[i+2])));
                    glNormal3fv(    pArray->GetNormal  (aIndex[i+2]) );
                    glVertex3fv(  *(pArray->GetVertex  (aIndex[i+2])));

                    glTexCoord2fv(*(pArray->GetTexCoord(aIndex[i+3])));
                    glNormal3fv(    pArray->GetNormal  (aIndex[i+3]) );
                    glVertex3fv(  *(pArray->GetVertex  (aIndex[i+3])));

                    glTexCoord2fv(*(pArray->GetTexCoord(aIndex[i+4])));
                    glNormal3fv(    pArray->GetNormal  (aIndex[i+4]) );
                    glVertex3fv(  *(pArray->GetVertex  (aIndex[i+4])));

                    glTexCoord2fv(*(pArray->GetTexCoord(aIndex[i+5])));
                    glNormal3fv(    pArray->GetNormal  (aIndex[i+5]) );
                    glVertex3fv(  *(pArray->GetVertex  (aIndex[i+5])));
                glEnd();
            }
        } else {
            for (i=0; i<imax; i+=6) {
                glBegin(GL_TRIANGLE_FAN);
                    glTexCoord2fv(   *(pArray->GetTexCoord(aIndex[i])));
                    glColor4ubv((BYTE*)pArray->GetColor   (aIndex[i]) );
                    glVertex3fv(     *(pArray->GetVertex  (aIndex[i])));

                    glTexCoord2fv(   *(pArray->GetTexCoord(aIndex[i+1])));
                    glColor4ubv((BYTE*)pArray->GetColor   (aIndex[i+1]) );
                    glVertex3fv(     *(pArray->GetVertex  (aIndex[i+1])));

                    glTexCoord2fv(   *(pArray->GetTexCoord(aIndex[i+2])));
                    glColor4ubv((BYTE*)pArray->GetColor   (aIndex[i+2]) );
                    glVertex3fv(     *(pArray->GetVertex  (aIndex[i+2])));

                    glTexCoord2fv(   *(pArray->GetTexCoord(aIndex[i+3])));
                    glColor4ubv((BYTE*)pArray->GetColor   (aIndex[i+3]) );
                    glVertex3fv(     *(pArray->GetVertex  (aIndex[i+3])));

                    glTexCoord2fv(   *(pArray->GetTexCoord(aIndex[i+4])));
                    glColor4ubv((BYTE*)pArray->GetColor   (aIndex[i+4]) );
                    glVertex3fv(     *(pArray->GetVertex  (aIndex[i+4])));

                    glTexCoord2fv(   *(pArray->GetTexCoord(aIndex[i+5])));
                    glColor4ubv((BYTE*)pArray->GetColor   (aIndex[i+5]) );
                    glVertex3fv(     *(pArray->GetVertex  (aIndex[i+5])));
                glEnd();
            }
        }
    }
};
} //namespace GL

#endif  // _CORE_GLELEMENT_INCLUDED
