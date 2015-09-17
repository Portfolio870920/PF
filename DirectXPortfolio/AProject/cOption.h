#pragma once

class cOption
{
private:
	cOption(void);
	static	cOption*	OptionInst;

	CUiBase* cUiBase;
	CGame*	 cGame;
	HWND	 m_hwnd;
	POINT ptMouse;
	//옵션
	VERTEX2D	*Black;
	LPDIRECT3DTEXTURE9	pTexBlack;

	VERTEX2D	*OptionBG;
	LPDIRECT3DTEXTURE9	pTexOptionBG;

	LPDIRECT3DTEXTURE9	pTexReTry;		//버튼들의 텍스쳐
	LPDIRECT3DTEXTURE9	pTexShowControl;
	LPDIRECT3DTEXTURE9	pTexMainMenu;

	VERTEX2D	*Controls;
	LPDIRECT3DTEXTURE9	pTexControls;
	LPDIRECT3DTEXTURE9	pTexComBoSystem;
	LPDIRECT3DTEXTURE9	pTexBuffSkills;

	int Select;
	int Click;
	int Select2;


public:
	~cOption(void);
	static	cOption*	GetInst(void);
	void	FreeInst(void);

	unsigned int Init(HWND hwnd);
	unsigned int Update(void);
	void Render(void);
};
