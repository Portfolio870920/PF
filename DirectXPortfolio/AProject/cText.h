#pragma once

class cText
{
private:
	static	cText*		TextInst;
			ID3DXFont*	m_pFont;
			int			m_nMax_X;
			int			m_nMax_Y;
	cText(void);

public:	
	~cText(void);
	void	Init(void);
	void    Print( LPCSTR cSTR, int nX/*���ڿ��� ������ǥ*/ = 0, int nY/*���ڿ��� ������ǥ*/ = 0, D3DXCOLOR ARGB = 0xFFFFFFFF );
	static	cText*		GetInst(void);
			void		FreeInst(void);
};

// ����
//TCHAR	str_Text[256];	// ���ڿ� ����
//ZeroMemory(str_Text,sizeof(str_Text)); //���ڿ� ���� �ʱ�ȭ
////sprintf(������,���ķδ� printfó�� ���);
//sprintf(str_Text,"��� : %d",A0); // �̷������� ���
//TEXTMGR->Print(str_Text); // �ؽ�Ʈ���
//sprintf(str_Text,"��� : %d",A1); // �̷������� ���
//TEXTMGR->Print(str_Text); // �ؽ�Ʈ���
//sprintf(str_Text,"��� : %d",A2); // �̷������� ���
//TEXTMGR->Print(str_Text); // �ؽ�Ʈ���
//sprintf(str_Text,"��� : %d",A3); // �̷������� ���
//TEXTMGR->Print(str_Text); // �ؽ�Ʈ���
//�Ѱ� ������ ����Ŀ� ���� ����
//dx D3DFRAME->GetDevice()->BeginScene();�� D3DFRAME->GetDevice()->EndScene(); ���̿� ������