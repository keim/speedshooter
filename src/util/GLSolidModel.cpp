//--------------------------------------------------------------------------------
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
#include "std_include.h"
#include "include_opengl.h"

namespace GL {
//--------------------------------------------------
// スタティックメンバ
//--------------------------------------------------
// 共有テクスチャ管理インスタンス
CTextureManager  CSolidModel::s_DefaultTextureManager;

// 描画サブルーチン Functor 
void  (*(CSolidModel::s_funcDrawSub[CSolidModel::DRAWSUB_FUNC_MAX]))(CSolidModel::OBJECT_DATA*, int&) = {
    CSolidModel::DrawSub,
    CSolidModel::DrawSubS,
    CSolidModel::DrawSubT,
    CSolidModel::DrawSubST
};


//--------------------------------------------------
// コンストラクタ
//--------------------------------------------------
CSolidModel::CSolidModel()
{
    m_uiID = 0;
    m_iDisplayListCount = 0;
	// デフォルトテクス管理インスタンス
	m_pTextureManager = &s_DefaultTextureManager;
}

CSolidModel::~CSolidModel()
{
    Free();
}

//--------------------------------------------------
// テクスチャ管理インスタンス
//--------------------------------------------------
// 外部のテクスチャ管理インスタンスを使用する
void CSolidModel::SetTextureManager(CTextureManager* ptm)
{
    m_pTextureManager = ptm;
}

// デフォルトテクスチャ管理インスタンスを取得する
CTextureManager* CSolidModel::GetDefaultTextureManager()
{
    return &s_DefaultTextureManager;
}

//--------------------------------------------------
// ファイル操作
//--------------------------------------------------
// 読込
bool CSolidModel::Load(char* strFileName)
{
    int     iTokenCount, isize;
    int     iObjectIndex;
    const char* str;

    Util::CSimpleScript script(strFileName);
    script.SetWhiteChar(" (),;\t\r");
    script.SetTokenChar("{}");

    // ヘッダチェック
    if (!CheckHeader(&script)) return false;
    
    // 読みこみ
    iObjectIndex = 0;
    while ((iTokenCount = TextReadLine(&script)) != -1) {
        // 最初のトークン
        str = script.GetHeadToken();
        
        // マテリアル
        if (stricmp(str, "Material") == 0) {                    
            // データ数の取得 
            str = script.GetNextInt(&isize);
            if (isize == 0)                      return false;
            // メモリ領域の確保
            m_aMaterial.resize(isize);
            // データの読みこみ
            if (!ReadTextMaterialChunk(&script)) return false;
        } else 

        // オブジェクト
        if (stricmp(str, "Object") == 0) {
            // メモリ領域の確保
            m_aObject.resize(iObjectIndex + 1);
            // データの読みこみ
            if (!ReadTextObjectData(&m_aObject[iObjectIndex], &script)) return false;
            // インデックスのインクリメント 
            iObjectIndex++;
        } else 

        // 終わり
        if (stricmp(str, "Eof") == 0) {
            break;
        } else 

        // 未対応チャンクのスキップ 
        {
            if (script.GetToken(iTokenCount - 1)[0] != '{') return false;
            if (!TextSkipChunk(&script)) return false;
        }
    }

    // 不完全でも読み込む 
    CalcFaceNormal();
    return true;
}

// 一行読込
int CSolidModel::TextReadLine(Util::CSimpleScript* pscript)
{
    int iTokenCount;
    const char* str;
    
    // 無意味な行は飛ばす 
    while ((iTokenCount = pscript->ReadLine()) != -1) {
        // 空白行
        if (iTokenCount == 0)          continue;
        str = pscript->GetHeadToken();
        // コメントアウト
        if (memcmp(str, "//", 2) == 0) continue;
        if (*str == '#')               continue;
        break;
    }
    
    // pscript->GetNextToken() で最初のトークンを読み込めるようにリセット 
    pscript->ResetSequence();
    return iTokenCount;
}

// チャンクスキップ 
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

// ヘッダチェック
bool CSolidModel::CheckHeader(Util::CSimpleScript* pscript)
{
    // 最初は必ず二文字
    if (TextReadLine(pscript) != 2) return false;
    const char* str = pscript->GetHeadToken();

    // "GLSolidModel Ver1" (拡張)
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
    
    // Header 無し 
    {
        return false;
    }

    return true;
}

// マテリアルチャンクの読込
bool CSolidModel::ReadTextMaterialChunk(Util::CSimpleScript* pscript)
{
    const char* str;
    MATERIAL_DATA* pMat;
    int idx;
        
    // マテリアルチャンクの読み込み 
    for (idx = 0; TextReadLine(pscript) != -1; idx++) {
        
        // 最初のトークン
        str = pscript->GetHeadToken();

        // 最初のトークンが "}" なら終了 
        if (*str == '}') return true;
        
        // 初期設定
        if (idx >= m_aMaterial.size()) return false;
        m_aMaterial[idx].material.m_vColor.Set(0.5f, 0.5f, 0.5f, 1.0f);
                
        // マテリアルデータの読み込み 
        if (!ReadTextMaterialData(&(m_aMaterial[idx]), pscript)) return false;
    }
    
    // ループを脱出した場合，途中で読み込み終了．エラー 
    return false;
}

// テキストマテリアルデータの読込
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
    
