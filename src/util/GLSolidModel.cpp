//--------------------------------------------------------------------------------
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
#include "std_include.h"
#include "include_opengl.h"

namespace GL {
//--------------------------------------------------
// �X�^�e�B�b�N�����o
//--------------------------------------------------
// ���L�e�N�X�`���Ǘ��C���X�^���X
CTextureManager  CSolidModel::s_DefaultTextureManager;

// �`��T�u���[�`�� Functor 
void  (*(CSolidModel::s_funcDrawSub[CSolidModel::DRAWSUB_FUNC_MAX]))(CSolidModel::OBJECT_DATA*, int&) = {
    CSolidModel::DrawSub,
    CSolidModel::DrawSubS,
    CSolidModel::DrawSubT,
    CSolidModel::DrawSubST
};


//--------------------------------------------------
// �R���X�g���N�^
//--------------------------------------------------
CSolidModel::CSolidModel()
{
    m_uiID = 0;
    m_iDisplayListCount = 0;
	// �f�t�H���g�e�N�X�Ǘ��C���X�^���X
	m_pTextureManager = &s_DefaultTextureManager;
}

CSolidModel::~CSolidModel()
{
    Free();
}

//--------------------------------------------------
// �e�N�X�`���Ǘ��C���X�^���X
//--------------------------------------------------
// �O���̃e�N�X�`���Ǘ��C���X�^���X���g�p����
void CSolidModel::SetTextureManager(CTextureManager* ptm)
{
    m_pTextureManager = ptm;
}

// �f�t�H���g�e�N�X�`���Ǘ��C���X�^���X���擾����
CTextureManager* CSolidModel::GetDefaultTextureManager()
{
    return &s_DefaultTextureManager;
}

//--------------------------------------------------
// �t�@�C������
//--------------------------------------------------
// �Ǎ�
bool CSolidModel::Load(char* strFileName)
{
    int     iTokenCount, isize;
    int     iObjectIndex;
    const char* str;

    Util::CSimpleScript script(strFileName);
    script.SetWhiteChar(" (),;\t\r");
    script.SetTokenChar("{}");

    // �w�b�_�`�F�b�N
    if (!CheckHeader(&script)) return false;
    
    // �ǂ݂���
    iObjectIndex = 0;
    while ((iTokenCount = TextReadLine(&script)) != -1) {
        // �ŏ��̃g�[�N��
        str = script.GetHeadToken();
        
        // �}�e���A��
        if (stricmp(str, "Material") == 0) {                    
            // �f�[�^���̎擾 
            str = script.GetNextInt(&isize);
            if (isize == 0)                      return false;
            // �������̈�̊m��
            m_aMaterial.resize(isize);
            // �f�[�^�̓ǂ݂���
            if (!ReadTextMaterialChunk(&script)) return false;
        } else 

        // �I�u�W�F�N�g
        if (stricmp(str, "Object") == 0) {
            // �������̈�̊m��
            m_aObject.resize(iObjectIndex + 1);
            // �f�[�^�̓ǂ݂���
            if (!ReadTextObjectData(&m_aObject[iObjectIndex], &script)) return false;
            // �C���f�b�N�X�̃C���N�������g 
            iObjectIndex++;
        } else 

        // �I���
        if (stricmp(str, "Eof") == 0) {
            break;
        } else 

        // ���Ή��`�����N�̃X�L�b�v 
        {
            if (script.GetToken(iTokenCount - 1)[0] != '{') return false;
            if (!TextSkipChunk(&script)) return false;
        }
    }

    // �s���S�ł��ǂݍ��� 
    CalcFaceNormal();
    return true;
}

// ��s�Ǎ�
int CSolidModel::TextReadLine(Util::CSimpleScript* pscript)
{
    int iTokenCount;
    const char* str;
    
    // ���Ӗ��ȍs�͔�΂� 
    while ((iTokenCount = pscript->ReadLine()) != -1) {
        // �󔒍s
        if (iTokenCount == 0)          continue;
        str = pscript->GetHeadToken();
        // �R�����g�A�E�g
        if (memcmp(str, "//", 2) == 0) continue;
        if (*str == '#')               continue;
        break;
    }
    
    // pscript->GetNextToken() �ōŏ��̃g�[�N����ǂݍ��߂�悤�Ƀ��Z�b�g 
    pscript->ResetSequence();
    return iTokenCount;
}

// �`�����N�X�L�b�v 
bool CSolidModel::TextSkipChunk(Util::CSimpleScript* pscript)
{
    int cast_count = 1;
    int iTokenCount;

    while ((iTokenCount = TextReadLine(pscript)) != -1) {
        if (pscript->GetHeadToken()[0] == '}') {
            cast_count--;
            if (cast_count == 0) break;
        }
        if (pscript->GetToken(iTokenCount -1)[0] == '{') cast_count++;
    }
    
    if (cast_count == 0) return true;
    return false;
}

// �w�b�_�`�F�b�N
bool CSolidModel::CheckHeader(Util::CSimpleScript* pscript)
{
    // �ŏ��͕K���񕶎�
    if (TextReadLine(pscript) != 2) return false;
    const char* str = pscript->GetHeadToken();

    // "GLSolidModel Ver1" (�g��)
    if (stricmp(str, "GLSolidModel") == 0) {
        if (pscript->GetNextComp("Ver1") == NULL) return false;
    } else 

    // "Metasequoia Document"
    if (strcmp(str, "Metasequoia") == 0) {
        if (pscript->GetNextComp("Document") == NULL) return false;
        // "Format Text Ver 1.0"
        if (TextReadLine(pscript) != 4) return false;
        if (pscript->GetNextComp("Format")   == NULL) return false;
        if (pscript->GetNextComp("Text")     == NULL) return false;
        if (pscript->GetNextComp("Ver")      == NULL) return false;
        if (pscript->GetNextComp("1.0")      == NULL) return false;
    } else 
    
    // Header ���� 
    {
        return false;
    }

    return true;
}

// �}�e���A���`�����N�̓Ǎ�
bool CSolidModel::ReadTextMaterialChunk(Util::CSimpleScript* pscript)
{
    const char* str;
    MATERIAL_DATA* pMat;
    int idx;
        
    // �}�e���A���`�����N�̓ǂݍ��� 
    for (idx = 0; TextReadLine(pscript) != -1; idx++) {
        
        // �ŏ��̃g�[�N��
        str = pscript->GetHeadToken();

        // �ŏ��̃g�[�N���� "}" �Ȃ�I�� 
        if (*str == '}') return true;
        
        // �����ݒ�
        if (idx >= m_aMaterial.size()) return false;
        m_aMaterial[idx].material.m_vColor.Set(0.5f, 0.5f, 0.5f, 1.0f);
                
        // �}�e���A���f�[�^�̓ǂݍ��� 
        if (!ReadTextMaterialData(&(m_aMaterial[idx]), pscript)) return false;
    }
    
    // ���[�v��E�o�����ꍇ�C�r���œǂݍ��ݏI���D�G���[ 
    return false;
}

// �e�L�X�g�}�e���A���f�[�^�̓Ǎ�
bool CSolidModel::ReadTextMaterialData(CSolidModel::MATERIAL_DATA* pMat, Util::CSimpleScript* pscript)
{
    const char* str;
    DWORD dwColor = 0;
    int   i;
    CTexture* ptex;
    float a = 0.3f;
    float d = 1.0f;
    float s = 0.0f;
    float p = 0.5f;
    float e = 0.0f;
    char  strFileTex[256];
    char  strFileAlp[256];
    strFileTex[0] = '\0';
    strFileAlp[0] = '\0';
    
    // ���ʖ��̎擾
    str = pscript->GetHeadToken();
    if (str[0] != '"') return false;
    pMat->strName.assign(&(str[1]), strlen(str)-2);

    // �}�e���A�����̎擾 
    for (str = pscript->GetNextToken(); str != NULL; str = pscript->GetNextToken()) {
        // �F
        if (stricmp(str, "col") == 0) {
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.r)) == NULL) break;
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.g)) == NULL) break;
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.b)) == NULL) break;
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.a)) == NULL) break;
        } else 
        
        // ���_�F�̗L�� 
        if (stricmp(str, "vcol") == 0) {
            if (pscript->GetNextInt(&i) == NULL) break;
            pMat->bVertexColor = (i!=0);
        } else 
        
        // ���� 
        if (stricmp(str, "amb") == 0) {                                 // ����
            if (pscript->GetNextFloat(&a) == NULL) break;
        } else 
        if (stricmp(str, "dif") == 0) {                                 // �g�U��
            if (pscript->GetNextFloat(&d) == NULL) break;
        } else 
        if (stricmp(str, "spec") == 0 || stricmp(str, "spc") == 0) {    // ���ʔ��ˌ��i"spc"�͊g���j
            if (pscript->GetNextFloat(&s) == NULL) break;
        } else 
        if (stricmp(str, "power") == 0 || stricmp(str, "phong") == 0) { // ���ʔ��˂̋����i"phong"�͊g���j
            if (pscript->GetNextFloat(&p) == NULL) break;
        } else 
        if (stricmp(str, "emi") == 0) {                                 // ���Ȕ���
            if (pscript->GetNextFloat(&e) == NULL) break;
        } else 

        // �e�N�X�`��
        if (stricmp(str, "tex") == 0) {                                 // Texture Bmp �̃p�X�擾
            if ((str = pscript->GetNextToken()) == NULL) break;
            strcpy(strFileTex, pscript->GetFileDir());
            strncat(strFileTex, &(str[1]), strlen(str)-2);
        } else 
        if (stricmp(str, "aplane") == 0) {                              // Alpha Bmp �̃p�X�擾
            if ((str = pscript->GetNextToken()) == NULL) break;
            strcpy(strFileAlp, pscript->GetFileDir());
            strncat(strFileAlp, &(str[1]), strlen(str)-2);
        } else 
        
        // ���Ή�
        if (stricmp(str, "bump")      == 0 ||
            stricmp(str, "shader")    == 0 ||
            stricmp(str, "proj_type") == 0) {
            if (pscript->GetNextToken() == NULL) break;
        } else 
        if (stricmp(str, "proj_pos")   == 0 ||
            stricmp(str, "proj_scale") == 0 ||
            stricmp(str, "proj_angle") == 0) {
            if (pscript->GetNextToken() == NULL) break;
            if (pscript->GetNextToken() == NULL) break;
            if (pscript->GetNextToken() == NULL) break;
        }
    }

    // �ǂݍ��񂾃f�[�^���� CMaterial �ݒ�
    // ����ݒ�
    pMat->material.CalcParam(a, d, s, p * 0.01f, e);
    // �e�N�X�`���̓ǂݍ���
    if (strFileTex[0] != '\0' && m_pTextureManager != NULL) {
        if (strFileAlp[0] != '\0') ptex = m_pTextureManager->Load(strFileTex, strFileAlp);
        else                       ptex = m_pTextureManager->Load(strFileTex, 3);
        pMat->material.m_pTexture = ptex;
    }
    
    return true;
}

