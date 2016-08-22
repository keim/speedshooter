//--------------------------------------------------------------------------------
//  Memory Sub Routines
//    Copyright  (C)  2005  kei mesuda
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

//================================================================================
// ※※※※ 著作権関連 ※※※※
//================================================================================
//  このファイルで使われるメモリ管理ルーチンの一部は，GifLib パッケージの一部として，
//  Gershon Elber と Eric S. Raymond により書かれました．
//================================================================================

#ifndef _CORE_MEMORY_INCLUDED
#define _CORE_MEMORY_INCLUDED

#pragma message ("<< CORE_MESSAGE >>  include Memory.h")

//--------------------------------------------------------------------------------
// File から各値を取得する．
//--------------------------------------------------------------------------------
#define fileRead(pfile, a)     ( fread(&(a), sizeof(a), 1, pfile) == 1 )

//--------------------------------------------------------------------------------
//  メモリ管理マクロ（calloc/free のラッパ）
//  memCreate(type, n)      type[n] の作成
//  memDelete(ptr)          ptr を削除 / ptr = NULL を代入
//  memLength(ptr)          ptr の配列要素数を返す
//  memRealloc(ptr, size)   ptr について，size 再確保
//  memExpand(ptr, size)    ptr について，size 領域拡張
//  memElement(ptr, idx)    ptr[idx] を安全に返す（範囲外は 0）
//  memAppend(ptr, elem)    配列 ptr に 要素 elem を追加する
//  memJoin(ptr1, ptr2)     配列 ptr1 の後ろに ptr2 を追加する
//  memLoad(path)           ファイルを読み込んで，メモリ上に保存する
//--------------------------------------------------------------------------------
#define memCreate(type,n)       ( (type*) Util::memalloc(n*sizeof(type)) )
#define memDelete(ptr)          ( Util::memfree((char*)(ptr)), (ptr)=0 )
#define memLength(ptr)          ( (int)(Util::memlength((char*)(ptr))/sizeof((ptr)[0])) )
#define memRealloc(ptr, size)   ( *(char**)(&ptr) = Util::memrealloc((char*)(ptr),sizeof((ptr)[0])*size) )
#define memExpand(ptr, size)    ( *(char**)(&ptr) = Util::memexpand((char*)(ptr),sizeof((ptr)[0])*size) )
#define memElement(ptr, idx)    ( (((idx)<memLength(ptr)) && ((idx)>=0)) ? (ptr)[idx] : 0 )
#define memAppend(ptr, elem)    ( *(char**)(&ptr) = Util::memexpand((char*)(ptr),sizeof((ptr)[0])), (ptr)[len(ptr)-1]=(elem) )
#define memJoin(ptr1, ptr2)     ( *(char**)(&ptr1) = Util::memjoin((char*)(ptr1),(char*)(ptr2)) )
#define memLoad(path)           ( Util::memload(path, 1024) )

