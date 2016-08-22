//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CSolidModel)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  GL::CSolidModel   
//  �Œ� Model �N���X�D��{�I�ɂ� Metasequoia �� DisplayList ���b�v�D
//  �T�|�[�g  �G�@���v�Z�C�ޗ��Ǘ��C���_�F�CUVMap�C�X���[�X�V�F�C�f�B���O(�\��)
//  ��T�|�[�g�G�_(�Ɨ����_), ����, 4���_���ʂ̕ۑ�(3���_�ɕ������ĕۑ�)
//  �\���G CSolidModel { MATERIAL_DATA, OBJECT_DATA { VERTEX_DATA, FACE_DATA }; };
//  1. Load() �� Metasequoia Object File �̓ǂ߂镔�������Ǎ�(4���_���ʂ�3���_�ɕ���)
//  2.1 Compile() -> CallList() �ŁCTexture�o�^ -> DisplayList�� �쐬 -> DisplayList �`��
//  2.2 RegisterTexture() -> Draw() �ŁCTexture�o�^ -> ���`��
//  2.3 DrawSolid() �ŁC�}�e���A���𖳎����Č`��̂ݕ`��
//  
//  1. SetTextureManager() �ŊO���̃e�N�X�`���}�l�[�W�����֘A�t����i�ʏ�f�t�H���g�j�D
//  2. AllocObject() �ŃI�u�W�F�N�g���̊m�ہiAlloc���Ȃ��ꍇLoad()�ŏ���Ɋm�ۂ����j
//  3. LoadBinary() �ŃI���W�i���o�C�i���f�[�^�Ǎ�
//  4. Save() �ŃI���W�i���o�C�i���f�[�^�ۑ�
//  5. MakeArray() �� InterleavedArrays(GL_TRIANGLES) �ɕϊ��D
//
//--------------------------------------------------------------------------------
// GL::CompileModel()
// Metasequoia Model Object File ���� OpenGL Display List �𐶐����� ID ��Ԃ��D
//--------------------------------------------------------------------------------

#ifndef _CORE_GLSOLIDMODEL_INCLUDED
#define _CORE_GLSOLIDMODEL_INCLUDED

#include "SimpleScript.h"
#include "GLMaterial.h"
#include "GLArray.h"
#include "GLElement.h"
#include "GLLight.h"
#pragma message ("<< CORE_MESSAGE >>  include GL::CSolidModel")

namespace GL {
class CSolidModel
{
//--------------------------------------------------
// �\����
//--------------------------------------------------
public:
    // Vertex �\����
    struct VERTEX_DATA {
        CVector vc;                             // ���_
        DWORD   col;                            // ���_�F
    };

    // Face �\����
    struct FACE_DATA {
        char        mat;                        // �}�e���A���ԍ�(-1 �ŕW���F)
        WORD        idx[3];                     // ���_�C���f�b�N�X
        VECTOR2D    uv[3];                      // �e�N�X�`�����W
        // �ȉ��L�^���Ȃ��D���f�������̓x�Ɍv�Z���� 
        CVector     vcFaceNormal;               // �ʖ@���f�[�^
        CVector     vcVertexNormal[3];          // ���_�@���f�[�^
    };

    // Object �\����
    class OBJECT_DATA {
    public:
        std::string              strName;       // �I�u�W�F�N�g��
        std::vector<VERTEX_DATA> aVertex;       // ���_�f�[�^
        std::vector<FACE_DATA>   aFace;         // �ʃf�[�^
        float                    fFacet;        // Smoothing Angle [rad]
        DWORD                    dwColor;       // �F
        // �ȉ��L�^���Ȃ��D���f�������̓x�Ɍv�Z����
        CVector                  vcMinimum;     // �I�u�W�F�N�g���\�����钸�_�� x,y,z �ŏ��l
        CVector                  vcMaximum;     // �I�u�W�F�N�g���\�����钸�_�� x,y,z �ő�l

        OBJECT_DATA()
        {
            fFacet = 0.0f;
            dwColor = 0xffffffff;
        }

        ~OBJECT_DATA()
        {
        }
    };

    // Material �\����
    class MATERIAL_DATA {
    public:
        std::string              strName;        // �}�e���A����
        CMaterial                material;       // �ޗ��p�����[�^ 
        bool                     bVertexColor;   // ���_�F���̗p����(0/1)
        
        MATERIAL_DATA()
        {
            bVertexColor = false;
        }
        
        ~MATERIAL_DATA()
        {
        }
    };

//--------------------------------------------------
// �����o�ϐ�
//--------------------------------------------------
private:
    std::vector<MATERIAL_DATA> m_aMaterial;          // �ޗ��f�[�^�{��
    std::vector<OBJECT_DATA>   m_aObject;            // �I�u�W�F�N�g�f�[�^�{��

