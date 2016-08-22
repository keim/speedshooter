//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CTexture, GL::CTextureManager)
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// GL::CTexture;   OpenGL Texture ラップクラス
// 1. Create() / Load() でPixelデータをシステムメモリ上に作成（この段階では登録はしない）
// 2. SetParameters() で，Mipmap生成/拡大/縮小/ラッピングの設定
// 3. Register() でビデオメモリ上にロード / UnRegister() でビデオメモリ上からアンロード
// 4. Bind() で glBindTexture() / UnBind() で glBindTexture(0)
// 5. DrawPixels() で glDrawPixel()
// 6. CopyTexture() でglCopyTexImage2D()
// 7. Update() でglTexSubImage*()
// 8. Delete() で登録抹消→Pixelデータ破棄
// 9. Is*()，Get*() で情報参照
//--------------------------------------------------------------------------------
// GL::CTextureManager;   GL::CTexture の管理クラス
// 同じファイル名のテクスチャを二回メモリ上展開しないようにする．
// GL::CSolidModel 内で使用．通常，使う必要は無い．
//--------------------------------------------------------------------------------
#ifndef _CORE_GLTEXTURE_INCLUDED
#define _CORE_GLTEXTURE_INCLUDED

#include <list>
#include <string>
#include "RGBA.h"
#pragma message ("<< CORE_MESSAGE >>  include GL::CTexture")

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif



namespace GL {
class CTexture
{
//---------------------------------------------
//  メンバ変数
//---------------------------------------------
private:
    GLubyte*    m_pData;                    // PixelData
    int         m_iWidth;                   // 幅
    int         m_iHeight;                  // 高さ
    int         m_iBytes;                   // Dataのバイト数（GL_ALPHA=1, GL_RGB=3, GL_RGBA=4)
    GLuint      m_uiID;                     // Texture Name (0 なら UnRegisterd)
    std::string m_strFileName;              // FileName
    std::string m_strAlphaName;             // AlphaMapのFileName

    bool        m_bMipmap;                  // Mipmap を生成するか
    int         m_iMinFilter;               // テクスチャ縮小の計算方法
    int         m_iMagFilter;               // テクスチャ拡大の計算方法
    int         m_iWrapS;                   // テクスチャx方向のラッピング方法
    int         m_iWrapT;                   // テクスチャy方向のラッピング方法

    int         m_iLog2Width;               // log2(Width)

    GLenum      m_enFormat;                 // Format(m_iBytesにしたがって内部で設定) (GL_ALPHA / GL_RGB / GL_RGBA)
    GLenum      m_enTarget;                 // GL_TEXTURE_1D / GL_TEXTURE_2D

public:
    CTexture();         // コンストラクタ
    ~CTexture();        // デストラクタ（Delete()を呼び出す）

//---------------------------------------------
//  メンバ関数
//---------------------------------------------
public:
    // 生成/破棄
    bool Create(int width, int height, int bytes, const GLubyte* pixels=NULL);                          // 作成
    bool Load(const char* strFileName, int bytes);                  // File から読みこみ
    bool Load(const char* strFileName, DWORD dwTransColor);         // File から読みこみ（指定色透過）
    bool Load(const char* strFileName, const char* strAlphaName);   // File からAlphaマップの読みこみ（32bitテクスチャに対して8bitビットマップのみ）
    void Transparent(DWORD dwColor);                                // 透過色指定
    void Delete(bool bUnregister=true);                             // 破棄（bUnregister=true でGRAMから登録抹消）

    bool BitBlt(int x, int y, const GLubyte* pixels, int pix_width, int pix_height, int pix_bytes);     // pixel data のコピー

    // 設定/登録
    void SetParameters(bool bMipmap, bool bMinLinear, bool bMagLinear, bool bRepeatX, bool bRepeatY);   // テクスチャ生成方法の設定（登録前のみ有効）
    bool RegParameters(bool bMinLinear, bool bMagLinear, bool bRepeatX, bool bRepeatY);                 // テクスチャ描画方法の設定（登録中も有効）
    bool Register();    // 登録（ビデオメモリ上に展開）
    void UnRegister();  // 登録抹消（ビデオメモリ上から抹消）
	