    // 識別名の取得
    str = pscript->GetHeadToken();
    if (str[0] != '"') return false;
    pMat->strName.assign(&(str[1]), strlen(str)-2);

    // マテリアル情報の取得 
    for (str = pscript->GetNextToken(); str != NULL; str = pscript->GetNextToken()) {
        // 色
        if (stricmp(str, "col") == 0) {
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.r)) == NULL) break;
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.g)) == NULL) break;
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.b)) == NULL) break;
            if (pscript->GetNextFloat(&(pMat->material.m_vColor.a)) == NULL) break;
        } else 
        
        // 頂点色の有無 
        if (stricmp(str, "vcol") == 0) {
            if (pscript->GetNextInt(&i) == NULL) break;
            pMat->bVertexColor = (i!=0);
        } else 
        
        // 光沢 
        if (stricmp(str, "amb") == 0) {                                 // 環境光
            if (pscript->GetNextFloat(&a) == NULL) break;
        } else 
        if (stricmp(str, "dif") == 0) {                                 // 拡散光
            if (pscript->GetNextFloat(&d) == NULL) break;
        } else 
        if (stricmp(str, "spec") == 0 || stricmp(str, "spc") == 0) {    // 鏡面反射光（"spc"は拡張）
            if (pscript->GetNextFloat(&s) == NULL) break;
        } else 
        if (stricmp(str, "power") == 0 || stricmp(str, "phong") == 0) { // 鏡面反射の強さ（"phong"は拡張）
            if (pscript->GetNextFloat(&p) == NULL) break;
        } else 
        if (stricmp(str, "emi") == 0) {                                 // 自己発光
            if (pscript->GetNextFloat(&e) == NULL) break;
        } else 

        // テクスチャ
        if (stricmp(str, "tex") == 0) {                                 // Texture Bmp のパス取得
            if ((str = pscript->GetNextToken()) == NULL) break;
            strcpy(strFileTex, pscript->GetFileDir());
            strncat(strFileTex, &(str[1]), strlen(str)-2);
        } else 
        if (stricmp(str, "aplane") == 0) {                              // Alpha Bmp のパス取得
            if ((str = pscript->GetNextToken()) == NULL) break;
            strcpy(strFileAlp, pscript->GetFileDir());
            strncat(strFileAlp, &(str[1]), strlen(str)-2);
        } else 
        
        // 未対応
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

    // 読み込んだデータから CMaterial 設定
    // 光沢設定
    pMat->material.CalcParam(a, d, s, p * 0.01f, e);
    // テクスチャの読み込み
    if (strFileTex[0] != '\0' && m_pTextureManager != NULL) {
        if (strFileAlp[0] != '\0') ptex = m_pTextureManager->Load(strFileTex, strFileAlp);
        else                       ptex = m_pTextureManager->Load(strFileTex, 3);
        pMat->material.m_pTexture = ptex;
    }
    
    return true;
}

// オブジェクトチャンクの読込 
bool CSolidModel::ReadTextObjectData(CSolidModel::OBJECT_DATA* pObj, Util::CSimpleScript* pscript)
{
    const char* str;
    float   f, r, g, b, a;

    // 識別名の取得
    if ((str = pscript->GetNextToken()) == NULL) return false;
    pObj->strName.assign(&(str[1]), strlen(str)-2);

    // 情報読込
    while (TextReadLine(pscript) != -1) {
        
        // 先頭トークン
        str = pscript->GetHeadToken();
        
        // color(alphaは拡張) 
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
        
        // 未対応タグは全部飛ばす 
    }
    
    return false;
}