// �I�u�W�F�N�g�`�����N�̓Ǎ� 
bool CSolidModel::ReadTextObjectData(CSolidModel::OBJECT_DATA* pObj, Util::CSimpleScript* pscript)
{
    const char* str;
    float   f, r, g, b, a;

    // ���ʖ��̎擾
    if ((str = pscript->GetNextToken()) == NULL) return false;
    pObj->strName.assign(&(str[1]), strlen(str)-2);

    // ���Ǎ�
    while (TextReadLine(pscript) != -1) {
        
        // �擪�g�[�N��
        str = pscript->GetHeadToken();
        
        // color(alpha�͊g��) 
        if (stricmp(str, "color") == 0) {
            if (pscript->GetNextFloat(&r) == NULL) return false;
            if (pscript->GetNextFloat(&g) == NULL) return false;
            if (pscript->GetNextFloat(&b) == NULL) return false;
            if (pscript->GetNextFloat(&a) == NULL) a = 1;
            pObj->dwColor = GLRGBAf(r, g, b, a);
        } else
        
        // facet
        if (stricmp(str, "facet") == 0) {
            if (pscript->GetNextFloat(&f) == NULL) return false;
            pObj->fFacet = DEGREEtoRADIAN(f);
        } else
        
        // vertex
        if (stricmp(str, "vertex") == 0) {
            if (!ReadTextVertexData(pObj, pscript)) return false;
        } else
         
        // face
        if (stricmp(str, "face") == 0) {
            if (!ReadTextFaceData(pObj, pscript)) return false;
        } else
        
        // end
        if (str[0] == '}') {
            return true;
        } 
        
        // ���Ή��^�O�͑S����΂� 
    }
    
    return false;
}

