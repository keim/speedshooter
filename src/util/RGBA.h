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
// 構造体定義 
//--------------------------------------------------------------------------------

// RGBA データ構造体
//--------------------------------------------------
typedef struct {
  char achIdentifier[4];        // "rgba"/"bgra" ビット配列の通り．どっちか
  unsigned short wWidth;        // WORD 型な理由は，ヘッダは 8byte の方が
  unsigned short wHeight;       // ちょっとだけキリが良いかなと思っただけ
  union {
    unsigned char abyData[1];   // pixel data 本体; BYTE  アクセス
    unsigned long adwData[1];   //                  DWORD アクセス
  };
} RGBA_DATA;



// べた画像フォーマット ARGB
// "とってもごはん"（http://www.emit.jp/）から拝借．
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

// 最低限のBitmap 情報のみ取得する．File Pointer は，データ部先頭まで進む．（※適当）
bool ReadBitmapHeader(FILE* pFile, int* pWidth, int* pHeight, int* pBits, unsigned char* abyColorTable);





//--------------------------------------------------------------------------------
// 関数 
//--------------------------------------------------------------------------------

// メモリ管理 
//--------------------------------------------------

// メモリ領域確保
RGBA_DATA* CreateRGBAData(const int width, const int height, bool bRGBA);

// メモリ領域解放/NULL代入
void DeleteRGBAData(RGBA_DATA* &pRGBA);



// 参照
//--------------------------------------------------

// サイズ計算
int GetRGBADataSize(const RGBA_DATA* pRGBA);

// 形式判断 (※適当)
bool IsRGBA(const RGBA_DATA* pRGBA);



// 操作
//--------------------------------------------------

// 変換．"RGBA"<->"BGRA"
void ConvertRGBABits(RGBA_DATA* pRGBA, bool bRGBA);

// 読込 (ファイルのフォーマットに関係無くbRGBAで指定した形式で読みこむ)
RGBA_DATA* LoadRGBAData(const char* strFileName, bool bRGBA);

// 保存
bool SaveRGBAData(const RGBA_DATA* pRGBA, const char* strFileName);



// 生成
//--------------------------------------------------

// Bitmap Data 読み込み（色マップ読み込み，アルファは255） 
RGBA_DATA* CompRGBAData(const char* strFileName, bool bRGBA);

// Bitmap Data 読み込み（アルファマップ読み込み，色は指定）
RGBA_DATA* CompRGBAData(unsigned char *abyColor, const char* strAlphaFileName, bool bRGBA);

// Bitmap Data 読み込み（透過色指定） 
RGBA_DATA* CompRGBAData(const char* strFileName, unsigned char *abyColor, bool bRGBA);

// Bitmap Data 読み込み（色/アルファマップの読み込み）
RGBA_DATA* CompRGBAData(const char* strFileName, const char* strAlphaFileName, bool bRGBA);

#endif //_CORE_RGBA_INCLUDED
