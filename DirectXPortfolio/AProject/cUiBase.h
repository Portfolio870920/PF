#pragma once

struct	VERTEX2D
{
	float	x, y, z, w;
	float	u, v;
};

class CUiBase
{
public:
	CUiBase(void);
	virtual ~CUiBase(void);


	

	VERTEX2D* CreateVertex2D (float x1, float x2, float y1, float y2, float z);	// x, y, z를 받아 그 위치에 공간을 만드는 함수

	void Render(IDirect3DBaseTexture9 *pTexture, const void *VerTexName);		// 텍스쳐포인터와 만든 공간의 이름을 받아 화면에 그리는 함수

	void VerTexMove(float x, float y, VERTEX2D *Name);							// 만든 공간을 x, y축으로 움직이는 함수

	void VerTexTrans(float x1, float x2, float y1, float y2, VERTEX2D *Name);	// 만든 공간의 각 좌우상하를 움직여 공간을 변형시키는 함수

	
	
};