// 頂点データの読込
bool CSolidModel::ReadTextVertexData(CSolidModel::OBJECT_DATA* pObj, Util::CSimpleScript* pscript)
{    
    float f;
    int idx, iSize;
    const char* str = pscript->GetNextInt(&iSize);
    if (iSize == 0) return false;

    // メモリ領域確保
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
            // 頂点色（拡張）
            pscript->GetNextDWORD(&(pObj->aVertex[idx].col));
        }}
        idx++;
    }
    return false;
}

// 面データの読込
bool CSolidModel::ReadTextFaceData(CSolidModel::OBJECT_DATA* pObj, Util::CSimpleScript* pscript)
{
    int iIndexMax, iSize, i;
    
    // 平面数の取得 
    pscript->GetNextInt(&iSize);
    if (iSize == 0) return false;
    
    // 仮置き場確保 (std::vector だと何故か Segment Fault)
    WORD*  aInfo  = memCreate(WORD,  iSize);
    WORD*  aIndex = memCreate(WORD,  iSize*4);
    float* aUV    = memCreate(float, iSize*8);
    if (aInfo == NULL || aIndex == NULL || aUV == NULL) return false;

    // テキストから面情報を取得 
    iIndexMax = ReadTextFaceLine(aInfo, aIndex, aUV, iSize, pscript);
    
    // 成功ならFACE_DATAの生成
    if (iIndexMax > 0) {
        
        // 面数のカウント/領域確保
        int iFaceCount = 0;
        for (i=0; i<iIndexMax; i++) iFaceCount += (aInfo[i] & 0xff) - 2;
        pObj->aFace.resize(iFaceCount);

        // FACE_DATA 構造体にコピー
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
            
            // ４頂点の場合は，２つの平面を生成
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

   
    // 仮置き場破棄 
    memDelete(aInfo);
    memDelete(aIndex);
    memDelete(aUV);
   
    return (iIndexMax > 0);
}

// テキストから面情報を取得 
int CSolidModel::ReadTextFaceLine(WORD* aInfo, WORD* aIndex, float* aUV, int iSize, Util::CSimpleScript* pscript)
{
    const char* str;
    int iVertexCount, idx, base_idx, iv, i;

    
    for (idx = 0; true; idx++) {
        // １行読み込み
        if (TextReadLine(pscript) == -1) return 0;
        
        // 初期設定
        aInfo[idx] = 0;
        
        // 頂点数の取得 
        str = pscript->GetNextInt(&iVertexCount);
        
        // 閉じ括弧で終了
        if (*str == '}') break;
        
        // オーバーフロー/頂点数のチェック 
        if ((idx >= iSize) || (iVertexCount != 3 && iVertexCount != 4)) return 0;

        // トークンの取得
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
// 操作
//--------------------------------------------------
// 行列変換 
void CSolidModel::MultMatrix(CMatrix &mat)
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) MultMatrix(i, mat);
}

// 行列変換 
void CSolidModel::MultMatrix(int iModelIndex, CMatrix &mat)
{
    int i;
    OBJECT_DATA* pobj = &(m_aObject[iModelIndex]);
    int          imax = pobj->aVertex.size();
    for (i=0; i<imax; i++) mat.Transform(pobj->aVertex[i].vc, pobj->aVertex[i].vc);
    CalcFaceNormal(iModelIndex);
}

// 回転クォータニオン変換 
void CSolidModel::MultQuat(CVector &qt)
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) MultQuat(i, qt);
}

// 回転クォータニオン変換 
void CSolidModel::MultQuat(int iModelIndex, CVector &qt)
{
    int i;
    OBJECT_DATA* pobj = &(m_aObject[iModelIndex]);
    int          imax = pobj->aVertex.size();
    for (i=0; i<imax; i++) pobj->aVertex[i].vc.QRotate(qt);
    CalcFaceNormal(iModelIndex);
}

// 面法線／頂点法線の計算
void CSolidModel::CalcFaceNormal()
{
    int imax = m_aObject.size();
    for (int i=0; i<imax; i++) CalcFaceNormal(i);
}