// ���_�f�[�^�̓Ǎ�
bool CSolidModel::ReadTextVertexData(CSolidModel::OBJECT_DATA* pObj, Util::CSimpleScript* pscript)
{    
    float f;
    int idx, iSize;
    const char* str = pscript->GetNextInt(&iSize);
    if (iSize == 0) return false;

    // �������̈�m��
    pObj->aVertex.resize(iSize);
    
    idx = 0;
    while (TextReadLine(pscript) != -1) {
        str = pscript->GetNextFloat(&f);
        if (*str == '}') return true;
        if (idx == iSize) return false;
        pObj->aVertex[idx].vc.w = 1;
        pObj->aVertex[idx].vc.x = f;
        if (pscript->GetNextFloat(&(pObj->aVertex[idx].vc.y)) != NULL) {
        if (pscript->GetNextFloat(&(pObj->aVertex[idx].vc.z)) != NULL) {
            // ���_�F�i�g���j
            pscript->GetNextDWORD(&(pObj->aVertex[idx].col));
        }}
        idx++;
    }
    return false;
}

// �ʃf�[�^�̓Ǎ�
bool CSolidModel::ReadTextFaceData(CSolidModel::OBJECT_DATA* pObj, Util::CSimpleScript* pscript)
{
    int iIndexMax, iSize, i;
    
    // ���ʐ��̎擾 
    pscript->GetNextInt(&iSize);
    if (iSize == 0) return false;
    
    // ���u����m�� (std::vector ���Ɖ��̂� Segment Fault)
    WORD*  aInfo  = memCreate(WORD,  iSize);
    WORD*  aIndex = memCreate(WORD,  iSize*4);
    float* aUV    = memCreate(float, iSize*8);
    if (aInfo == NULL || aIndex == NULL || aUV == NULL) return false;

    // �e�L�X�g����ʏ����擾 
    iIndexMax = ReadTextFaceLine(aInfo, aIndex, aUV, iSize, pscript);
    
    // �����Ȃ�FACE_DATA�̐���
    if (iIndexMax > 0) {
        
        // �ʐ��̃J�E���g/�̈�m��
        int iFaceCount = 0;
        for (i=0; i<iIndexMax; i++) iFaceCount += (aInfo[i] & 0xff) - 2;
        pObj->aFace.resize(iFaceCount);

        // FACE_DATA �\���̂ɃR�s�[
        WORD*       pIndex = aIndex;
        float*      pUV    = aUV;
        FACE_DATA*  pData  = &(pObj->aFace[0]);

        for (i=0; i<iIndexMax; i++) {
            pData->mat    = (aInfo[i] >> 8) - 1;
            pData->idx[0] = pIndex[0];
            pData->idx[1] = pIndex[1];
            pData->idx[2] = pIndex[2];
            pData->uv[0].Set(pUV[0], pUV[1]);
            pData->uv[1].Set(pUV[2], pUV[3]);
            pData->uv[2].Set(pUV[4], pUV[5]);
            pData = &(pData[1]);
            
            // �S���_�̏ꍇ�́C�Q�̕��ʂ𐶐�
            if ((aInfo[i] & 0xff) == 4) {
                pData->mat    = (aInfo[i] >> 8) - 1;
                pData->idx[0] = pIndex[0];
                pData->idx[1] = pIndex[2];
                pData->idx[2] = pIndex[3];
                pData->uv[0].Set(pUV[0], pUV[1]);
                pData->uv[1].Set(pUV[4], pUV[5]);
                pData->uv[2].Set(pUV[6], pUV[7]);
                pData = &(pData[1]);
            }
            
            pIndex = &(pIndex[4]);
            pUV    = &(pUV[8]);
        }
    }

   
    // ���u����j�� 
    memDelete(aInfo);
    memDelete(aIndex);
    memDelete(aUV);
   
    return (iIndexMax > 0);
}

