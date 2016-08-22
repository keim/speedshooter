#ifdef _WIN32
#	include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include <vector>

#include "RGBA.h"

// �Ǎ�
static bool LoadRGBADataF(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile);
// Bitmap Data �ǂݍ��݁i�F�}�b�v�ǂݍ��݁C�A���t�@��255�j 
static bool CompRGBADataF(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile);
// Bitmap Data �ǂݍ��݁i�A���t�@�}�b�v�ǂݍ��݁C�F�͎w��j
static bool CompRGBADataAF(RGBA_DATA* &pRGBA, unsigned char *abyColor, bool bRGBA, FILE* pFile);
// Bitmap Data �ǂݍ��݁i���ߐF�w��j 
static bool CompRGBADataTF(RGBA_DATA* &pRGBA, unsigned char *abyColor, bool bRGBA, FILE* pFile);
// Bitmap Data �ǂݍ��݁i�F/�A���t�@�}�b�v�̓ǂݍ��݁j
static bool CompRGBAColorMap(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile);
static bool CompRGBAAlphaMap(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile);



// �Œ����Bitmap ���̂ݎ擾����DFile Pointer �́C�f�[�^���擪�܂Ői�ށD�i���K���j
bool ReadBitmapHeader(FILE* pFile, int* pWidth, int* pHeight, int* pBits, unsigned char* abyColorTable)
{
  unsigned char tmpTable[1024];
  char pFileHeader[14];
  BIH  InfoHeader;

  fseek(pFile, 0, SEEK_SET);

  // �w�b�_�ǂ݂���
  if (fread(pFileHeader, 14, 1, pFile) != 1) return false;
  if (fread(&InfoHeader, 40, 1, pFile) != 1) return false;
  if (memcmp(pFileHeader, "BM", 2) != 0)     return false;
  if (InfoHeader.biCompression != 0)         return false;

  // ���Q��
  *pWidth  = InfoHeader.biWidth;
  *pHeight = InfoHeader.biHeight;
  *pBits   = InfoHeader.biBitCount;
  
  // �J���[�e�[�u���Q�� 
  if (*pBits == 8) {
    if (abyColorTable == NULL) abyColorTable = tmpTable;
    unsigned int ccount = (InfoHeader.biClrUsed == 0) ? 256 : InfoHeader.biClrUsed;
    if (fread(abyColorTable, 4, ccount, pFile) != ccount) return false;
  }

  return true;
}


// �������̈�m��
RGBA_DATA* CreateRGBAData(const int width, const int height, bool bRGBA)
{
  // �������m�� 
  RGBA_DATA* pRet = (RGBA_DATA*)malloc(width * height * 4 + 8);
  if (pRet == NULL) return NULL;

  // �w�b�_�ݒ� 
  if (bRGBA) memcpy(pRet->achIdentifier, "rgba", 4);
  else       memcpy(pRet->achIdentifier, "bgra", 4);
  pRet->wWidth  = width;
  pRet->wHeight = height;

  return pRet;
}


// �������̈���/NULL����D
void DeleteRGBAData(RGBA_DATA* &pRGBA)
{
  if (pRGBA != NULL) free(pRGBA);
  pRGBA = NULL;
}


// �T�C�Y�v�Z
int GetRGBADataSize(const RGBA_DATA* pRGBA)
{
  return (pRGBA->wWidth * pRGBA->wHeight * 4);
}


// �`�����f (���K��)
bool IsRGBA(const RGBA_DATA* pRGBA)
{
  return (pRGBA->achIdentifier[0] == 'r');
}


// �ϊ��D"RGBA"<->"BGRA"
void ConvertRGBABits(RGBA_DATA* pRGBA, bool bRGBA)
{
  // �t�H�[�}�b�g��v�H 
  if (IsRGBA(pRGBA) == bRGBA) return;

  // �w�b�_�������� 
  if (bRGBA) memcpy(pRGBA->achIdentifier, "rgba", 4);
  else       memcpy(pRGBA->achIdentifier, "bgra", 4);

  // �ϊ� 
  int i, sw;
  int imax = GetRGBADataSize(pRGBA);
  for (i=0; i<imax; i+=4) {
    sw = pRGBA->abyData[i];
    pRGBA->abyData[i] = pRGBA->abyData[i+2];
    pRGBA->abyData[i+2] = sw;
  }
}


