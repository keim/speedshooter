//--------------------------------------------------------------------------------
//  OpenGL Wrapping Class (GL::CSolidModel)
//    Copyright  (C)  2005  kei mesuda  
//    (http://www.yomogi.sakura.ne.jp/~si)
//--------------------------------------------------------------------------------
//  GL::CSolidModel   
//  固定 Model クラス．基本的には Metasequoia → DisplayList ラップ．
//  サポート  ；法線計算，材料管理，頂点色，UVMap，スムースシェイディング(予定)
//  非サポート；点(独立頂点), 線分, 4頂点平面の保存(3頂点に分解して保存)
//  構造； CSolidModel { MATERIAL_DATA, OBJECT_DATA { VERTEX_DATA, FACE_DATA }; };
//  1. Load() で Metasequoia Object File の読める部分だけ読込(4頂点平面は3頂点に分解)
//  2.1 Compile() -> CallList() で，Texture登録 -> DisplayListの 作成 -> DisplayList 描画
//  2.2 RegisterTexture() -> Draw() で，Texture登録 -> 直描画
//  2.3 DrawSolid() で，マテリアルを無視して形状のみ描画
//  
//  1. SetTextureManager() で外部のテクスチャマネージャを関連付ける（通常デフォルト）．
//  2. AllocObject() でオブジェクト数の確保（Allocしない場合Load()で勝手に確保される）
//  3. LoadBinary() でオリジナルバイナリデータ読込
//  4. Save() でオリジナルバイナリデータ保存
//  5. MakeArray() で InterleavedArrays(GL_TRIANGLES) に変換．
//
//--------------------------------------------------------------------------------
// GL::CompileModel()
// Metasequoia Model Object File から OpenGL Display List を生成して ID を返す．
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
// 構造体
//--------------------------------------------------
public:
    // Vertex 構造体
    struct VERTEX_DATA {
        CVector vc;                             // 頂点
        DWORD   col;                            // 頂点色
    };

    // Face 構造体
    struct FACE_DATA {
        char        mat;                        // マテリアル番号(-1 で標準色)
        WORD        idx[3];                     // 頂点インデックス
        VECTOR2D    uv[3];                      // テクスチャ座標
        // 以下記録しない．モデル生成の度に計算する 
        CVector     vcFaceNormal;               // 面法線データ
        CVector     vcVertexNormal[3];          // 頂点法線データ
    };

    // Object 構造体
    class OBJECT_DATA {
    public:
        std::string              strName;       // オブジェクト名
        std::vector<VERTEX_DATA> aVertex;       // 頂点データ
        std::vector<FACE_DATA>   aFace;         // 面データ
        float                    fFacet;        // Smoothing Angle [rad]
        DWORD                    dwColor;       // 色
        // 以下記録しない．モデル生成の度に計算する
        CVector                  vcMinimum;     // オブジェクトを構成する頂点の x,y,z 最小値
        CVector                  vcMaximum;     // オブジェクトを構成する頂点の x,y,z 最大値

        OBJECT_DATA()
        {
            fFacet = 0.0f;
            dwColor = 0xffffffff;
        }

        ~OBJECT_DATA()
        {
        }
    };

    // Material 構造体
    class MATERIAL_DATA {
    public:
        std::string              strName;        // マテリアル名
        CMaterial                material;       // 材料パラメータ 
        bool                     bVertexColor;   // 頂点色を採用する(0/1)
        
        MATERIAL_DATA()
        {
            bVertexColor = false;
        }
        
        ~MATERIAL_DATA()
        {
        }
    };

//--------------------------------------------------
// メンバ変数
//--------------------------------------------------
private:
    std::vector<MATERIAL_DATA> m_aMaterial;          // 材料データ本体
    std::vector<OBJECT_DATA>   m_aObject;            // オブジェクトデータ本体

    GLuint                     m_uiID;               // DisplayList ID
    int                        m_iDisplayListCount;  // DisplayList Count
	CVector                    m_vcMinimum;          // 頂点の最小値
	CVector                    m_vcMaximum;          // 頂点の最大値

    CTextureManager*           m_pTextureManager;       // テクスチャ管理クラスのポインタ
    static CTextureManager     s_DefaultTextureManager; // 共有テクスチャ管理インスタンス