// �e�L�X�g����ʏ����擾 
int CSolidModel::ReadTextFaceLine(WORD* aInfo, WORD* aIndex, float* aUV, int iSize, Util::CSimpleScript* pscript)
{
    const char* str;
    int iVertexCount, idx, base_idx, iv, i;

    
    for (idx = 0; true; idx++) {
        // �P�s�ǂݍ���
        if (TextReadLine(pscript) == -1) return 0;
        
        // �����ݒ�
        aInfo[idx] = 0;
        
        // ���_���̎擾 
        str = pscript->GetNextInt(&iVertexCount);
        
        // �����ʂŏI��
        if (*str == '}') break;
        
        // �I�[�o�[�t���[/���_���̃`�F�b�N 
        if ((idx >= iSize) || (iVertexCount != 3 && iVertexCount != 4)) return 0;

        // �g�[�N���̎擾
        while ((str = pscript->GetNextToken()) != NULL) {

            // V(i1, i2, i3 ...)
            if (stricmp(str, "V") == 0) {
                base_idx = idx * 4;
                for (i=0; i<iVertexCount; i++) {
                    if (pscript->GetNextInt(&iv) == NULL) return 0;
                    aIndex[base_idx+i] = iv;
                }
            } else 

            // M(m)
            if (stricmp(str, "M") == 0) {
                if (pscript->GetNextInt(&iv) == NULL) return 0;
                aInfo[idx] = ((iv+1) << 8);
            } else 

            // UV(fx1, fy1, fx2, fy2, fx3, fy3 ...)
            if (stricmp(str, "UV") == 0) {
                base_idx = idx * 8;
                for (i=0; i<iVertexCount; i++) {
                    if (pscript->GetNextFloat(&(aUV[base_idx+i*2]))   == NULL) return 0;
                    if (pscript->GetNextFloat(&(aUV[base_idx+i*2+1])) == NULL) return 0;
                    aUV[base_idx+i*2+1] = 1-aUV[base_idx+i*2+1];
                }
            }
            
        }
        
        aInfo[idx] |= iVertexCount;
    }

    return idx;
}



