#include "MainHeader.h"

CUiBase::CUiBase(void)
{
}

CUiBase::~CUiBase(void)
{
}

VERTEX2D* CUiBase::CreateVertex2D(float x1, float x2, float y1, float y2, float z)	// �� ���ڵ��� ����:
{																					// x1	���� ������ ���� ���� ���ϴ� ����
	VERTEX2D	*Name	=	new	VERTEX2D[4];										// x2	���� ������ ������ ���� ���ϴ� ����
	Name[0].x = x1;	Name[0].y =y1;	Name[0].z = z;	Name[0].w = 1.0f;				// y1	���� ������ �� ���� ���ϴ� ����
	Name[1].x = x2;	Name[1].y =y1;	Name[1].z = z;	Name[1].w = 1.0f;				// y2	���� ������ �� �Ʒ��� ���ϴ� ����
	Name[2].x = x1;	Name[2].y =y2;	Name[2].z = z;	Name[2].w = 1.0f;				// z	ȭ���� �տ� ��ġ����, �ڿ� ��ġ���� ���ϴ� ����
	Name[3].x = x2;	Name[3].y =y2;	Name[3].z = z;	Name[3].w = 1.0f;				// z = 0.0f(ȭ���� ��, UI ��), z = 1.0f(ȭ���� ��, ��� ��)

	Name[0].u = 0.0f;	Name[0].v = 0.0f;											// u�� v�� ���� �ؽ��ĸ� ���� ������ ���� ������ ���� �ȼ�ũ��� ����� ������,
	Name[1].u = 1.0f;	Name[1].v = 0.0f;											// ���� ������ �ǹ̸� ������, ���常 ������ ����
	Name[2].u = 0.0f;	Name[2].v = 1.0f;
	Name[3].u = 1.0f;	Name[3].v = 1.0f;
	return Name;
}

void CUiBase::VerTexMove(float x, float y, VERTEX2D *Name)							// ���� ������ ��ġ�� �־��� ���ڸ�ŭ x,y������ �̵���Ŵ
{
	Name[0].x += x;	Name[0].y += y;	
	Name[1].x += x;	Name[1].y += y;	
	Name[2].x += x;	Name[2].y += y;	
	Name[3].x += x;	Name[3].y += y;	
}

void CUiBase::VerTexTrans(float x1, float x2, float y1, float y2, VERTEX2D *Name)	// ���� ������ ��ľ��� x,y���� ��ȭ���� ��������� ������ ���̰ų� �ø��� �ְ� ��
{																					// HP���� ������ ��(x2)�� �ٿ� HP�� �پ����� ���̰� �ϴ� �� ����
	Name[0].x += x1;	Name[0].y += y1;	
	Name[1].x += x2;	Name[1].y += y1;	
	Name[2].x += x1;	Name[2].y += y2;	
	Name[3].x += x2;	Name[3].y += y2;	
}

void CUiBase::Render(IDirect3DBaseTexture9 *pTexture, const void *VerTexName)		// ���� �ؽ��İ� �� �����Ϳ� ���� 2D���ؽ� ������ �̸��� ���ڷ� �޾�
{																					// ȭ�鿡 �׷���
	D3DFRAME->GetDevice()->SetFVF ( CUSTOM_VERTEX2D );
	D3DFRAME->GetDevice()->SetTexture ( 0, pTexture );
	D3DFRAME->GetDevice()->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, VerTexName, sizeof(VERTEX2D) );
}
