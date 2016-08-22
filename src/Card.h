//------------------------------------------------------------
// カード
//------------------------------------------------------------
class CCard
{
friend class CCardSuit;
private:
    CCard*  pPrev;  // 山札内にある場合，その次のカードのポインタ
    CCard*  pNext;  // 山札内にある場合，その前のカードのポインタ
    BYTE    n;      // カードの種類．0-51．÷4 で数字，余りがマーク．

public:
    CCard()
    {
        pPrev = NULL;
        pNext = NULL;
        n = 255;
    }

	// 数字取得
    inline int GetCardNumber()
    {
        return GET_CARD_NUMBER(n);
    }

	// マーク取得
    inline int GetCardSymbol()
    {
        return GET_CARD_SYMBOL(n);
    }

	// テクスチャ上のUV座標を計算
    inline void GetUV(float *pu, float *pv)
    {
        int num = GET_CARD_NUMBER(n);
        if (num == 12) {
            *pu = 0.0f;
            *pv = GET_CARD_SYMBOL(n) * 0.125f + 0.5f;
        } else {
            *pu = num * 0.078125f;
            *pv = GET_CARD_SYMBOL(n) * 0.125f;
        }
    }
};





//------------------------------------------------------------
// 山札
//   class CCard を Chain List 構造で保持
//------------------------------------------------------------
class CCardSuit
{
private:
    CCard   m_HeadCard;     // 山札の先頭のカードの前にあるダミーカード
    CCard   m_TailCard;     // 山札の末尾のカードの次にあるダミーカード
    CCard   m_Cards[52];    // カードの実体
    int     m_iCount;       // 山札の数

public:
    CCardSuit()
    {
        Reset();
    }

private:
	// idx 番目のカードを山札から一枚引く．
    CCard* RemoveCard(int idx)
    {
        CCard* pCard = m_HeadCard.pNext;
        for (int i=0; i<idx; i++) pCard = pCard->pNext;
        pCard->pNext->pPrev = pCard->pPrev;
        pCard->pPrev->pNext = pCard->pNext;
        pCard->pPrev = NULL;
        pCard->pNext = NULL;
        m_iCount--;
        return pCard;
    }

public:
    // リセット
    void Reset()
    {
        int i;

        // Set Card No
        for (i=0; i<52; i++) m_Cards[i].n = i;
        // Set Card Suit Chain
        for (i=1; i<51; i++) {
            m_Cards[i].pPrev = &m_Cards[i-1];
            m_Cards[i].pNext = &m_Cards[i+1];
        }
        m_HeadCard.pNext  = &m_Cards[0];
        m_Cards[0].pPrev  = &m_HeadCard;
        m_Cards[0].pNext  = &m_Cards[1];
        m_Cards[51].pPrev = &m_Cards[50];
        m_Cards[51].pNext = &m_TailCard;
        m_TailCard.pPrev  = &m_Cards[51];
        m_iCount = 52;
    }

    // カードを1枚引く
    CCard* Draw()
    {
        if (m_iCount == 0) return NULL;
        int n = randMT() % m_iCount;
        return RemoveCard(n);
    }

    // カードを1枚山に戻す
    void   Drop(CCard* pCard)
    {
        pCard->pPrev = m_TailCard.pPrev;
        pCard->pNext = &m_TailCard;
        m_TailCard.pPrev->pNext = pCard;
        m_TailCard.pPrev        = pCard;
        m_iCount++;
    }

    // n で指定したカードの実体を取得
    CCard* GetCard(int n)
    {
        return &(m_Cards[n]);
    }
};