//--------------------------------------------------
// ����
//--------------------------------------------------
// �s��ϊ� 
void CSolidModel::MultMatrix(CMatrix &mat)
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) MultMatrix(i, mat);
}

// �s��ϊ� 
void CSolidModel::MultMatrix(int iModelIndex, CMatrix &mat)
{
    int i;
    OBJECT_DATA* pobj = &(m_aObject[iModelIndex]);
    int          imax = pobj->aVertex.size();
    for (i=0; i<imax; i++) mat.Transform(pobj->aVertex[i].vc, pobj->aVertex[i].vc);
    CalcFaceNormal(iModelIndex);
}

// ��]�N�H�[�^�j�I���ϊ� 
void CSolidModel::MultQuat(CVector &qt)
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) MultQuat(i, qt);
}

// ��]�N�H�[�^�j�I���ϊ� 
void CSolidModel::MultQuat(int iModelIndex, CVector &qt)
{
    int i;
    OBJECT_DATA* pobj = &(m_aObject[iModelIndex]);
    int          imax = pobj->aVertex.size();
    for (i=0; i<imax; i++) pobj->aVertex[i].vc.QRotate(qt);
    CalcFaceNormal(iModelIndex);
}

// �ʖ@���^���_�@���̌v�Z
void CSolidModel::CalcFaceNormal()
{
    int imax = m_aObject.size();
    for (int i=0; i<imax; i++) CalcFaceNormal(i);
}

// �ʖ@���^���_�@���̌v�Z
void CSolidModel::CalcFaceNormal(int iObjectIndex)
{
    float theata;
    WORD v1, v2;
    CVector vc1, vc2;
    int i, j, imax, idx;
    
    OBJECT_DATA* pobj = &(m_aObject[iObjectIndex]);
    imax = pobj->aFace.size();
    if (imax == 0) return;
    
    for (i=0; i<imax; i++) {
        // �ʖ@���̌v�Z
        vc1 = pobj->aVertex[pobj->aFace[i].idx[0]].vc - pobj->aVertex[pobj->aFace[i].idx[1]].vc;
        vc2 = pobj->aVertex[pobj->aFace[i].idx[2]].vc - pobj->aVertex[pobj->aFace[i].idx[0]].vc;
        pobj->aFace[i].vcFaceNormal.Cross(vc1, vc2);
        pobj->aFace[i].vcFaceNormal.Normalize();
        pobj->aFace[i].vcVertexNormal[0] = pobj->aFace[i].vcFaceNormal;
        pobj->aFace[i].vcVertexNormal[1] = pobj->aFace[i].vcFaceNormal;
        pobj->aFace[i].vcVertexNormal[2] = pobj->aFace[i].vcFaceNormal;
    }
}

