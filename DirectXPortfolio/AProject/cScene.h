#pragma once

class CScene
{
public:
	HWND HWnd;
	CMesh	m_Mesh;	//매쉬 로드변수 스카이박스

	CScene(void);
	virtual ~CScene(void);

	virtual unsigned int Init(void) =0;
	virtual unsigned int Update(void) =0;
	virtual void Render(void) =0;
	virtual void Free(void) =0;
};