// �ǂݍ��� (�t�@�C���̃t�H�[�}�b�g�Ɋ֌W����bRGBA�Ŏw�肵���`���œǂ݂���)
RGBA_DATA* LoadRGBAData(const char* strFileName, bool bRGBA)
{
  FILE* pFile = fopen(strFileName, "rb");
  if (pFile == NULL) return NULL;
  
  RGBA_DATA *pRGBA = NULL;
  if (!LoadRGBADataF(pRGBA, bRGBA, pFile)) {
    if (!CompRGBADataF(pRGBA, bRGBA, pFile)) {
      DeleteRGBAData(pRGBA);
    }
  }

  fclose(pFile);
  return pRGBA;
}

bool LoadRGBADataF(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile)
{
  unsigned long adw[2];
  bool  bFileRGBA;
  
  fseek(pFile, 0, SEEK_SET);

  // �w�b�_�ǂݍ��� 
  RGBA_DATA header;
  if (fread(&header, 8, 1, pFile) != 1) return false;

  // File �� RGBA/BGRA/ARGB ����C�̈�m�� 
  if (memcmp(header.achIdentifier, "BGRA", 4) == 0) {

    // ARGB File(64bit ���Ή�)
    if (header.wWidth != 0x0808 || header.wHeight != 0x0808) return false;
    if (fread(adw, 4, 2, pFile) != 2) return false;
    pRGBA = CreateRGBAData(adw[0], adw[1], false);

  } else {

    // RGBA File
    if (memcmp(header.achIdentifier, "rgba", 4) == 0) bFileRGBA = true;  
    else  
    if (memcmp(header.achIdentifier, "bgra", 4) == 0) bFileRGBA = false;
    else 
    return false;
    pRGBA = CreateRGBAData(header.wWidth, header.wHeight, bFileRGBA);

  }

  if (pRGBA == NULL) return false;
  
  // �f�[�^�ǂݍ��� 
  if (fread(pRGBA->abyData, GetRGBADataSize(pRGBA), 1, pFile) != 1) return false;

  // �f�[�^�ϊ� 
  ConvertRGBABits(pRGBA, bRGBA);
  
  return true;
}



// �ۑ�
bool SaveRGBAData(const RGBA_DATA* pRGBA, const char* strFileName)
{
  FILE* pFile = fopen(strFileName, "wb");
  if (pFile == NULL) return false;
  bool return_value = (fwrite(pRGBA, GetRGBADataSize(pRGBA)+8, 1, pFile) != 1);
  fclose(pFile);
  return return_value;
}



// Bitmap Data �ǂݍ��݁i�F�}�b�v�ǂݍ��݁C�A���t�@��255�j 
RGBA_DATA* CompRGBAData(const char* strFileName, bool bRGBA)
{
  FILE* pFile = fopen(strFileName, "rb");
  if (pFile == NULL) return false;

  RGBA_DATA *pRGBA = NULL;
  if (!CompRGBADataF(pRGBA, bRGBA, pFile)) DeleteRGBAData(pRGBA);

  fclose(pFile);
  return pRGBA;
}

