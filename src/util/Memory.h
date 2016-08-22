//--------------------------------------------------------------------------------
//  Memory Sub Routines
//    Copyright  (C)  2005  kei mesuda
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

//================================================================================
// �������� ���쌠�֘A ��������
//================================================================================
//  ���̃t�@�C���Ŏg���郁�����Ǘ����[�`���̈ꕔ�́CGifLib �p�b�P�[�W�̈ꕔ�Ƃ��āC
//  Gershon Elber �� Eric S. Raymond �ɂ�菑����܂����D
//================================================================================

#ifndef _CORE_MEMORY_INCLUDED
#define _CORE_MEMORY_INCLUDED

#pragma message ("<< CORE_MESSAGE >>  include Memory.h")

//--------------------------------------------------------------------------------
// File ����e�l���擾����D
//--------------------------------------------------------------------------------
#define fileRead(pfile, a)     ( fread(&(a), sizeof(a), 1, pfile) == 1 )

//--------------------------------------------------------------------------------
//  �������Ǘ��}�N���icalloc/free �̃��b�p�j
//  memCreate(type, n)      type[n] �̍쐬
//  memDelete(ptr)          ptr ���폜 / ptr = NULL ����
//  memLength(ptr)          ptr �̔z��v�f����Ԃ�
//  memRealloc(ptr, size)   ptr �ɂ��āCsize �Ċm��
//  memExpand(ptr, size)    ptr �ɂ��āCsize �̈�g��
//  memElement(ptr, idx)    ptr[idx] �����S�ɕԂ��i�͈͊O�� 0�j
//  memAppend(ptr, elem)    �z�� ptr �� �v�f elem ��ǉ�����
//  memJoin(ptr1, ptr2)     �z�� ptr1 �̌��� ptr2 ��ǉ�����
//  memLoad(path)           �t�@�C����ǂݍ���ŁC��������ɕۑ�����
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
// �e��G���f�B�A���ϊ����[�`��
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
// ����������
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
// DWORD/WORD�� memset
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
// �T�C�Y�ύX memcpy
// src ���� src_unit[Byte] ���C dst �� dst_step[Byte] �����ɁC src_size[Byte] �R�s�[����D
// ��jsrc_unit=3, dst_step=4 �̏ꍇ�C���̂悤�ɃR�s�[�����D
// src; [A][B][C][D][E][F][G][H][I][J]...
// dst; [A][B][C][?][D][E][F][?][G][H][I][?]... 
// ��jsrc_unit=4, dst_step=3 �̏ꍇ�C���̂悤�ɃR�s�[�����D
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
// Riff Data �֘A [ChunkID Size Data]...
//--------------------------------------------------------------------------------
// Riff Data Chunk �� ID������ ��ǂ݂���
bool ReadRiffChunkID(FILE* pFile, char* strID);

// ReadRiffChunkID() ��CRiff Data Chunk �� �f�[�^�{�� ��ǂ݂��ށD
// NULL�n���ŃT�C�Y�̂ݓǂ݂��ށDNULL�n���Ŗ����ꍇ�C�������s���ł�����Chunk�܂œǂݔ�΂��D
int  ReadRiffChunkData(FILE* pFile, void* pData=NULL, int iMemorySize=0);

// ReadRiffChunkID() ��CRiff Data ��ǂݔ�΂�
int  SkipRiffChunk(FILE* pFile);

// Riff Data Chunk ����������
bool WriteRiffChunk(FILE* pFile, char* strID, int iSize, void* pData);

//--------------------------------------------------------------------------------
// File �֘A
//--------------------------------------------------------------------------------
// �f�B���N�g���p�X�ȉ��폜�i�Ō��"\"���܂ށj 
bool GetFileDir(char* strFileName);

// �f�B���N�g���p�X�i�Ō��"\"���܂܂Ȃ��j/�t�@�C����/�g���q�ɕ���
bool GetFileName(char* strFilePath, char* &pDir, char* &pName, char* &pExt);

// �t�@�C���̑��΃p�X���擾
bool SetRelativePath(char* strFilePath, char* strHomeDir, char* strFileName);

//--------------------------------------------------------------------------------
// �����񑀍�
//--------------------------------------------------------------------------------
// �w�肳�ꂽ�����������񒆂ɍŌ�Ɍ��ꂽ�ʒu�����߂�
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

// �啶��/����������ʂ����ɕ������r����
inline bool CompAlpha(const char a, const char b)
{
    if (a == b) return true;
    if (islower(a)) return (a == (b - 'A' + 'a'));
    if (isupper(a)) return (a == (b - 'a' + 'A'));
    return false;
}

// �w�肳�ꂽ�����������񒆂Ɍ��ꂽ�񐔂�Ԃ�
inline int StrCharCount(char* str, const char ch)
{
    int cnt = 0;
    for (char* ptr = str; ptr != '\0'; ptr++) if (*ptr == ch) cnt++;
    return cnt;
}

// ������̓�����A�����Ĉ�v���镶������Ԃ�
inline int StrSearchComp(const char* stra, const char* strb)
{
    int i;
    int imax = __min(strlen(stra), strlen(strb));
    for (i=0; i<imax; i++) if (stra[i] != strb[i]) break;
    return i;
}

// ������̓�����A�����Ĉ�v���镶������Ԃ�(��/�������̋�ʖ���)
inline int StrSearchIComp(const char* stra, const char* strb)
{
    int i;
    int imax = __min(strlen(stra), strlen(strb));
    for (i=0; i<imax; i++) if (CompAlpha(stra[i], strb[i])) break;
    return i;
}

// �����񃊃e�����̒��g�����R�s�[�i""���̎�肾���j
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

// SJIS �����̂P�o�C�g�ڂ��H
inline bool IsSJISTop(unsigned char ch)
{
    // Ascii�����̔�������ɂ������̂ŏ����璷
    return ((ch>=0x80) && (ch<=0x9f || (ch>=0xe0 && ch<=0xfc)));
}

// SJIS �����̂Q�o�C�g�ڂ��H
inline bool IsSJISBottom(unsigned char ch)
{
    return ((ch>=0x40 && ch<=0x7e) || (ch>=0x80 && ch<=0xfc));
}

} //Util

#endif //_CORE_MEMORY_INCLUDED