//--------------------------------------------------
// メンバ関数
//--------------------------------------------------
public:
    // コンストラクタ
    CSolidModel();
    ~CSolidModel();

    // 設定
    void  SetTextureManager(CTextureManager* ptm);      // 外部テクスチャ管理インスタンスを使用する
    static CTextureManager* GetDefaultTextureManager(); // デフォルトテクスチャ管理インスタンスの取得

    // ファイル入力
    bool  Load(char* strFileName);              // テキストデータ，Metasequoia File の読みこみ

    // 操作
    void  Free(bool bFreeList = true);                  // 全メモリの解放（bFreeList = false で DisplayListは破棄しない）
    void  RegisterTexture();                            // 使用するテクスチャを登録する
    void  UnRegisterTexture();                          // 使用するテクスチャの登録を抹消する
    void  MultMatrix(CMatrix &mat);                     // 座標変換
    void  MultMatrix(int iObjectIndex, CMatrix &mat);   // 座標変換（オブジェクト単位）
    void  MultQuat(CVector &qt);                        // 回転クォータニオン変換
    void  MultQuat(int iObjectIndex, CVector &qt);      // 回転クォータニオン変換（オブジェクト単位）
    void  CalcFaceNormal();                             // 法線を計算する
    void  CalcFaceNormal(int iObjectIndex);             // 法線を計算する（オブジェクト単位）
	void  CalcVertexRange();                            // 頂点の存在する範囲を計算
	void  CalcVertexRange(int iObjectIndex);            // 頂点の存在する範囲を計算（オブジェクト単位）

    // 描画
    void  Draw(bool bSmooth=false);                     // 素直に描画
    void  Draw(int iObjectIndex, bool bSmooth=false);   // 素直に描画（オブジェクト単位）
    void  DrawSolid();                                  // モデルだけ描画
    void  DrawSolid(int iObjectIndex);                  // モデルだけ描画（オブジェクト単位）

    // ディスプレイリスト関連
    GLuint Compile(bool bCompose=false, bool bSmooth=false);    // Display List の作成, bCompose=false でオブジェクト単位で別のDisplay Listを作る
    void   FreeList();                                          // Display List の破棄
//  void   CallList();                                          // Compile() で生成した Display List を呼び出す 
//  void   CallList(int idx);                                   // Compile() で生成した Display List を呼び出す（インデックス指定） 

    // 変換
    bool   MakeArray(int idxObject, CArray* pArray, bool bSmooth=false);    // CArrayの作成（オブジェクト単位）

// 内部で使用
private:
    // データ読み込み 
    int   TextReadLine(Util::CSimpleScript* pscript);   // 空白行とコメントアウトを飛ばしてReadLine
    bool  TextSkipChunk(Util::CSimpleScript* pscript);  // ...} を飛ばす

    bool  CheckHeader(Util::CSimpleScript* pscript);                                // ヘッダチェック
    bool  ReadTextMaterialChunk(Util::CSimpleScript* pscript);                      // Material チャンクを読む 
    bool  ReadTextMaterialData(MATERIAL_DATA* pMat, Util::CSimpleScript* pscript);  // Material データを読む 
    bool  ReadTextObjectData(OBJECT_DATA* pObj, Util::CSimpleScript* pscript);      // Object データを読む
    bool  ReadTextVertexData(OBJECT_DATA* pObj, Util::CSimpleScript* pscript);      // Vertex データを読む
    bool  ReadTextFaceData  (OBJECT_DATA* pObj, Util::CSimpleScript* pscript);      // Face データを読む
    int   ReadTextFaceLine(WORD* aInfo, WORD* aIndex, float* aUV, int iSize, Util::CSimpleScript* pscript);
    
    // 描画
    enum DRAWSUB_FUNC {
        DRAWSUB_NOFLAG        = 0x00,
        DRAWSUB_VERTEX_NORMAL = 0x01,
        DRAWSUB_TEXTURE_UV    = 0x02,
        DRAWSUB_FUNC_MAX   = 4
    };
    static void  (*(s_funcDrawSub[DRAWSUB_FUNC_MAX]))(OBJECT_DATA*, int&);  // 描画サブルーチン Functor 
    static void  DrawSub(OBJECT_DATA* pObj, int &i);       // 描画サブルーチン（面法線，  UVマップ無し） 
    static void  DrawSubS(OBJECT_DATA* pObj, int &i);      // 描画サブルーチン（頂点法線，UVマップ無し）
    static void  DrawSubT(OBJECT_DATA* pObj, int &i);      // 描画サブルーチン（面法線，  UVマップ有り）
    static void  DrawSubST(OBJECT_DATA* pObj, int &i);     // 描画サブルーチン（頂点法線，UVマップ有り）
    DRAWSUB_FUNC DrawSubChangeMaterial(int idx);           // マテリアル差し替え（返値;テクスチャ参照）