bool CompRGBADataF(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile)
{
  int w, h, b, i, imax, pix;
  unsigned char abyColorTable[1024], bytmp[4];

  if (!ReadBitmapHeader(pFile, &w, &h, &b, abyColorTable)) return false;
  if ((w & 3) != 0) return false;  // �ʓ|�Ȃ̂� 4 �̔{���ȊO�̓G���[
  if ((pRGBA = CreateRGBAData(w, h, bRGBA)) == NULL) return false;
  imax = GetRGBADataSize(pRGBA);

  switch (b) {
  case 8:
    if (bRGBA) {
      for (i=0; i<imax; i+=4) {
        if ((pix = fgetc(pFile)) == EOF) return false;
        pix = pix << 2;
        pRGBA->abyData[i] = abyColorTable[pix+2];
        pRGBA->abyData[i+1] = abyColorTable[pix+1];
        pRGBA->abyData[i+2] = abyColorTable[pix];
        pRGBA->abyData[i+3] = 255;
      }
    } else {
      for (i=0; i<imax; i+=4) {
        if ((pix = fgetc(pFile)) == EOF) return false;
        pix = pix << 2;
        pRGBA->abyData[i] = abyColorTable[pix];
        pRGBA->abyData[i+1] = abyColorTable[pix+1];
        pRGBA->abyData[i+2] = abyColorTable[pix+2];
        pRGBA->abyData[i+3] = 255;
      }
    }
    break;
  case 24:
    if (bRGBA) {
      for (i=0; i<imax; i+=4) {
        if (fread(bytmp, 3, 1, pFile) != 1) return false;
        pRGBA->abyData[i] = bytmp[2];
        pRGBA->abyData[i+1] = bytmp[1];
        pRGBA->abyData[i+2] = bytmp[0];
        pRGBA->abyData[i+3] = 255;
      }
    } else {
      for (i=0; i<imax; i+=4) {
        if (fread(bytmp, 3, 1, pFile) != 1) return false;
        pRGBA->abyData[i] = bytmp[0];
        pRGBA->abyData[i+1] = bytmp[1];
        pRGBA->abyData[i+2] = bytmp[2];
        pRGBA->abyData[i+3] = 255;
      }
    }
    break;
  default:
    return false;
  }
  
  return true;
}



// Bitmap Data �ǂݍ��݁i�A���t�@�}�b�v�ǂݍ��݁C�F�͎w��j
RGBA_DATA* CompRGBAData(unsigned char *abyColor, const char* strAlphaFileName, bool bRGBA)
{
  FILE* pFile = fopen(strAlphaFileName, "rb");
  if (pFile == NULL) return false;

  RGBA_DATA* pRGBA = NULL;
  if (!CompRGBADataAF(pRGBA, abyColor, bRGBA, pFile)) DeleteRGBAData(pRGBA);

  fclose(pFile);
  return pRGBA;
}

bool CompRGBADataAF(RGBA_DATA* &pRGBA, unsigned char *abyColor, bool bRGBA, FILE* pFile)
{
  int w, h, b, i, imax, pix;
  unsigned char abyColorTable[1024], bytmp[4], swb;

  if (!ReadBitmapHeader(pFile, &w, &h, &b, abyColorTable)) return  false;
  if ((w & 3) != 0) return  false;  // �ʓ|�Ȃ̂� 4 �̔{���ȊO�̓G���[
  if ((pRGBA = CreateRGBAData(w, h, bRGBA)) == NULL) return  false;
  imax = GetRGBADataSize(pRGBA);

  if (!bRGBA) {
    swb         = abyColor[2];
    abyColor[2] = abyColor[0];
    abyColor[0] = swb;
  }
  
  switch (b) {
  case 8:
    for (i=0; i<1024; i+=4) abyColorTable[i>>2] = RGBtoGRAY(abyColorTable[i+2], abyColorTable[i+1], abyColorTable[i]);
    for (i=0; i<imax; i+=4) {
      if ((pix = fgetc(pFile)) == EOF) return  false;
      pRGBA->abyData[i] = abyColor[0];
      pRGBA->abyData[i+1] = abyColor[1];
      pRGBA->abyData[i+2] = abyColor[2];
      pRGBA->abyData[i+3] = abyColorTable[pix];
    }
    break;
    
  case 24:
    for (i=0; i<imax; i+=4) {
      if (fread(bytmp, 3, 1, pFile) != 1) return  false;
      pRGBA->abyData[i] = abyColor[0];
      pRGBA->abyData[i+1] = abyColor[1];
      pRGBA->abyData[i+2] = abyColor[2];
      pRGBA->abyData[i+3] = RGBtoGRAY(bytmp[2], bytmp[1], bytmp[0]);
    }
    break;
    
  default:
    return  false;
  }
  
  return  true;
}