namespace Util {
//--------------------------------------------------------------------------------
// 各種エンディアン変換ルーチン
// type = SwapEndian(type)
//--------------------------------------------------------------------------------
inline short SwapEndian(const short &s)
{
    char* c = (char*)(&s);
    char  b[2] = {c[1], c[0]};
    return *(short*)b;
}

inline unsigned short SwapEndian(const unsigned short &us)
{
    char* c = (char*)(&us);
    char  b[2] = {c[1], c[0]};
    return *(unsigned short*)b;
}

inline long SwapEndian(const long &l)
{
    char* c = (char*)(&l);
    char  b[4] = {c[3], c[2], c[1], c[0]};
    return *(long*)b;
}

inline unsigned long SwapEndian(const unsigned long &ul)
{
    char* c = (char*)(&ul);
    char  b[4] = {c[3], c[2], c[1], c[0]};
    return *(unsigned long*)b;
}

inline float SwapEndian(const float &f)
{
    char* c = (char*)(&f);
    char  b[8] = {c[3], c[2], c[1], c[0]};
    return *(float*)b;
}

inline double SwapEndian(const double &d)
{
    char* c = (char*)(&d);
    char  b[8] = {c[7], c[6], c[5], c[4], c[3], c[2], c[1], c[0]};
    return *(double*)b;
}

//--------------------------------------------------------------------------------
// メモリ操作
//--------------------------------------------------------------------------------
char*       memalloc(long size);
void        memfree(char *ptr);
char*       memexpand(char *a, long expand);
char*       memrealloc(char *a, long size);
char*       memjoin(char *a, char *b);
char*       memload(char *path, int buffer_size);

inline long memlength(char *ptr)
{
    return (ptr != NULL) ? ((long*)ptr)[-1] : 0;
}

//--------------------------------------------------------------------------------
// DWORD/WORD版 memset
//--------------------------------------------------------------------------------
inline void* memsetS(void* ptr, unsigned short us, int size)
{
    unsigned long* pul = (unsigned long*)ptr;
    unsigned long   ul = us | (us << 16);
    int i;
    int size_dw = size >> 2;
    for (i=0; i<size_dw; i++) pul[i] = ul;
    int size_spls = size - (size_dw << 2);
    char* pch = (char*)(&pul[size_dw]);
    for (i=0; i<size_spls; i++, ul>>=8) pch[i] = (char)ul;
    return ptr;
}

inline void* memsetL(void* ptr, unsigned long ul, int size)
{
    unsigned long* pul = (unsigned long*)ptr;
    int i;
    int size_dw = size >> 2;
    for (i=0; i<size_dw; i++) pul[i] = ul;
    int size_spls = size - (size_dw << 2);
    char* pch = (char*)(&pul[size_dw]);
    for (i=0; i<size_spls; i++, ul>>=8) pch[i] = (char)ul;
    return ptr;
}
	
//--------------------------------------------------------------------------------
// サイズ変更 memcpy
// src から src_unit[Byte] を， dst に dst_step[Byte] おきに， src_size[Byte] コピーする．
// 例）src_unit=3, dst_step=4 の場合，↓のようにコピーされる．
// src; [A][B][C][D][E][F][G][H][I][J]...
// dst; [A][B][C][?][D][E][F][?][G][H][I][?]... 
// 例）src_unit=4, dst_step=3 の場合，↓のようにコピーされる．
// src; [A][B][C][D][E][F][G][H][I][J][K][L]...
// dst; [A][B][C][E][F][G][I][J][K]... 
//--------------------------------------------------------------------------------
inline void* step_memcpy(void* dst, const void* src, int src_size, int src_unit, int dst_step)
{
	char* pDst = (char*)dst;
	char* pSrc = (char*)src;
	int cpysize = __min(src_unit, dst_step);
	int imax = src_size / src_unit;
	for (int i=0; i<imax; i++, pDst+=dst_step, pSrc+=src_unit) memcpy(pDst, pSrc, cpysize);
	return pDst;
}
	
//--------------------------------------------------------------------------------
// Riff Data 関連 [ChunkID Size Data]...
//--------------------------------------------------------------------------------
// Riff Data Chunk の ID文字列 を読みこむ
bool ReadRiffChunkID(FILE* pFile, char* strID);

// ReadRiffChunkID() 後，Riff Data Chunk の データ本体 を読みこむ．
// NULL渡しでサイズのみ読みこむ．NULL渡しで無い場合，メモリ不足でも次のChunkまで読み飛ばす．
int  ReadRiffChunkData(FILE* pFile, void* pData=NULL, int iMemorySize=0);

// ReadRiffChunkID() 後，Riff Data を読み飛ばす
int  SkipRiffChunk(FILE* pFile);

// Riff Data Chunk を書きこむ
bool WriteRiffChunk(FILE* pFile, char* strID, int iSize, void* pData);

//--------------------------------------------------------------------------------
// File 関連
//--------------------------------------------------------------------------------
// ディレクトリパス以下削除（最後に"\"を含む） 
bool GetFileDir(char* strFileName);

// ディレクトリパス（最後に"\"を含まない）/ファイル名/拡張子に分離
bool GetFileName(char* strFilePath, char* &pDir, char* &pName, char* &pExt);

// ファイルの相対パスを取得
bool SetRelativePath(char* strFilePath, char* strHomeDir, char* strFileName);

//--------------------------------------------------------------------------------
// 文字列操作
//--------------------------------------------------------------------------------
// 指定された文字が文字列中に最後に現れた位置を求める
inline char* StrRChar(char* str, const char ch)
{
    int imax = strlen(str) - 1;
    for (int i=imax; i>=0; i--) {
        if (str[i] == ch) {
            return str + i;
        }
    }
    return NULL;
}

// 大文字/小文字を区別せずに文字を比較する
inline bool CompAlpha(const char a, const char b)
{
    if (a == b) return true;
    if (islower(a)) return (a == (b - 'A' + 'a'));
    if (isupper(a)) return (a == (b - 'a' + 'A'));
    return false;
}

// 指定された文字が文字列中に現れた回数を返す
inline int StrCharCount(char* str, const char ch)
{
    int cnt = 0;
    for (char* ptr = str; ptr != '\0'; ptr++) if (*ptr == ch) cnt++;
    return cnt;
}

// 文字列の頭から連続して一致する文字数を返す
inline int StrSearchComp(const char* stra, const char* strb)
{
    int i;
    int imax = __min(strlen(stra), strlen(strb));
    for (i=0; i<imax; i++) if (stra[i] != strb[i]) break;
    return i;
}

// 文字列の頭から連続して一致する文字数を返す(大/小文字の区別無し)
inline int StrSearchIComp(const char* stra, const char* strb)
{
    int i;
    int imax = __min(strlen(stra), strlen(strb));
    for (i=0; i<imax; i++) if (CompAlpha(stra[i], strb[i])) break;
    return i;
}

// 文字列リテラルの中身だけコピー（""内の取りだし）
inline char* StringLiteral(char* strDst, const char* strSrc)
{
    char* ptrStart = strchr(strSrc, '\"');
    if (ptrStart == NULL) return NULL;
    ptrStart++;
    char* ptrEnd = strchr(ptrStart, '\"');
    if (ptrEnd == NULL) return NULL;
    int len = ptrEnd - ptrStart;
    if (len == 0) strcpy(strDst, "");
    else          strncpy(strDst, ptrStart, len);
    return &(ptrEnd[1]);
}

// SJIS 文字の１バイト目か？
inline bool IsSJISTop(unsigned char ch)
{
    // Ascii文字の判定を一回にしたいので少し冗長
    return ((ch>=0x80) && (ch<=0x9f || (ch>=0xe0 && ch<=0xfc)));
}

// SJIS 文字の２バイト目か？
inline bool IsSJISBottom(unsigned char ch)
{
    return ((ch>=0x40 && ch<=0x7e) || (ch>=0x80 && ch<=0xfc));
}

} //Util

#endif //_CORE_MEMORY_INCLUDED