    GLuint                     m_uiID;               // DisplayList ID
    int                        m_iDisplayListCount;  // DisplayList Count
	CVector                    m_vcMinimum;          // ���_�̍ŏ��l
	CVector                    m_vcMaximum;          // ���_�̍ő�l

    CTextureManager*           m_pTextureManager;       // �e�N�X�`���Ǘ��N���X�̃|�C���^
    static CTextureManager     s_DefaultTextureManager; // ���L�e�N�X�`���Ǘ��C���X�^���X

//--------------------------------------------------
// �����o�֐�
//--------------------------------------------------
public:
    // �R���X�g���N�^
    CSolidModel();
    ~CSolidModel();

    // �ݒ�
    void  SetTextureManager(CTextureManager* ptm);      // �O���e�N�X�`���Ǘ��C���X�^���X���g�p����
    static CTextureManager* GetDefaultTextureManager(); // �f�t�H���g�e�N�X�`���Ǘ��C���X�^���X�̎擾

    // �t�@�C������
    bool  Load(char* strFileName);              // �e�L�X�g�f�[�^�CMetasequoia File �̓ǂ݂���

    // ����
    void  Free(bool bFreeList = true);                  // �S�������̉���ibFreeList = false �� DisplayList�͔j�����Ȃ��j
    void  RegisterTexture();                            // �g�p����e�N�X�`����o�^����
    void  UnRegisterTexture();                          // �g�p����e�N�X�`���̓o�^�𖕏�����
    void  MultMatrix(CMatrix &mat);                     // ���W�ϊ�
    void  MultMatrix(int iObjectIndex, CMatrix &mat);   // ���W�ϊ��i�I�u�W�F�N�g�P�ʁj
    void  MultQuat(CVector &qt);                        // ��]�N�H�[�^�j�I���ϊ�
    void  MultQuat(int iObjectIndex, CVector &qt);      // ��]�N�H�[�^�j�I���ϊ��i�I�u�W�F�N�g�P�ʁj
    void  CalcFaceNormal();                             // �@�����v�Z����
    void  CalcFaceNormal(int iObjectIndex);             // �@�����v�Z����i�I�u�W�F�N�g�P�ʁj
	void  CalcVertexRange();                            // ���_�̑��݂���͈͂��v�Z
	void  CalcVertexRange(int iObjectIndex);            // ���_�̑��݂���͈͂��v�Z�i�I�u�W�F�N�g�P�ʁj

    // �`��
    void  Draw(bool bSmooth=false);                     // �f���ɕ`��
    void  Draw(int iObjectIndex, bool bSmooth=false);   // �f���ɕ`��i�I�u�W�F�N�g�P�ʁj
    void  DrawSolid();                                  // ���f�������`��
    void  DrawSolid(int iObjectIndex);                  // ���f�������`��i�I�u�W�F�N�g�P�ʁj

    // �f�B�X�v���C���X�g�֘A
    GLuint Compile(bool bCompose=false, bool bSmooth=false);    // Display List �̍쐬, bCompose=false �ŃI�u�W�F�N�g�P�ʂŕʂ�Display List�����
    void   FreeList();                                          // Display List �̔j��
//  void   CallList();                                          // Compile() �Ő������� Display List ���Ăяo�� 
//  void   CallList(int idx);                                   // Compile() �Ő������� Display List ���Ăяo���i�C���f�b�N�X�w��j 

    // �ϊ�
    bool   MakeArray(int idxObject, CArray* pArray, bool bSmooth=false);    // CArray�̍쐬�i�I�u�W�F�N�g�P�ʁj

// �����Ŏg�p
private:
    // �f�[�^�ǂݍ��� 
    int   TextReadLine(Util::CSimpleScript* pscript);   // �󔒍s�ƃR�����g�A�E�g���΂���ReadLine
    bool  TextSkipChunk(Util::CSimpleScript* pscript);  // ...} ���΂�

    bool  CheckHeader(Util::CSimpleScript* pscript);                                // �w�b�_�`�F�b�N
    bool  ReadTextMaterialChunk(Util::CSimpleScript* pscript);                      // Material �`�����N��ǂ� 
    bool  ReadTextMaterialData(MATERIAL_DATA* pMat, Util::CSimpleScript* pscript);  // Material �f�[�^��ǂ� 
    bool  ReadTextObjectData(OBJECT_DATA* pObj, Util::CSimpleScript* pscript);      // Object �f�[�^��ǂ�
    bool  ReadTextVertexData(OBJECT_DATA* pObj, Util::CSimpleScript* pscript);      // Vertex �f�[�^��ǂ�
    bool  ReadTextFaceData  (OBJECT_DATA* pObj, Util::CSimpleScript* pscript);      // Face �f�[�^��ǂ�
    int   ReadTextFaceLine(WORD* aInfo, WORD* aIndex, float* aUV, int iSize, Util::CSimpleScript* pscript);
    