// Bitmap Data �ǂݍ��݁i���ߐF�w��j 
RGBA_DATA* CompRGBAData(const char* strFileName, unsigned char* abyColor, bool bRGBA)
{
  FILE* pFile = fopen(strFileName, "rb");
  if (pFile == NULL) return false;

  RGBA_DATA* pRGBA = NULL;
  if (!CompRGBADataTF(pRGBA, abyColor, bRGBA, pFile)) DeleteRGBAData(pRGBA);

  fclose(pFile);
  return pRGBA;
}

bool CompRGBADataTF(RGBA_DATA* &pRGBA, unsigned char *abyColor, bool bRGBA, FILE* pFile)
{
  // dwKeyColor = *(unsigned long*){B, G, R, A}
  unsigned char abyKeyColor[4] = {abyColor[2], abyColor[1], abyColor[0], 255};
  unsigned long dwKeyColor     = *(unsigned long*)abyKeyColor;

  int w, h, b, i, imax;
  unsigned long pix;
  unsigned char abyColorTable[1024], bytmp[4];

  if (!ReadBitmapHeader(pFile, &w, &h, &b, abyColorTable)) return false;
  if ((w & 3) != 0) return false;  // �ʓ|�Ȃ̂� 4 �̔{���ȊO�̓G���[
  if ((pRGBA = CreateRGBAData(w, h, bRGBA)) == NULL) return false;
  imax = GetRGBADataSize(pRGBA);

  switch (b) {
  case 8:
    for (i=0; i<1024; i+=4) {
      abyColorTable[i+3] = 255;
      if (dwKeyColor == (*(unsigned long*)(&abyColorTable[i]))) break;
    }
    dwKeyColor = i;
    if (bRGBA) {
      for (i=0; i<imax; i+=4) {
        if ((pix = fgetc(pFile)) == EOF) return false;
        pix = pix << 2;
        pRGBA->abyData[i]   = abyColorTable[pix+2];
        pRGBA->abyData[i+1] = abyColorTable[pix+1];
        pRGBA->abyData[i+2] = abyColorTable[pix];
        pRGBA->abyData[i+3] = (pix == dwKeyColor) ? 0 : 255;
      }
    } else {
      for (i=0; i<imax; i+=4) {
        if ((pix = fgetc(pFile)) == EOF) return false;
        pix = pix << 2;
        pRGBA->abyData[i]   = abyColorTable[pix];
        pRGBA->abyData[i+1] = abyColorTable[pix+1];
        pRGBA->abyData[i+2] = abyColorTable[pix+2];
        pRGBA->abyData[i+3] = (pix == dwKeyColor) ? 0 : 255;
      }
    }
    break;
    
  case 24:
    bytmp[3] = 255;
    if (bRGBA) {
      for (i=0; i<imax; i+=4) {
        if (fread(bytmp, 3, 1, pFile) != 1) return false;
        pRGBA->abyData[i] = bytmp[2];
        pRGBA->abyData[i+1] = bytmp[1];
        pRGBA->abyData[i+2] = bytmp[0];
        pRGBA->abyData[i+3] = ((*(unsigned long*)bytmp) == dwKeyColor) ? 0 : 255;
      }
    } else {
      for (i=0; i<imax; i+=4) {
        if (fread(bytmp, 3, 1, pFile) != 1) return false;
        pRGBA->abyData[i] = bytmp[0];
        pRGBA->abyData[i+1] = bytmp[1];
        pRGBA->abyData[i+2] = bytmp[2];
        pRGBA->abyData[i+3] = ((*(unsigned long*)bytmp) == dwKeyColor) ? 0 : 255;
      }
    }
    break;
    
  default:
    return false;
  }
  
  return true;
}



