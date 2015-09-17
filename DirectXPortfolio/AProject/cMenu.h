#pragma once

class CMenu	: public CScene
{
public:
	CMenu();
	~CMenu(void);

	unsigned int Init(void);
	unsigned int Update(void);
	void Render(void);
	void Free(void);

private:
	CUiBase* cUiBase;
	int Select;	//� ��ư�� �������
	int Click;	//� ��ư�� ��������
	POINT ptMouse;	//���콺��ġ ���� ����

	BOOL				m_bIsStop;	// ������Ʈ���¸� ���ߴ��� �ȸ��ߴ��� üũ


	//����
	VERTEX2D	*Title;
	LPDIRECT3DTEXTURE9	pTexTitle;

	//��ŸƮ��ư
	VERTEX2D	*Start;
	LPDIRECT3DTEXTURE9	pTexStart;
	LPDIRECT3DTEXTURE9	pTexRStart;

	//�ɼǹ�ư
	VERTEX2D	*Option;
	LPDIRECT3DTEXTURE9	pTexOption;
	LPDIRECT3DTEXTURE9	pTexROption;

	//�����ư
	VERTEX2D	*Exit;
	LPDIRECT3DTEXTURE9	pTexExit;
	LPDIRECT3DTEXTURE9	pTexRExit;

	//�ε�
	VERTEX2D	*Loading;
	LPDIRECT3DTEXTURE9	pTexLoading;
};
