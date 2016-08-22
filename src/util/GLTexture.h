//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CTexture, GL::CTextureManager)
//    Copyright  (C)  2006  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
// GL::CTexture;   OpenGL Texture ���b�v�N���X
// 1. Create() / Load() ��Pixel�f�[�^���V�X�e����������ɍ쐬�i���̒i�K�ł͓o�^�͂��Ȃ��j
// 2. SetParameters() �ŁCMipmap����/�g��/�k��/���b�s���O�̐ݒ�
// 3. Register() �Ńr�f�I��������Ƀ��[�h / UnRegister() �Ńr�f�I�������ォ��A�����[�h
// 4. Bind() �� glBindTexture() / UnBind() �� glBindTexture(0)
// 5. DrawPixels() �� glDrawPixel()
// 6. CopyTexture() ��glCopyTexImage2D()
// 7. Update() ��glTexSubImage*()
// 8. Delete() �œo�^������Pixel�f�[�^�j��
// 9. Is*()�CGet*() �ŏ��Q��
//--------------------------------------------------------------------------------
// GL::CTextureManager;   GL::CTexture �̊Ǘ��N���X
// �����t�@�C�����̃e�N�X�`�����񃁃�����W�J���Ȃ��悤�ɂ���D
// GL::CSolidModel ���Ŏg�p�D�ʏ�C�g���K�v�͖����D
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
//  �����o�ϐ�
//---------------------------------------------
private:
    GLubyte*    m_pData;                    // PixelData
    int         m_iWidth;                   // ��
    int         m_iHeight;                  // ����
    int         m_iBytes;                   // Data�̃o�C�g���iGL_ALPHA=1, GL_RGB=3, GL_RGBA=4)
    GLuint      m_uiID;                     // Texture Name (0 �Ȃ� UnRegisterd)
    std::string m_strFileName;              // FileName
    std::string m_strAlphaName;             // AlphaMap��FileName

    bool        m_bMipmap;                  // Mipmap �𐶐����邩
    int         m_iMinFilter;               // �e�N�X�`���k���̌v�Z���@
    int         m_iMagFilter;               // �e�N�X�`���g��̌v�Z���@
    int         m_iWrapS;                   // �e�N�X�`��x�����̃��b�s���O���@
    int         m_iWrapT;                   // �e�N�X�`��y�����̃��b�s���O���@

    int         m_iLog2Width;               // log2(Width)

    GLenum      m_enFormat;                 // Format(m_iBytes�ɂ��������ē����Őݒ�) (GL_ALPHA / GL_RGB / GL_RGBA)
    GLenum      m_enTarget;                 // GL_TEXTURE_1D / GL_TEXTURE_2D

public:
    CTexture();         // �R���X�g���N�^
    ~CTexture();        // �f�X�g���N�^�iDelete()���Ăяo���j

//---------------------------------------------
//  �����o�֐�
//---------------------------------------------
public:
    // ����/�j��
    bool Create(int width, int height, int bytes, const GLubyte* pixels=NULL);                          // �쐬
    bool Load(const char* strFileName, int bytes);                  // File ����ǂ݂���
    bool Load(const char* strFileName, DWORD dwTransColor);         // File ����ǂ݂��݁i�w��F���߁j
    bool Load(const char* strFileName, const char* strAlphaName);   // File ����Alpha�}�b�v�̓ǂ݂��݁i32bit�e�N�X�`���ɑ΂���8bit�r�b�g�}�b�v�̂݁j
    void Transparent(DWORD dwColor);                                // ���ߐF�w��
    void Delete(bool bUnregister=true);                             // �j���ibUnregister=true ��GRAM����o�^�����j

    bool BitBlt(int x, int y, const GLubyte* pixels, int pix_width, int pix_height, int pix_bytes);     // pixel data �̃R�s�[

    // �ݒ�/�o�^
    void SetParameters(bool bMipmap, bool bMinLinear, bool bMagLinear, bool bRepeatX, bool bRepeatY);   // �e�N�X�`���������@�̐ݒ�i�o�^�O�̂ݗL���j
    bool RegParameters(bool bMinLinear, bool bMagLinear, bool bRepeatX, bool bRepeatY);                 // �e�N�X�`���`����@�̐ݒ�i�o�^�����L���j
    bool Register();    // �o�^�i�r�f�I��������ɓW�J�j
    void UnRegister();  // �o�^�����i�r�f�I�������ォ�疕���j
	
    void CopyTexture(int x, int y); // Frame buffer ����e�N�X�`������

    // ����
    void Update();      // �o�^�� m_pData �̕ύX���e�N�X�`���ɔ��f
    
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

    // glRasterPos() -> glDrawPixels() (�����`����ŃC���f�b�N�X�w��j
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
    bool AllocPixelArea(int width, int height, int bytes);  // PixelData�̗̈�m��
    void FreePixelArea();                                   // PixelData�̗̈�j��

//--------------------------------------------------
// �Q��
//--------------------------------------------------
public:
    // ��H
    bool IsEmpty()
    {
        return (m_pData == NULL);
    }

    // �e�N�X�`���t�@�C���̃��[�h���H
    inline bool IsLoaded()
    {
        return (!m_strFileName.empty());
    }

    // �e�N�X�`�����o�^����Ă��邩
    inline bool IsRegistered()
    {
        return (m_uiID != 0);
    }

    // �t�@�C�����擾
    inline const char* GetFileName()
    {
        return m_strFileName.data();
    }

    // �s�N�Z���f�[�^�Q��
    inline GLubyte* GetPixelData()
    {
        return m_pData;
    }

    // �s�N�Z���f�[�^�Q��(���W�w��)
    inline GLubyte* GetPixelData(int x, int y)
    {
        int log2 = m_iLog2Width;
        int byte = m_iBytes;
        return &(m_pData[((y << log2) + x) * byte]);
    }

    // ���擾
    inline int GetWidth()
    {
        return m_iWidth;
    }

    // �����擾
    inline int GetHeight()
    {
        return m_iHeight;
    }

    // �F�[�x�擾
    inline int& GetBytes()
    {
        return m_iBytes;
    }

    // OpenGL��TextureID���擾
    inline GLuint& GetID()
    {
        return m_uiID;
    }
};




//--------------------------------------------------------------------------------
// GL::CTextureManager;   GL::CTexture �̊Ǘ��N���X
// Load()�Ńt�@�C���ǂݍ��݁DCTexture �̃|�C���^��Ԃ��D
// ���ɊJ���Ă���t�@�C�����̏ꍇ�́C�ȑO�J�������̂Ɠ����|�C���^��Ԃ��D
//--------------------------------------------------------------------------------
class CTextureManager
{
private:
    std::list<CTexture>    m_TextureList;

public:
    CTextureManager();
    ~CTextureManager();

    CTexture* Load(char* strFileName, int byte);                                    // File ����ǂ݂���
    CTexture* Load(char* strFileName, char* strAlphFileName);                       // File ����ǂ݂���(AlphMap�t��)
    CTexture* Create(int width, int height, int bytes, const GLubyte* pixels=NULL); // ��������
    CTexture* CheckFile(char* strFileName);                                         // ���łɊJ�����t�@�C���̌���
 
    int       RegisterTextures();        // �܂Ƃ߂ăe�N�X�`���o�^
    void      UnRegisterTextures();      // �܂Ƃ߂ăe�N�X�`���o�^����

private:
    CTexture* New();
};

} // GL

#endif // _CORE_GLTEXTURE_INCLUDED