// Bitmap Data �ǂݍ��݁i�F/�A���t�@�}�b�v�̓ǂݍ��݁j
RGBA_DATA* CompRGBAData(const char* strFileName, const char* strAlphaFileName, bool bRGBA)
{
  FILE*      pFile = NULL;
  RGBA_DATA* pRGBA = NULL;
  bool       bSucceed;


  // Color Map
  pFile = fopen(strFileName, "rb");
  if (pFile == NULL) return false;

  bSucceed = CompRGBAColorMap(pRGBA, bRGBA, pFile);
  fclose(pFile);
  
  if (!bSucceed) {
    DeleteRGBAData(pRGBA);
    return NULL;
  }


  // Alpha Map
  pFile = fopen(strAlphaFileName, "rb");
  if (pFile == NULL) return false;

  bSucceed = CompRGBAAlphaMap(pRGBA, bRGBA, pFile);
  fclose(pFile);

  if (!bSucceed) DeleteRGBAData(pRGBA);
  return pRGBA;
}

bool CompRGBAColorMap(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile)
{
  int w, h, b, i, imax, pix;
  unsigned char abyColorTable[1024], bytmp[4];
  
  if (!ReadBitmapHeader(pFile, &w, &h, &b, abyColorTable)) return false;
  if ((w & 3) != 0) return false;  // �ʓ|�Ȃ̂� 4 �̔{���ȊO�̓G���[
  if ((pRGBA = CreateRGBAData(w, h, bRGBA)) == NULL) return false;
  imax = GetRGBADataSize(pRGBA);
  
  switch (b) {
  case 8:
    if (bRGBA) {
      for (i=0; i<imax; i+=4) {
        if ((pix = fgetc(pFile)) == EOF) return false;
        pix = pix << 2;
        pRGBA->abyData[i] = abyColorTable[pix+2];
        pRGBA->abyData[i+1] = abyColorTable[pix+1];
        pRGBA->abyData[i+2] = abyColorTable[pix];
      }
    } else {
      for (i=0; i<imax; i+=4) {
        if ((pix = fgetc(pFile)) == EOF) return false;
        pix = pix << 2;
        pRGBA->abyData[i] = abyColorTable[pix];
        pRGBA->abyData[i+1] = abyColorTable[pix+1];
        pRGBA->abyData[i+2] = abyColorTable[pix+2];
      }
    }
    break;
    
  case 24:
    if (bRGBA) {
      for (i=0; i<imax; i+=4) {
        if (fread(bytmp, 3, 1, pFile) != 1) return false;
        pRGBA->abyData[i] = bytmp[2];
        pRGBA->abyData[i+1] = bytmp[1];
        pRGBA->abyData[i+2] = bytmp[0];
      }
    } else {
      for (i=0; i<imax; i+=4) {
        if (fread(bytmp, 3, 1, pFile) != 1) return false;
        pRGBA->abyData[i] = bytmp[0];
        pRGBA->abyData[i+1] = bytmp[1];
        pRGBA->abyData[i+2] = bytmp[2];
      }
    }
    break;
    
  default:
    return false;
  }
  
  return true;
}

bool CompRGBAAlphaMap(RGBA_DATA* &pRGBA, bool bRGBA, FILE* pFile)
{
  int w, h, b, i, imax, pix;
  unsigned char abyColorTable[1024], bytmp[4];

  // �T�C�Y���Ⴄ�ƃG���[
  if (!ReadBitmapHeader(pFile, &w, &h, &b, abyColorTable)) return false;
  if (w != pRGBA->wWidth || h != pRGBA->wHeight) return false;

  switch (b) {
  case 8:
    for (i=0; i<1024; i+=4) abyColorTable[i>>2] = RGBtoGRAY(abyColorTable[i+2], abyColorTable[i+1], abyColorTable[i]);
    for (i=0; i<imax; i+=4) {
      if ((pix = fgetc(pFile)) == EOF) return false;
      pRGBA->abyData[i+3] = abyColorTable[pix];
    }
    break;

  case 24:
    for (i=0; i<imax; i+=4) {
      if (fread(bytmp, 3, 1, pFile) != 1) return false;
      pRGBA->abyData[i+3] = RGBtoGRAY(bytmp[2], bytmp[1], bytmp[0]);
    }
    break;

  default:
    return false;
  }

  return true;
}
