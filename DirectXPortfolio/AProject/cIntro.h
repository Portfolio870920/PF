#pragma once

class CIntro : public CScene
{
public:
	CIntro();
	~CIntro(void);

	unsigned int Init(void);
	unsigned int Update(void);
	void Render(void);
	void Free(void);

private:
	CUiBase* cUiBase;

	//��Ʈ�α���
	VERTEX2D	*T4thTeam;
	LPDIRECT3DTEXTURE9	pTex4thTeam;
};
