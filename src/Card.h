//------------------------------------------------------------
// �J�[�h
//------------------------------------------------------------
class CCard
{
friend class CCardSuit;
private:
    CCard*  pPrev;  // �R�D���ɂ���ꍇ�C���̎��̃J�[�h�̃|�C���^
    CCard*  pNext;  // �R�D���ɂ���ꍇ�C���̑O�̃J�[�h�̃|�C���^
    BYTE    n;      // �J�[�h�̎�ށD0-51�D��4 �Ő����C�]�肪�}�[�N�D

public:
    CCard()
    {
        pPrev = NULL;
        pNext = NULL;
        n = 255;
    }

	// �����擾
    inline int GetCardNumber()
    {
        return GET_CARD_NUMBER(n);
    }

	// �}�[�N�擾
    inline int GetCardSymbol()
    {
        return GET_CARD_SYMBOL(n);
    }

	// �e�N�X�`�����UV���W���v�Z
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
// �R�D
//   class CCard �� Chain List �\���ŕێ�
//------------------------------------------------------------
class CCardSuit
{
private:
    CCard   m_HeadCard;     // �R�D�̐擪�̃J�[�h�̑O�ɂ���_�~�[�J�[�h
    CCard   m_TailCard;     // �R�D�̖����̃J�[�h�̎��ɂ���_�~�[�J�[�h
    CCard   m_Cards[52];    // �J�[�h�̎���
    int     m_iCount;       // �R�D�̐�

public:
    CCardSuit()
    {
        Reset();
    }

private:
	// idx �Ԗڂ̃J�[�h���R�D����ꖇ�����D
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
    // ���Z�b�g
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

    // �J�[�h��1������
    CCard* Draw()
    {
        if (m_iCount == 0) return NULL;
        int n = randMT() % m_iCount;
        return RemoveCard(n);
    }

    // �J�[�h��1���R�ɖ߂�
    void   Drop(CCard* pCard)
    {
        pCard->pPrev = m_TailCard.pPrev;
        pCard->pNext = &m_TailCard;
        m_TailCard.pPrev->pNext = pCard;
        m_TailCard.pPrev        = pCard;
        m_iCount++;
    }

    // n �Ŏw�肵���J�[�h�̎��̂��擾
    CCard* GetCard(int n)
    {
        return &(m_Cards[n]);
    }
};