// 面法線／頂点法線の計算
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
        // 面法線の計算
        vc1 = pobj->aVertex[pobj->aFace[i].idx[0]].vc - pobj->aVertex[pobj->aFace[i].idx[1]].vc;
        vc2 = pobj->aVertex[pobj->aFace[i].idx[2]].vc - pobj->aVertex[pobj->aFace[i].idx[0]].vc;
        pobj->aFace[i].vcFaceNormal.Cross(vc1, vc2);
        pobj->aFace[i].vcFaceNormal.Normalize();
        pobj->aFace[i].vcVertexNormal[0] = pobj->aFace[i].vcFaceNormal;
        pobj->aFace[i].vcVertexNormal[1] = pobj->aFace[i].vcFaceNormal;
        pobj->aFace[i].vcVertexNormal[2] = pobj->aFace[i].vcFaceNormal;
    }
}

// 範囲計算
void CSolidModel::CalcVertexRange()
{
	if (IsEmpty()) return;
	CalcVertexRange(0);
	m_vcMinimum.Copy(m_aObject[0].vcMinimum);
    m_vcMaximum.Copy(m_aObject[0].vcMaximum);
    int imax = m_aObject.size();
    for (int i=1; i<imax; i++) CalcVertexRange(i);
}

// 範囲計算
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
// 描画
//--------------------------------------------------
// 素直に描画
void CSolidModel::Draw(bool bSmooth/*=false*/)
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) {
        Draw(i, bSmooth);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

// 素直に描画（オブジェクト単位）
void CSolidModel::Draw(int iObjectIndex, bool bSmooth/*=false*/)
{
    if (iObjectIndex >= m_aObject.size()) return;
    OBJECT_DATA* pObj = &m_aObject[iObjectIndex];

    int imax = pObj->aFace.size();
    if (imax == 0) return;

    int i;
    int enFunctor;
    
    // 頂点法線描画フラグ
    DRAWSUB_FUNC enVertexNormal = (bSmooth) ? DRAWSUB_NOFLAG : DRAWSUB_VERTEX_NORMAL;
    
    // 描画
    for (i=0; i<imax;) {
        // マテリアルの差し替え
        enFunctor = DrawSubChangeMaterial(pObj->aFace[i].mat) | enVertexNormal;
        
        // 描画サブルーチンの呼び出し
        glBegin(GL_TRIANGLES);
        s_funcDrawSub[enFunctor](pObj, i);
        glEnd();
    }
}

// マテリアル差し替え（返値;テクスチャ参照フラグ） 
CSolidModel::DRAWSUB_FUNC CSolidModel::DrawSubChangeMaterial(int idx)
{
    if (idx != -1) m_aMaterial[idx].material.Draw();
    return (m_aMaterial[idx].material.m_pTexture == NULL) ? DRAWSUB_NOFLAG : DRAWSUB_TEXTURE_UV;
}

// 描画サブルーチン（面法線，  UVマップ無し） 
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

// 描画サブルーチン（頂点法線，UVマップ無し）
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

// 描画サブルーチン（面法線，  UVマップ有り）
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

// 描画サブルーチン（頂点法線，UVマップ有り）
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

// モデルだけ描画
void  CSolidModel::DrawSolid()
{
    int i;
    int imax = m_aObject.size();
    for (i=0; i<imax; i++) DrawSolid(i);
}

// モデルだけ描画
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

// OpenGL Display List の作成．bCompose==true で単一 / false でオブジェクト数分作成
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

// DisplayList の開放
void CSolidModel::FreeList()
{
    if (m_iDisplayListCount <= 0) return;
    glDeleteLists(m_uiID, m_iDisplayListCount);
    m_iDisplayListCount = 0;
    m_uiID = 0;
}

// Textureを登録する
void CSolidModel::RegisterTexture()
{
    int imax = m_aMaterial.size();
    for (int i=0; i<imax; i++) m_aMaterial[i].material.RegisterTexture();
}

// Textureを登録抹消する
void CSolidModel::UnRegisterTexture()
{
    int imax = m_aMaterial.size();
    for (int i=0; i<imax; i++) m_aMaterial[i].material.UnRegisterTexture();
}

// GL::CArray を作成する
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
// メモリ操作
//--------------------------------------------------
// メモリ領域の全解放（Display List を解放するかは選択）
void CSolidModel::Free(bool bFreeList/*=true*/)
{
    int i;
    
    // Display List の解放
	if (bFreeList) FreeList();

    // Object の解放
    m_aObject.clear();

    // Material の解放
    m_aMaterial.clear();
}

} // GL