//--------------------------------------------------
// 参照
//--------------------------------------------------
public:
    // 空か 
    inline bool IsEmpty()
    {
        return m_aObject.empty();
    }

    // 含まれるオブジェクト数の参照
    inline int GetObjectCount()
    {
        return m_aObject.size();
    }

    // 含まれるマテリアリ数の参照
    inline int GetMaterialCount()
    {
        return m_aMaterial.size();
    }

    // オブジェクト名から，そのインデックスを返す．return -1 でエラー
    inline int GetIndex(char* strName)
    {
        int imax = GetObjectCount();
        for (int i=0; i<imax; i++) {
            if (m_aObject[i].strName.compare(strName) == 0) return i;
        }
        return -1;
    }

    // 名前に対応したマテリアルを返す．return NULL でエラー
    inline CMaterial* GetMaterial(char* strName)
    {
        int imax = GetMaterialCount();
        for (int i=0; i<imax; i++) {
            if (m_aMaterial[i].strName.compare(strName) == 0) return &(m_aMaterial[i].material);
        }
        return NULL;
    }
    
    // インデックス指定でマテリアルを返す．
    inline CMaterial* GetMaterial(int idx)
    {
        return (idx < m_aMaterial.size()) ? &(m_aMaterial[idx].material) : NULL;
    }

    // Compile() で作成したDisplay List の ID を返す
    inline GLuint GetDisplayListID()
    {
        return m_uiID;
    }

    // Compile() で作成したDisplay List の 数 を返す
    inline int GetDisplayListCount()
    {
        return m_iDisplayListCount;
    }
    
    // Compile() で生成した Display List を呼び出す
    inline void CallList()
	{
    	for (int i=0; i<m_iDisplayListCount; i++) glCallList(m_uiID + i);
    }
    
    // Compile() で生成した Display List を呼び出す（インデックス指定） 
    inline void CallList(int idx)
	{
    	glCallList(m_uiID + idx);
    }

	// 頂点の最小値
	inline CVector& GetVertexMin()
	{
		return m_vcMinimum;
	}

	inline CVector& GetVertexMin(int objidx)
	{
		return m_aObject[objidx].vcMinimum;
	}
	
	// 頂点の最大値
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
// Metasequoia Model Object File から OpenGL Display List を生成する．
//--------------------------------------------------------------------------------
inline GLint CompileModel(char* strFileName, bool bSmooth=false)
{
    CSolidModel model;
    GLint       dlidx;
    
    // モデル読み込み
    if (!model.Load(strFileName)) return -1;

    // Display List 生成
    dlidx = glGenLists(1);
    if (dlidx == 0) return 0;
    glNewList(dlidx, GL_COMPILE);
        model.Draw(bSmooth);
    glEndList();

    return dlidx;
}

} // GL

#endif //_CORE_GLSOLIDMODEL_INCLUDED