// �͈͌v�Z
void CSolidModel::CalcVertexRange()
{
	if (IsEmpty()) return;
	CalcVertexRange(0);
	m_vcMinimum.Copy(m_aObject[0].vcMinimum);
    m_vcMaximum.Copy(m_aObject[0].vcMaximum);
    int imax = m_aObject.size();
    for (int i=1; i<imax; i++) CalcVertexRange(i);
}

// �͈͌v�Z
void CSolidModel::CalcVertexRange(int iObjectIndex)
{
    int i, imax;
    OBJECT_DATA* pobj = &(m_aObject[iObjectIndex]);
	pobj->vcMinimum.SetPosition(0, 0, 0);
	pobj->vcMaximum.SetPosition(0, 0, 0);
    imax = pobj->aVertex.size();
    if (imax == 0) return;
	pobj->vcMinimum = pobj->aVertex[0].vc;
	pobj->vcMaximum = pobj->aVertex[0].vc;
    for (i=1; i<imax; i++) {
    	pobj->vcMinimum.Min(pobj->aVertex[i].vc);
    	pobj->vcMaximum.Max(pobj->aVertex[i].vc);
    }
	m_vcMinimum.Min(pobj->vcMinimum);
	m_vcMaximum.Max(pobj->vcMaximum);
}


//--------------------------------------------------
// �`��
//--------------------------------------------------
// �f���ɕ`��
void CSolidModel::Draw(bool bSmooth/*=false*/)
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) {
        Draw(i, bSmooth);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

// �f���ɕ`��i�I�u�W�F�N�g�P�ʁj
void CSolidModel::Draw(int iObjectIndex, bool bSmooth/*=false*/)
{
    if (iObjectIndex >= m_aObject.size()) return;
    OBJECT_DATA* pObj = &m_aObject[iObjectIndex];

    int imax = pObj->aFace.size();
    if (imax == 0) return;

    int i;
    int enFunctor;
    
    // ���_�@���`��t���O
    DRAWSUB_FUNC enVertexNormal = (bSmooth) ? DRAWSUB_NOFLAG : DRAWSUB_VERTEX_NORMAL;
    
    // �`��
    for (i=0; i<imax;) {
        // �}�e���A���̍����ւ�
        enFunctor = DrawSubChangeMaterial(pObj->aFace[i].mat) | enVertexNormal;
        
        // �`��T�u���[�`���̌Ăяo��
        glBegin(GL_TRIANGLES);
        s_funcDrawSub[enFunctor](pObj, i);
        glEnd();
    }
}

// �}�e���A�������ւ��i�Ԓl;�e�N�X�`���Q�ƃt���O�j 
CSolidModel::DRAWSUB_FUNC CSolidModel::DrawSubChangeMaterial(int idx)
{
    if (idx != -1) m_aMaterial[idx].material.Draw();
    return (m_aMaterial[idx].material.m_pTexture == NULL) ? DRAWSUB_NOFLAG : DRAWSUB_TEXTURE_UV;
}

// �`��T�u���[�`���i�ʖ@���C  UV�}�b�v�����j 
void  CSolidModel::DrawSub(CSolidModel::OBJECT_DATA* pObj, int &i)
{
    VERTEX_DATA* aVertex = &(pObj->aVertex[0]);
    FACE_DATA*   aFace   = &(pObj->aFace[0]);
    int          imax    = pObj->aFace.size();

    int iMatIndex = aFace[i].mat;
    for (; i<imax; i++) {
        if (aFace[i].mat != iMatIndex) return; 
        glNormal3fv(aFace[i].vcFaceNormal);
        glVertex3fv(aVertex[aFace[i].idx[2]].vc);
        glVertex3fv(aVertex[aFace[i].idx[1]].vc);
        glVertex3fv(aVertex[aFace[i].idx[0]].vc);
    }
}

// �`��T�u���[�`���i���_�@���CUV�}�b�v�����j
void  CSolidModel::DrawSubS(CSolidModel::OBJECT_DATA* pObj, int &i)
{
    VERTEX_DATA* aVertex = &(pObj->aVertex[0]);
    FACE_DATA*   aFace   = &(pObj->aFace[0]);
    int          imax    = pObj->aFace.size();

    int iMatIndex = aFace[i].mat;
    for (; i<imax; i++) {
        if (aFace[i].mat != iMatIndex) return; 
        glNormal3fv(aFace[i].vcVertexNormal[2]);
        glVertex3fv(aVertex[aFace[i].idx[2]].vc);
        glNormal3fv(aFace[i].vcVertexNormal[1]);
        glVertex3fv(aVertex[aFace[i].idx[1]].vc);
        glNormal3fv(aFace[i].vcVertexNormal[0]);
        glVertex3fv(aVertex[aFace[i].idx[0]].vc);
    }
}

// �`��T�u���[�`���i�ʖ@���C  UV�}�b�v�L��j
void  CSolidModel::DrawSubT(CSolidModel::OBJECT_DATA* pObj, int &i)
{
    VERTEX_DATA* aVertex = &(pObj->aVertex[0]);
    FACE_DATA*   aFace   = &(pObj->aFace[0]);
    int          imax    = pObj->aFace.size();

    int iMatIndex = aFace[i].mat;
    for (; i<imax; i++) {
        if (aFace[i].mat != iMatIndex) return; 
        glNormal3fv(aFace[i].vcFaceNormal);
        glTexCoord2fv(aFace[i].uv[2]);
        glVertex3fv(aVertex[aFace[i].idx[2]].vc);
        glTexCoord2fv(aFace[i].uv[1]);
        glVertex3fv(aVertex[aFace[i].idx[1]].vc);
        glTexCoord2fv(aFace[i].uv[0]);
        glVertex3fv(aVertex[aFace[i].idx[0]].vc);
    }
}

// �`��T�u���[�`���i���_�@���CUV�}�b�v�L��j
void  CSolidModel::DrawSubST(CSolidModel::OBJECT_DATA* pObj, int &i)
{
    VERTEX_DATA* aVertex = &(pObj->aVertex[0]);
    FACE_DATA*   aFace   = &(pObj->aFace[0]);
    int          imax    = pObj->aFace.size();

    int iMatIndex = aFace[i].mat;
    for (; i<imax; i++) {
        if (aFace[i].mat != iMatIndex) return; 
        glNormal3fv(aFace[i].vcVertexNormal[2]);
        glTexCoord2fv(aFace[i].uv[2]);
        glVertex3fv(aVertex[aFace[i].idx[2]].vc);
        glNormal3fv(aFace[i].vcVertexNormal[1]);
        glTexCoord2fv(aFace[i].uv[1]);
        glVertex3fv(aVertex[aFace[i].idx[1]].vc);
        glNormal3fv(aFace[i].vcVertexNormal[0]);
        glTexCoord2fv(aFace[i].uv[0]);
        glVertex3fv(aVertex[aFace[i].idx[0]].vc);
    }
}

// ���f�������`��
void  CSolidModel::DrawSolid()
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) DrawSolid(i);
}

