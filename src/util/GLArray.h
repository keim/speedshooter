//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CArray)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  GL::CArray
//  - glInterleavedArray() �̃��b�v�N���X
//  - �P���Ȓ��_���̔z����Ǘ�����D
//  1. SetFormat() �� Alloc() �Ńt�H�[�}�b�g�ݒ��C�̈�m��
//  2. SetInterleavedArrays() ��glInterleavedArray()
//  3. Draw() �� glDrawArrays()�Ƃ��ĕ`��
//  4. IsSupport*() �ŃT�|�[�g���CGet*() �Ńf�[�^�Q��
//--------------------------------------------------------------------------------
#ifndef _CORE_GLARRAY_INCLUDED
#define _CORE_GLARRAY_INCLUDED

#include "Vector.h"
#include "Memory.h"
#pragma message ("<< CORE_MESSAGE >>  include GL::CArray")

namespace GL {
class CArray
{
//--------------------------------------------------
// �t�H�[�}�b�g�Ǘ�
//--------------------------------------------------
private:
    enum GL_ARRAY_FORMAT {
        SUPPORT_V = 0,
        SUPPORT_VT,
        SUPPORT_VN,
        SUPPORT_VNT,
        SUPPORT_VC,
        SUPPORT_VCT,
        GL_ARRAY_FORMAT_MAX
    };

    struct GL_ARRAY_VERTEX_INFO {
        GLenum  enIAFormat;
        char    iSize;
        char    idxT;
        char    idxC;
        char    idxN;
        char    idxV;
    };

//--------------------------------------------------
// �����o�ϐ�
//--------------------------------------------------
private:
    GL_ARRAY_FORMAT m_enFormat;             // �f�[�^�t�H�[�}�b�g
    float*          m_aData;                // �f�[�^�{��
    int             m_iDataArraySize;       // �f�[�^�z��

    static GL_ARRAY_VERTEX_INFO s_info[GL_ARRAY_FORMAT_MAX];    // �e�t�H�[�}�b�g�̏��

//--------------------------------------------------
// �����o�֐�
//--------------------------------------------------
public:
    CArray()
    {
        m_enFormat = SUPPORT_V;
        m_aData = NULL;
        m_iDataArraySize = 0;
    }

    ~CArray()
    {
        Free();
    }

//--------------------------------------------------
// �쐬�j��
//--------------------------------------------------
    // �t�H�[�}�b�g�ݒ�ibNormal �� bColor �͓����Ɏg�p�ł��Ȃ��Dreturn false�j
    bool SetFormat(bool bTexCoord, bool bNormal, bool bColor)
    {
        if ((bNormal && bColor) || m_iDataArraySize != 0) return false;
        m_enFormat = (GL_ARRAY_FORMAT)(((bTexCoord) ? (1) : (0)) | ((bNormal) ? (2) : (0)) | ((bColor) ? (4) : (0)));

        return true;
    }

    // �m��/�Ċm��
    bool Alloc(int size)
    {
        memRealloc(m_aData, s_info[m_enFormat].iSize * size);
        if (m_aData == NULL) return false;
        m_iDataArraySize = size;
        return true;
    }

    // �ǉ��m��
    bool Expand(int size)
    {
        memExpand(m_aData, s_info[m_enFormat].iSize * size);
        if (m_aData == NULL) return false;
        m_iDataArraySize = size;
        return true;
    }

    // ���
    void Free()
    {
        memDelete(m_aData);
        m_iDataArraySize = 0;
    }

//--------------------------------------------------
// ���Q��
//--------------------------------------------------
    // ���_�@���̃T�|�[�g
    inline int IsSupportNormal()
    {
        return (m_enFormat & 2);
    }

    // ���_�F�̃T�|�[�g
    inline int IsSupportColor()
    {
        return (m_enFormat & 4);
    }

    // �e�N�X�`�����W�̃T�|�[�g
    inline int IsSupportTexCoord()
    {
        return (m_enFormat & 1);
    }

    // Vertex �Q��
    inline CVector* GetVertex(int idx)
    {
        return (CVector*)(&(m_aData[idx * s_info[m_enFormat].iSize + s_info[m_enFormat].idxV]));
    }

    // Normal �Q��
    inline float* GetNormal(int idx)
    {
        return &(m_aData[idx * s_info[m_enFormat].iSize + s_info[m_enFormat].idxN]);
    }

    // Color �Q��
    inline DWORD* GetColor(int idx)
    {
        return (DWORD*)(&(m_aData[idx * s_info[m_enFormat].iSize + s_info[m_enFormat].idxC]));
    }

    // TexCoord �Q��
    inline VECTOR2D* GetTexCoord(int idx)
    {
        return (VECTOR2D*)(&(m_aData[idx * s_info[m_enFormat].iSize + s_info[m_enFormat].idxT]));
    }

    // �����擾
    inline void* GetNext(void* ptr)
    {
        void* nextptr = (void*)(&(((float*)ptr)[s_info[m_enFormat].iSize]));
        if (nextptr >= &(m_aData[memLength(m_aData)])) return NULL;
        return nextptr;
    }

