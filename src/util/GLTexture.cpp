//--------------------------------------------------------------------------------
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
#include "std_include.h"
#include "include_opengl.h"

namespace GL {
CTexture::CTexture()
{
    m_pData = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iBytes = 1;
    m_uiID = 0;

    m_bMipmap = true;
    m_iMinFilter = GL_NEAREST_MIPMAP_NEAREST;
    m_iMagFilter = GL_NEAREST;
    m_iWrapS = GL_REPEAT;
    m_iWrapT = GL_REPEAT;

    m_enFormat = 0;
    m_enTarget = GL_TEXTURE_2D;
}

CTexture::~CTexture()
{
    Delete();
}

// 作成
bool CTexture::Create(int width, int height, int bytes, const GLubyte* pixels/*=NULL*/)
{
    if (!AllocPixelArea(width, height, bytes)) return false;
    if (pixels == NULL) {
        memset(m_pData, 0, m_iWidth * m_iHeight * m_iBytes);
    } else {
        memcpy(m_pData, pixels, width * height * bytes);
    }              
    return true;
}

// Texture 上の (x, y) に，pixels をコピーする
bool CTexture::BitBlt(int x, int y, const GLubyte* pixels, int pix_width, int pix_height, int pix_bytes)
{
    // コピー領域の大きさ
    int cx = __min(m_iWidth  - x, pix_width);
    int cy = __min(m_iHeight - y, pix_height);
    if (cx <= 0 || cy <= 0) return true;

    // データ幅
    int pdw = pix_width * pix_bytes;  // pixel data width
    int cdw = cx *        pix_bytes;  // copy-area data width

    int py;
    for (py=0; py<cy; py++) {
        Util::step_memcpy(GetPixelData(x, y), &(pixels[py*pdw]), cdw, pix_bytes, m_iBytes);
    }
    return true;
}

// Fileから読みこみ
bool CTexture::Load(const char* strFileName, int bytes)
{
    RGBA_DATA *pRGBA = LoadRGBAData(strFileName, true);
    if (pRGBA != NULL) {
        if (AllocPixelArea(pRGBA->wWidth, pRGBA->wHeight, bytes)) {
        Util::step_memcpy(m_pData, pRGBA->abyData, m_iWidth * m_iHeight * 4, 4, bytes);
		    m_strFileName = strFileName;
        }
        DeleteRGBAData(pRGBA);
    }
    return (m_pData != NULL);
}

// Fileから読みこみ
bool CTexture::Load(const char* strFileName, DWORD dwTransColor)
{
    RGBA_DATA *pRGBA = CompRGBAData(strFileName, (unsigned char*)(&dwTransColor), true);
    if (pRGBA != NULL) {
        if (AllocPixelArea(pRGBA->wWidth, pRGBA->wHeight, 4)) {
        memcpy(m_pData, pRGBA->abyData, m_iWidth * m_iHeight * 4);
		    m_strFileName = strFileName;
        }
        DeleteRGBAData(pRGBA);
    }
    return (m_pData != NULL);
}

// File からAlphaマップの読みこみ
bool CTexture::Load(const char* strFileName, const char* strAlphaName)
{
    RGBA_DATA *pRGBA = CompRGBAData(strFileName, strAlphaName, true);
    if (pRGBA != NULL) {
        if (AllocPixelArea(pRGBA->wWidth, pRGBA->wHeight, 4)) {
        memcpy(m_pData, pRGBA->abyData, m_iWidth * m_iHeight * 4);
		    m_strFileName  = strFileName;
		    m_strAlphaName = strAlphaName;
        }
        DeleteRGBAData(pRGBA);
    }
    return (m_pData != NULL);
}

// 透過色指定
void CTexture::Transparent(DWORD dwKeyColor)
{
    // 透過色指定
    int     addSrc, addDst;
    GLuint  uiColor;
    int iSize = m_iWidth * m_iHeight;
    for (addSrc=iSize*3-3, addDst=iSize*4-4; addDst>=0; addSrc-=3, addDst-=4) {
        uiColor = (*(GLuint*)(&m_pData[addSrc]) & 0x00ffffff);
        uiColor |= (uiColor == dwKeyColor) ? 0 : 0xff000000;
        *(GLuint*)(&m_pData[addDst]) = uiColor;
    }
}

// 登録抹消→破棄
void CTexture::Delete(bool bUnregister/*=true*/)
{
	if (bUnregister) UnRegister();
    FreePixelArea();
}

// 登録後 m_pData の変更をテクスチャに反映
void CTexture::Update()
{
    if (m_uiID == 0) return;
    glBindTexture(m_enTarget, m_uiID);
    if (m_enTarget == GL_TEXTURE_1D) {
        if (m_bMipmap)  gluBuild1DMipmaps(GL_TEXTURE_1D, m_iBytes, m_iWidth, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
        else            glTexSubImage1D(GL_TEXTURE_1D, 0, 0, m_iWidth, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
    } else {
        if (m_bMipmap)  gluBuild2DMipmaps(GL_TEXTURE_2D, m_iBytes, m_iWidth, m_iHeight, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
        else            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iWidth, m_iHeight, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
    }
    glBindTexture(m_enTarget, 0);
}

// テクスチャ生成方法の設定
void CTexture::SetParameters(bool bMipmap, bool bMinLinear, bool bMagLinear, bool bRepeatX, bool bRepeatY)
{
    m_bMipmap = bMipmap;
    if (m_bMipmap)  m_iMinFilter = (bMinLinear) ? (GL_LINEAR_MIPMAP_NEAREST) : (GL_NEAREST_MIPMAP_NEAREST);
    else            m_iMinFilter = (bMinLinear) ? (GL_LINEAR) : (GL_NEAREST);
    m_iMagFilter = (bMagLinear) ? (GL_LINEAR) : (GL_NEAREST);
    m_iWrapS = (bRepeatX) ? (GL_REPEAT) : (GL_CLAMP);
    m_iWrapT = (bRepeatY) ? (GL_REPEAT) : (GL_CLAMP);

    // 登録後なら，設定を反映させる
    if (m_uiID != 0) {
        UnRegister();
        Register();
    }
}

// テクスチャ描画方法の設定
bool CTexture::RegParameters(bool bMinLinear, bool bMagLinear, bool bRepeatX, bool bRepeatY)
{
    if (m_bMipmap)  m_iMinFilter = (bMinLinear) ? (GL_LINEAR_MIPMAP_NEAREST) : (GL_NEAREST_MIPMAP_NEAREST);
    else            m_iMinFilter = (bMinLinear) ? (GL_LINEAR) : (GL_NEAREST);
    m_iMagFilter = (bMagLinear) ? (GL_LINEAR) : (GL_NEAREST);
    m_iWrapS = (bRepeatX) ? (GL_REPEAT) : (GL_CLAMP);
    m_iWrapT = (bRepeatY) ? (GL_REPEAT) : (GL_CLAMP);

    // 登録前なら登録
    if (m_uiID == 0) {
        if (!Register()) return false;
    }
    glBindTexture(m_enTarget, m_uiID);

    // 計算方法の設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_iMinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_iMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_iWrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_iWrapT);

    glBindTexture(m_enTarget, 0);
    return true;
}

// テクスチャ登録
bool CTexture::Register()
{
    if (m_uiID != 0) return true;

    // 名前取得
    glGenTextures(1 , &m_uiID);
    if (m_uiID == 0) return false;
    glBindTexture(m_enTarget, m_uiID);
/*
    // 作成可能か？
    int iFormat;
    if (m_enTarget == GL_TEXTURE_1D) {
        glTexImage1D(GL_PROXY_TEXTURE_1D, 0, m_iBytes, m_iWidth, 0, m_enFormat, GL_UNSIGNED_BYTE, NULL);
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_1D, 0 ,GL_TEXTURE_INTERNAL_FORMAT, &iFormat);
    } else {
        glTexImage2D(GL_PROXY_TEXTURE_2D, 0, m_iBytes, m_iWidth, m_iHeight, 0, m_enFormat, GL_UNSIGNED_BYTE, NULL);
        glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0 ,GL_TEXTURE_INTERNAL_FORMAT, &iFormat);
    }
    if (!(iFormat && m_enFormat)) return false;
*/
    // 計算方法の設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_iMinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_iMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_iWrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_iWrapT);

    // 作成
    if (m_enTarget == GL_TEXTURE_1D) {
        if (m_bMipmap)  gluBuild1DMipmaps(GL_TEXTURE_1D, m_iBytes, m_iWidth, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
        else            glTexImage1D(GL_TEXTURE_1D, 0, m_iBytes, m_iWidth, 0, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
    } else {
        if (m_bMipmap)  gluBuild2DMipmaps(GL_TEXTURE_2D, m_iBytes, m_iWidth, m_iHeight, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
        else            glTexImage2D(GL_TEXTURE_2D, 0, m_iBytes, m_iWidth, m_iHeight, 0, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
    }
    glBindTexture(m_enTarget, 0);
    return true;
}

// テクスチャ登録抹消
void CTexture::UnRegister()
{
    if (m_uiID == 0) return;
    glDeleteTextures(1, &m_uiID);
    m_uiID = 0;
}

// glCopyTexImage2D
void CTexture::CopyTexture(int x, int y)
{
    if (m_uiID == 0) return;
    glBindTexture(m_enTarget, m_uiID);
    glCopyTexImage2D(m_enTarget, 0, m_enFormat, x, y, m_iWidth, m_iHeight, 0);
    glBindTexture(m_enTarget, 0);
}

// PixelDataの領域確保
bool CTexture::AllocPixelArea(int width, int height, int bytes)
{
    if (!(bytes == 1 || bytes == 3 || bytes == 4)) return false;
    width  = Calc2N(width);
    height = Calc2N(height);

    unsigned int uiOldSize = m_iWidth * m_iHeight * m_iBytes;
    unsigned int uiSize    =    width *    height *    bytes;

    if (uiSize != uiOldSize) {
        FreePixelArea();
        m_pData    = memCreate(GLubyte, uiSize);
    }

    //GL_LUMINANCE
    GLenum setForm[4] = {GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};
    m_enFormat = setForm[bytes-1];
    m_iWidth   = width;
    m_iHeight  = height;
    m_iBytes   = bytes;
    m_iLog2Width = CalcLog2(m_iWidth);

    return true;
}

// PixelDataの領域破棄
void CTexture::FreePixelArea()
{
    memDelete(m_pData);
    m_enFormat = GL_ALPHA;
    m_iWidth   = 0;
    m_iHeight  = 0;
    m_iBytes   = 1;
    m_iLog2Width = 0;
    m_strFileName.clear();
    m_strAlphaName.clear();
}



//--------------------------------------------------
// Texture 管理クラス
//--------------------------------------------------
// コンストラクタ
CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
    UnRegisterTextures();
}

// 新規作成 
CTexture* CTextureManager::New()
{
    m_TextureList.push_front(CTexture());
    return &(m_TextureList.front());
}

// File から読みこみ
CTexture* CTextureManager::Load(char* strFileName, int byte)
{
    // 同名ファイルの検索
    CTexture* pTexture = CheckFile(strFileName);
    if (pTexture != NULL) return pTexture;

    // 新規作成
    pTexture = New();
    if (pTexture == NULL) return NULL;

    // 読みこみ
    if (!pTexture->Load(strFileName, byte)) return NULL;
    return pTexture;
}

// File から読みこみ(AlphMap付き)
CTexture* CTextureManager::Load(char* strFileName, char* strAlphFileName)
{
    // 同名ファイルの検索
    CTexture* pTexture = CheckFile(strFileName);
    if (pTexture != NULL) return pTexture;

    // 新規作成
    pTexture = New();
    if (pTexture == NULL) return NULL;

    // 読みこみ
    if (!pTexture->Load(strFileName, strAlphFileName)) return NULL;
    return pTexture;
}

// 生成する
CTexture* CTextureManager::Create(int width, int height, int bytes, const GLubyte* pixels/*=NULL*/)
{
    // 新規作成
    CTexture* pTexture = New();
    if (pTexture == NULL) return NULL;

    // Memory Area を確保していない Texture を返す
    if (width == 0 && height == 0) return pTexture; 

    // Memory Area を確保して テクスチャ生成
    if (!pTexture->Create(width, height, bytes, pixels)) {
        pTexture->Delete();
        return NULL;
    }
    return pTexture;
}

// 同一パスのファイルの検索
CTexture* CTextureManager::CheckFile(char* strFileName)
{
    std::list<CTexture>::iterator it;
    for (it = m_TextureList.begin(); it != m_TextureList.end(); it++) {
        if (stricmp(it->GetFileName(), strFileName) == 0) return &(*it);
    }
    return NULL;
}

// まとめてテクスチャ登録
int  CTextureManager::RegisterTextures()
{
    int cnt = 0;
    std::list<CTexture>::iterator it;
    for (it = m_TextureList.begin(); it != m_TextureList.end(); it++) {
        if (it->Register()) cnt++;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return cnt;
}

// まとめてテクスチャ登録抹消
void CTextureManager::UnRegisterTextures()
{
    std::list<CTexture>::iterator it;
    for (it = m_TextureList.begin(); it != m_TextureList.end(); it++) {
        it->UnRegister();
    }
}

} // GL