// ���f�������`��
void  CSolidModel::DrawSolid(int iObjectIndex)
{
    int i, imax;
    VERTEX_DATA* aVertex = &(m_aObject[iObjectIndex].aVertex[0]);
    FACE_DATA*   aFace   = &(m_aObject[iObjectIndex].aFace[0]);

    if (iObjectIndex >= m_aObject.size()) return;
    imax = m_aObject[iObjectIndex].aFace.size();
    if (imax == 0) return;

    glBegin(GL_TRIANGLES);
        for (i=0; i<imax; i++) {
            glNormal3fv(aFace[i].vcFaceNormal);
            glVertex3fv(aVertex[aFace[i].idx[2]].vc);
            glVertex3fv(aVertex[aFace[i].idx[1]].vc);
            glVertex3fv(aVertex[aFace[i].idx[0]].vc);
        }
    glEnd();
}

// OpenGL Display List �̍쐬�DbCompose==true �ŒP�� / false �ŃI�u�W�F�N�g�����쐬
GLuint CSolidModel::Compile(bool bCompose/*=false*/, bool bSmooth/*=false*/)
{
    int     i;
    RegisterTexture();

    if (bCompose) {
        m_uiID = glGenLists(1);
        if (m_uiID == 0) return 0;
        m_iDisplayListCount = 1;
        glNewList(m_uiID, GL_COMPILE);
            Draw(bSmooth);
        glEndList();
    } else {
        m_uiID = glGenLists(m_aObject.size());
        if (m_uiID == 0) return 0;
        m_iDisplayListCount = m_aObject.size();
        for (i=0; i<m_iDisplayListCount; i++) {
            glNewList(m_uiID + i, GL_COMPILE);
                Draw(i, bSmooth);
            glEndList();
        }
        
    }
    return m_uiID;
}