    // �f�[�^���Q��
    inline int GetDataCount()
    {
        return m_iDataArraySize;
    }

//--------------------------------------------------
// ����
//--------------------------------------------------
    // �s���Z
    inline void MultMatrix(CMatrix& mat)
    {
        CVector* pVertex;
        for (pVertex = GetVertex(0); pVertex != NULL; pVertex = (CVector*)GetNext(pVertex)) {
            mat.Transform(*pVertex, *pVertex);
        }
    }

//--------------------------------------------------
// �`��
//--------------------------------------------------
    // �z��ݒ�
    inline void SetInterleavedArrays()
    {
        glInterleavedArrays(s_info[m_enFormat].enIAFormat, s_info[m_enFormat].iSize * sizeof(float), m_aData);
    }

    // �`��
    inline void Draw(GLenum mode)
    {
        Draw(mode, 0, m_iDataArraySize);
    }

    // �`��
    inline void Draw(GLenum mode, int idxStart, int iCount)
    {
        if (iCount == 0) {
            idxStart = 0;
            iCount = m_iDataArraySize;
        }
        glDrawArrays(mode, idxStart, iCount);
    }
};





//--------------------------------------------------------------------------------
//  ���܂��܂�3�����}�`��CArray�����
//--------------------------------------------------------------------------------
typedef struct tag_ARRAY_PARAM {
    CVector  scale;     // �傫��
    CVector  div;       // ������
    VECTOR2D rot_uv;    // �e�N�X�`���̌J��Ԃ���
    bool     bTexCoord;
    bool     bNormal;
    bool     bColor;
} ARRAY_PARAM;

//--------------------------------------------------
// ��]�ȉ~��
// ���e�GpParam->div.x * 2 + 2 �̔z�񖈂�GL_TRIANGL_STRIP �� pParam->div.y ��`��
// ARRAY_PARAM::scale  ��]�ȉ~�̂̔��a (x,y,z)
// ARRAY_PARAM::div    ��]�ȉ~�̂̕����� (x=�o����/y=�ܐ���)
// ARRAY_PARAM::rot_uv �e�N�X�`���̌J�Ԃ��� (x=�o������u/y=�ܐ�����v)
//--------------------------------------------------
inline int CompSphereArray(CArray* pArray, ARRAY_PARAM* pParam)
{
    int count = ((int)(pParam->div.x) * 2 + 2) * (int)(pParam->div.y);
    pArray->SetFormat(pParam->bTexCoord, pParam->bNormal, pParam->bColor);
    if (!pArray->Alloc(count)) return 0;
    int      i;
    CVector* pv;
    float    cs, sn, nr;
    float    drx, dry, rx, ry, rxmax, u, du, v1, v2, z1, z2, sn1, sn2;
    drx = (pParam->div.x == 0.0f) ? (fPI*0.25f) : (fRD/pParam->div.x);
    dry = (pParam->div.y == 0.0f) ? (fPI*0.25f) : (fPI/pParam->div.y);
    du = (pParam->div.x == 0.0f) ? (pParam->rot_uv.x * 0.25f) : (pParam->rot_uv.x/pParam->div.x);
    rxmax = fRD + drx*0.5f;

    i = 0;
    for (ry=0; ry<fPI; ry+=dry) {
        sn = sinf(ry);
        cs = cosf(ry);
        z1 = cs * pParam->scale.z;
        v1 = (cs + 1.0f) * pParam->rot_uv.y * 0.5f;
        sn1 = sn;
        sn = sinf(ry + dry);
        cs = cosf(ry + dry);
        v2 = (cs + 1.0f) * pParam->rot_uv.y * 0.5f;
        sn2 = sn;
        u = 0;
        for (rx=0; rx<rxmax; rx+=drx) {
            sn = sinf(rx);
            cs = cosf(rx);
            pArray->GetVertex(i)->SetPosition(cs*sn1*pParam->scale.x, sn*sn1*pParam->scale.y, z1);
            pArray->GetVertex(i+1)->SetPosition(cs*sn2*pParam->scale.x, sn*sn2*pParam->scale.y, z2);
            if (pArray->IsSupportTexCoord()) {
                pArray->GetTexCoord(i)->Set(u, v1);
                pArray->GetTexCoord(i+1)->Set(u, v2);
            }
            if (pArray->IsSupportNormal()) {
                sn = sinf(ry + dry * 0.5f);
                cs = cosf(ry + dry * 0.5f);
                nr = rx + drx * 0.5f;
                pv = (CVector*)(pArray->GetNormal(i));
                pv->Set(cosf(nr)*sn*pParam->scale.x, sinf(nr)*sn*pParam->scale.y, cs*pParam->scale.z);
                pv = (CVector*)(pArray->GetNormal(i+1));
                pv->Set(cosf(nr)*sn*pParam->scale.x, sinf(nr)*sn*pParam->scale.y, cs*pParam->scale.z);
            }
            if (pArray->IsSupportColor()) {
                *(pArray->GetColor(i)) = 0xffffffff;
                *(pArray->GetColor(i+1)) = 0xffffffff;
            }
            i += 2;
            u += du;
        }
    }
    return count;
}
    
//--------------------------------------------------
// �P������(���������̕�������)
// ���e�GpParam->div.x + 2 �̔z���GL_TRIANGL_FAN �ŕ`��
// ARRAY_PARAM::scale  ���̂̌a�ƍ���(x,y=�a/z=����)
// ARRAY_PARAM::div    ���̂̕����� (x=�o����������)
// ARRAY_PARAM::rot_uv �e�N�X�`���̌J�Ԃ��� (x=�o����u/y=��������v)
//--------------------------------------------------
inline int CompConeArray(CArray* pArray, ARRAY_PARAM* pParam)
{
    int divx = (pParam->div.x == 0.0f) ? 4 : (int)(pParam->div.x);
    int count = divx + 2;
    pArray->SetFormat(pParam->bTexCoord, pParam->bNormal, pParam->bColor);
    if (!pArray->Alloc(count)) return 0;
    int      i;
    CVector  vc;
    float    cs, sn, drx, rx;
    drx = fRD / divx;
    
    pArray->GetVertex(0)->SetPosition(0, 0, 0);
    if (pArray->IsSupportTexCoord()) pArray->GetTexCoord(0)->Set(0.5f, 0.5f);
    if (pArray->IsSupportNormal()) ((CVector*)pArray->GetNormal(0))->SetDirection(0, 0, 1);
    if (pArray->IsSupportColor()) *(pArray->GetColor(0)) = 0xffffffff;
    for (i=1, rx=0; i<count; i++, rx+=drx) {
        sn = sinf(rx);
        cs = cosf(rx);
        pArray->GetVertex(i)->SetPosition(cs*pParam->scale.x, sn*pParam->scale.y, pParam->scale.z);
        if (pArray->IsSupportTexCoord()) pArray->GetTexCoord(i)->SetRound(pParam->rot_uv.y, rx*pParam->rot_uv.x);
        if (pArray->IsSupportNormal()) {
            vc.Cross(*(pArray->GetVertex(0)), *(pArray->GetVertex(i-1)));
            vc.Normalize();
            ((CVector*)pArray->GetNormal(i))->SetDirection(vc.x, vc.y, vc.z);
        }
        if (pArray->IsSupportColor())   *(pArray->GetColor(i)) = 0xffffffff;
    }
    return count;
}

//--------------------------------------------------
// �P������(���������̕�������)
// ���e�G(pParam->div.x * 2) + 2 �̔z���GL_TRIANGL_STRIP �ŕ`��
// ARRAY_PARAM::scale  ����̏��/����̌a�ƍ���(x=���a/y=����a/z=����)
// ARRAY_PARAM::div    ����̕����� (x=�o����������)
// ARRAY_PARAM::rot_uv �e�N�X�`���̌J�Ԃ��� (x=�o����u/y=��������v)
//--------------------------------------------------
inline int CompTowerArray(CArray* pArray, ARRAY_PARAM* pParam)
{
    int divx = (pParam->div.x == 0.0f) ? 4 : (int)(pParam->div.x);
    int count = divx * 2 + 2;
    pArray->SetFormat(pParam->bTexCoord, pParam->bNormal, pParam->bColor);
    if (!pArray->Alloc(count)) return 0;
    int      i;
    CVector  vc1, vc2;
    float    cs, sn, drx, rx, u, du;
    drx = fRD  /(float)divx;
    du  = (pParam->rot_uv.x == 0.0f) ? 0.0f : (1.0f /((float)divx * pParam->rot_uv.x));
    
    for (i=0, rx=0, u=0.0f; i<count; i+=2, rx+=drx, u+=du) {
        sn = sinf(rx);
        cs = cosf(rx);
        pArray->GetVertex(i)->SetPosition(cs*pParam->scale.x, sn*pParam->scale.x, 0.0f);
        pArray->GetVertex(i+1)->SetPosition(cs*pParam->scale.y, sn*pParam->scale.y, pParam->scale.z);
        if (pArray->IsSupportTexCoord()) {
            pArray->GetTexCoord(i)->Set(u, 0.0f);
            pArray->GetTexCoord(i+1)->Set(u, pParam->rot_uv.y);
        }
        if (pArray->IsSupportColor()) {
            *(pArray->GetColor(i)) = 0xffffffff;
            *(pArray->GetColor(i+1)) = 0xffffffff;
        }
    }
    if (pArray->IsSupportNormal()) {
        for (i=0; i<count-2; i++) {
            vc1.Sub(*pArray->GetVertex(i), *pArray->GetVertex(i+1));
            vc2.Sub(*pArray->GetVertex(i+2), *pArray->GetVertex(i+1));
            vc1.Cross(vc2);
            vc1.Normalize();
            ((CVector*)pArray->GetNormal(i))->SetDirection(vc1.x, vc1.y, vc1.z);
        }
    }
    return count;

}

} //namespace GL 

#endif  // _CORE_GLARRAY_INCLUDED
