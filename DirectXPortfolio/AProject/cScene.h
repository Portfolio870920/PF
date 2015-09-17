#pragma once

class CScene
{
public:
	HWND HWnd;
	CMesh	m_Mesh;	//�Ž� �ε庯�� ��ī�̹ڽ�

	CScene(void);
	virtual ~CScene(void);

	virtual unsigned int Init(void) =0;
	virtual unsigned int Update(void) =0;
	virtual void Render(void) =0;
	virtual void Free(void) =0;
};
