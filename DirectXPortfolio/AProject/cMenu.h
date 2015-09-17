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
	int Select;	//어떤 버튼를 골랐는지
	int Click;	//어떤 버튼을 눌렀는지
	POINT ptMouse;	//마우스위치 들어가는 변수

	BOOL				m_bIsStop;	// 업데이트상태를 멈추는지 안멈추는지 체크


	//제목
	VERTEX2D	*Title;
	LPDIRECT3DTEXTURE9	pTexTitle;

	//스타트버튼
	VERTEX2D	*Start;
	LPDIRECT3DTEXTURE9	pTexStart;
	LPDIRECT3DTEXTURE9	pTexRStart;

	//옵션버튼
	VERTEX2D	*Option;
	LPDIRECT3DTEXTURE9	pTexOption;
	LPDIRECT3DTEXTURE9	pTexROption;

	//종료버튼
	VERTEX2D	*Exit;
	LPDIRECT3DTEXTURE9	pTexExit;
	LPDIRECT3DTEXTURE9	pTexRExit;

	//로딩
	VERTEX2D	*Loading;
	LPDIRECT3DTEXTURE9	pTexLoading;
};
