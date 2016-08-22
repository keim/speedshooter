//--------------------------------------------------------------------------------
//  RGBA/BGRA Format Sub Routines
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------

#ifndef _CORE_RGBA_INCLUDED
#define _CORE_RGBA_INCLUDED

#ifndef RGBtoGRAY
#define RGBtoGRAY(r, g, b)    ((unsigned char)(((r)*77 + (g)*150 + (b)*29)>>8))
#endif

#pragma message ("<< CORE_MESSAGE >>  include RGBA.h")


//--------------------------------------------------------------------------------
// �\���̒�` 
//--------------------------------------------------------------------------------

// RGBA �f�[�^�\����
//--------------------------------------------------
typedef struct {
  char achIdentifier[4];        // "rgba"/"bgra" �r�b�g�z��̒ʂ�D�ǂ�����
  unsigned short wWidth;        // WORD �^�ȗ��R�́C�w�b�_�� 8byte �̕���
  unsigned short wHeight;       // ������Ƃ����L�����ǂ����ȂƎv��������
  union {
    unsigned char abyData[1];   // pixel data �{��; BYTE  �A�N�Z�X
    unsigned long adwData[1];   //                  DWORD �A�N�Z�X
  };
} RGBA_DATA;



// �ׂ��摜�t�H�[�}�b�g ARGB
// "�Ƃ��Ă����͂�"�ihttp://www.emit.jp/�j����q�؁D
//--------------------------------------------------
typedef struct {
  union {
    unsigned char abyIdentifier[4]; // "BGRA"
    unsigned long dwIdentifier;     // 0x41524742(little endian)
  };
  unsigned long dwPixelFormat;      // 0x08080808(32bitColor) or 0x10101010(64bitColor)
  unsigned long dwWidth;
  unsigned long dwHeight;
} ArgbHeader;  



// Bitmap Info Header 
//--------------------------------------------------
typedef struct {
  unsigned long   biSize; 
  long            biWidth; 
  long            biHeight; 
  unsigned short  biPlanes; 
  unsigned short  biBitCount; 
  unsigned long   biCompression; 
  unsigned long   biSizeImage; 
  long            biXPelsPerMeter; 
  long            biYPelsPerMeter; 
  unsigned long   biClrUsed; 
  unsigned long   biClrImportant; 
} BIH; 

// �Œ����Bitmap ���̂ݎ擾����DFile Pointer �́C�f�[�^���擪�܂Ői�ށD�i���K���j
bool ReadBitmapHeader(FILE* pFile, int* pWidth, int* pHeight, int* pBits, unsigned char* abyColorTable);





//--------------------------------------------------------------------------------
// �֐� 
//--------------------------------------------------------------------------------

// �������Ǘ� 
//--------------------------------------------------

// �������̈�m��
RGBA_DATA* CreateRGBAData(const int width, const int height, bool bRGBA);

// �������̈���/NULL���
void DeleteRGBAData(RGBA_DATA* &pRGBA);



// �Q��
//--------------------------------------------------

// �T�C�Y�v�Z
int GetRGBADataSize(const RGBA_DATA* pRGBA);

// �`�����f (���K��)
bool IsRGBA(const RGBA_DATA* pRGBA);



// ����
//--------------------------------------------------

// �ϊ��D"RGBA"<->"BGRA"
void ConvertRGBABits(RGBA_DATA* pRGBA, bool bRGBA);

// �Ǎ� (�t�@�C���̃t�H�[�}�b�g�Ɋ֌W����bRGBA�Ŏw�肵���`���œǂ݂���)
RGBA_DATA* LoadRGBAData(const char* strFileName, bool bRGBA);

// �ۑ�
bool SaveRGBAData(const RGBA_DATA* pRGBA, const char* strFileName);



// ����
//--------------------------------------------------

// Bitmap Data �ǂݍ��݁i�F�}�b�v�ǂݍ��݁C�A���t�@��255�j 
RGBA_DATA* CompRGBAData(const char* strFileName, bool bRGBA);

// Bitmap Data �ǂݍ��݁i�A���t�@�}�b�v�ǂݍ��݁C�F�͎w��j
RGBA_DATA* CompRGBAData(unsigned char *abyColor, const char* strAlphaFileName, bool bRGBA);

// Bitmap Data �ǂݍ��݁i���ߐF�w��j 
RGBA_DATA* CompRGBAData(const char* strFileName, unsigned char *abyColor, bool bRGBA);

// Bitmap Data �ǂݍ��݁i�F/�A���t�@�}�b�v�̓ǂݍ��݁j
RGBA_DATA* CompRGBAData(const char* strFileName, const char* strAlphaFileName, bool bRGBA);

#endif //_CORE_RGBA_INCLUDED