    // �`��
    enum DRAWSUB_FUNC {
        DRAWSUB_NOFLAG        = 0x00,
        DRAWSUB_VERTEX_NORMAL = 0x01,
        DRAWSUB_TEXTURE_UV    = 0x02,
        DRAWSUB_FUNC_MAX   = 4
    };
    static void  (*(s_funcDrawSub[DRAWSUB_FUNC_MAX]))(OBJECT_DATA*, int&);  // �`��T�u���[�`�� Functor 
    static void  DrawSub(OBJECT_DATA* pObj, int &i);       // �`��T�u���[�`���i�ʖ@���C  UV�}�b�v�����j 
    static void  DrawSubS(OBJECT_DATA* pObj, int &i);      // �`��T�u���[�`���i���_�@���CUV�}�b�v�����j
    static void  DrawSubT(OBJECT_DATA* pObj, int &i);      // �`��T�u���[�`���i�ʖ@���C  UV�}�b�v�L��j
    static void  DrawSubST(OBJECT_DATA* pObj, int &i);     // �`��T�u���[�`���i���_�@���CUV�}�b�v�L��j
    DRAWSUB_FUNC DrawSubChangeMaterial(int idx);           // �}�e���A�������ւ��i�Ԓl;�e�N�X�`���Q�Ɓj

//--------------------------------------------------
// �Q��
//--------------------------------------------------
public:
    // �� 
    inline bool IsEmpty()
    {
        return m_aObject.empty();
    }

    // �܂܂��I�u�W�F�N�g���̎Q��
    inline int GetObjectCount()
    {
        return m_aObject.size();
    }

    // �܂܂��}�e���A�����̎Q��
    inline int GetMaterialCount()
    {
        return m_aMaterial.size();
    }

    // �I�u�W�F�N�g������C���̃C���f�b�N�X��Ԃ��Dreturn -1 �ŃG���[
    inline int GetIndex(char* strName)
    {
        int imax = GetObjectCount();
        for (int i=0; i<imax; i++) {
            if (m_aObject[i].strName.compare(strName) == 0) return i;
        }
        return -1;
    }

    // ���O�ɑΉ������}�e���A����Ԃ��Dreturn NULL �ŃG���[
    inline CMaterial* GetMaterial(char* strName)
    {
        int imax = GetMaterialCount();
        for (int i=0; i<imax; i++) {
            if (m_aMaterial[i].strName.compare(strName) == 0) return &(m_aMaterial[i].material);
        }
        return NULL;
    }
    
    // �C���f�b�N�X�w��Ń}�e���A����Ԃ��D
    inline CMaterial* GetMaterial(int idx)
    {
        return (idx < m_aMaterial.size()) ? &(m_aMaterial[idx].material) : NULL;
    }

    // Compile() �ō쐬����Display List �� ID ��Ԃ�
    inline GLuint GetDisplayListID()
    {
        return m_uiID;
    }

    // Compile() �ō쐬����Display List �� �� ��Ԃ�
    inline int GetDisplayListCount()
    {
        return m_iDisplayListCount;
    }
    
    // Compile() �Ő������� Display List ���Ăяo��
    inline void CallList()
	{
    	for (int i=0; i<m_iDisplayListCount; i++) glCallList(m_uiID + i);
    }
    
    // Compile() �Ő������� Display List ���Ăяo���i�C���f�b�N�X�w��j 
    inline void CallList(int idx)
	{
    	glCallList(m_uiID + idx);
    }

	// ���_�̍ŏ��l
	inline CVector& GetVertexMin()
	{
		return m_vcMinimum;
	}

	inline CVector& GetVertexMin(int objidx)
	{
		return m_aObject[objidx].vcMinimum;
	}
	
	// ���_�̍ő�l
	inline CVector& GetVertexMax()
	{
		return m_vcMaximum;
	}

	inline CVector& GetVertexMax(int objidx)
	{
		return m_aObject[objidx].vcMaximum;
	}
};



//--------------------------------------------------------------------------------
// GL::CompileModel()
//--------------------------------------------------------------------------------
// Metasequoia Model Object File ���� OpenGL Display List �𐶐�����D
//--------------------------------------------------------------------------------
inline GLint CompileModel(char* strFileName, bool bSmooth=false)
{
    CSolidModel model;
    GLint       dlidx;
    
    // ���f���ǂݍ���
    if (!model.Load(strFileName)) return -1;

    // Display List ����
    dlidx = glGenLists(1);
    if (dlidx == 0) return 0;
    glNewList(dlidx, GL_COMPILE);
        model.Draw(bSmooth);
    glEndList();

    return dlidx;
}

} // GL

#endif //_CORE_GLSOLIDMODEL_INCLUDED

