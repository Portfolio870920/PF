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


	

	VERTEX2D* CreateVertex2D (float x1, float x2, float y1, float y2, float z);	// x, y, z�� �޾� �� ��ġ�� ������ ����� �Լ�

	void Render(IDirect3DBaseTexture9 *pTexture, const void *VerTexName);		// �ؽ��������Ϳ� ���� ������ �̸��� �޾� ȭ�鿡 �׸��� �Լ�

	void VerTexMove(float x, float y, VERTEX2D *Name);							// ���� ������ x, y������ �����̴� �Լ�

	void VerTexTrans(float x1, float x2, float y1, float y2, VERTEX2D *Name);	// ���� ������ �� �¿���ϸ� ������ ������ ������Ű�� �Լ�

	
	
};