// DisplayList �̊J��
void CSolidModel::FreeList()
{
    if (m_iDisplayListCount <= 0) return;
    glDeleteLists(m_uiID, m_iDisplayListCount);
    m_iDisplayListCount = 0;
    m_uiID = 0;
}

// Texture��o�^����
void CSolidModel::RegisterTexture()
{
    int imax = m_aMaterial.size();
    for (int i=0; i<imax; i++) m_aMaterial[i].material.RegisterTexture();
}

// Texture��o�^��������
void CSolidModel::UnRegisterTexture()
{
    int imax = m_aMaterial.size();
    for (int i=0; i<imax; i++) m_aMaterial[i].material.UnRegisterTexture();
}

// GL::CArray ���쐬����
bool CSolidModel::MakeArray(int idx, CArray* pArray, bool bSmooth/*=false*/)
{
    int i, imax, add;
    VERTEX_DATA* aVertex = &(m_aObject[idx].aVertex[0]);
    FACE_DATA*   aFace   = &(m_aObject[idx].aFace[0]);

    imax = m_aObject[idx].aFace.size();
    if (!pArray->Alloc(imax * 3)) return false;

    for (i=0, add=0; i<imax; i++) {
        *(pArray->GetVertex(add++)) = aVertex[aFace[i].idx[0]].vc;
        *(pArray->GetVertex(add++)) = aVertex[aFace[i].idx[1]].vc;
        *(pArray->GetVertex(add++)) = aVertex[aFace[i].idx[2]].vc;
    }
    if (pArray->IsSupportNormal()) {
        float* nml;
        if (bSmooth) {
            for (i=0, add=0; i<imax; i++) {
                nml = pArray->GetNormal(add++);
                nml[0] = aFace[i].vcVertexNormal[0].x;
                nml[1] = aFace[i].vcVertexNormal[0].y;
                nml[2] = aFace[i].vcVertexNormal[0].z;
                nml = pArray->GetNormal(add++);
                nml[0] = aFace[i].vcVertexNormal[1].x;
                nml[1] = aFace[i].vcVertexNormal[1].y;
                nml[2] = aFace[i].vcVertexNormal[1].z;
                nml = pArray->GetNormal(add++);
                nml[0] = aFace[i].vcVertexNormal[2].x;
                nml[1] = aFace[i].vcVertexNormal[2].y;
                nml[2] = aFace[i].vcVertexNormal[2].z;
            }
        } else {
            for (i=0, add=0; i<imax; i++) {
                nml = pArray->GetNormal(add++);
                nml[0] = aFace[i].vcFaceNormal.x;
                nml[1] = aFace[i].vcFaceNormal.y;
                nml[2] = aFace[i].vcFaceNormal.z;
                nml = pArray->GetNormal(add++);
                nml[0] = aFace[i].vcFaceNormal.x;
                nml[1] = aFace[i].vcFaceNormal.y;
                nml[2] = aFace[i].vcFaceNormal.z;
                nml = pArray->GetNormal(add++);
                nml[0] = aFace[i].vcFaceNormal.x;
                nml[1] = aFace[i].vcFaceNormal.y;
                nml[2] = aFace[i].vcFaceNormal.z;
            }
        }
    }
    if (pArray->IsSupportColor()) {
        for (i=0, add=0; i<imax; i++) {
            *(pArray->GetColor(add++)) = aVertex[aFace[i].idx[0]].col;
            *(pArray->GetColor(add++)) = aVertex[aFace[i].idx[1]].col;
            *(pArray->GetColor(add++)) = aVertex[aFace[i].idx[2]].col;
        }
    }
    if (pArray->IsSupportTexCoord()) {
        for (i=0, add=0; i<imax; i++) {
            *(pArray->GetTexCoord(add++)) = aFace[i].uv[0];
            *(pArray->GetTexCoord(add++)) = aFace[i].uv[1];
            *(pArray->GetTexCoord(add++)) = aFace[i].uv[2];
        }
    }

    return true;
}

//--------------------------------------------------
// ����������
//--------------------------------------------------
// �������̈�̑S����iDisplay List ��������邩�͑I���j
void CSolidModel::Free(bool bFreeList/*=true*/)
{
    int i;
    
    // Display List �̉��
	if (bFreeList) FreeList();

    // Object �̉��
    m_aObject.clear();

    // Material �̉��
    m_aMaterial.clear();
}

} // GL