    void CopyTexture(int x, int y); // Frame buffer からテクスチャ生成

    // 操作
    void Update();      // 登録後 m_pData の変更をテクスチャに反映
    
    // glBindTexture()
    void Bind()
    {
        glBindTexture(m_enTarget, m_uiID);
    }
    
    // glBindTexture(0)
    void UnBind()
    {
        glBindTexture(m_enTarget, 0);
    }

    // glRasterPos() -> glDrawPixels()
    void DrawPixels(float x, float y)
    {
        glRasterPos2f(x, y);
        glDrawPixels(m_iWidth, m_iHeight, m_enFormat, GL_UNSIGNED_BYTE, m_pData);
    }

    // glRasterPos() -> glDrawPixels() (正方形限定でインデックス指定）
    void DrawPixels(float x, float y, int n)
    {
        glRasterPos2f(x, y);
        glDrawPixels(m_iWidth, m_iWidth, m_enFormat, GL_UNSIGNED_BYTE, &(m_pData[(m_iHeight-m_iWidth*(n+1))*m_iWidth*m_iBytes]));
    }

    // glReadPixels()
    void ReadPixels(int x, int y)
    {
        glPixelStorei(GL_PACK_ALIGNMENT ,4);
        glReadPixels(x, y, m_iWidth, m_iHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_pData);
    }

private:
    bool AllocPixelArea(int width, int height, int bytes);  // PixelDataの領域確保
    void FreePixelArea();                                   // PixelDataの領域破棄

//--------------------------------------------------
// 参照
//--------------------------------------------------
public:
    // 空？
    bool IsEmpty()
    {
        return (m_pData == NULL);
    }

    // テクスチャファイルのロードか？
    inline bool IsLoaded()
    {
        return (!m_strFileName.empty());
    }

    // テクスチャが登録されているか
    inline bool IsRegistered()
    {
        return (m_uiID != 0);
    }

    // ファイル名取得
    inline const char* GetFileName()
    {
        return m_strFileName.data();
    }

    // ピクセルデータ参照
    inline GLubyte* GetPixelData()
    {
        return m_pData;
    }

    // ピクセルデータ参照(座標指定)
    inline GLubyte* GetPixelData(int x, int y)
    {
        int log2 = m_iLog2Width;
        int byte = m_iBytes;
        return &(m_pData[((y << log2) + x) * byte]);
    }

    // 幅取得
    inline int GetWidth()
    {
        return m_iWidth;
    }

    // 高さ取得
    inline int GetHeight()
    {
        return m_iHeight;
    }

    // 色深度取得
    inline int& GetBytes()
    {
        return m_iBytes;
    }

    // OpenGLのTextureIDを取得
    inline GLuint& GetID()
    {
        return m_uiID;
    }
};




//--------------------------------------------------------------------------------
// GL::CTextureManager;   GL::CTexture の管理クラス
// Load()でファイル読み込み．CTexture のポインタを返す．
// 既に開いているファイル名の場合は，以前開いたものと同じポインタを返す．
//--------------------------------------------------------------------------------
class CTextureManager
{
private:
    std::list<CTexture>    m_TextureList;

public:
    CTextureManager();
    ~CTextureManager();

    CTexture* Load(char* strFileName, int byte);                                    // File から読みこみ
    CTexture* Load(char* strFileName, char* strAlphFileName);                       // File から読みこみ(AlphMap付き)
    CTexture* Create(int width, int height, int bytes, const GLubyte* pixels=NULL); // 生成する
    CTexture* CheckFile(char* strFileName);                                         // すでに開いたファイルの検索
 
    int       RegisterTextures();        // まとめてテクスチャ登録
    void      UnRegisterTextures();      // まとめてテクスチャ登録抹消

private:
    CTexture* New();
};

} // GL

#endif // _CORE_GLTEXTURE_INCLUDED
