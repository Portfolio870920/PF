#include "MainHeader.h"

CUiBase::CUiBase(void)
{
}

CUiBase::~CUiBase(void)
{
}

VERTEX2D* CUiBase::CreateVertex2D(float x1, float x2, float y1, float y2, float z)	// 각 인자들의 설명:
{																					// x1	만들 공간의 왼쪽 끝을 정하는 인자
	VERTEX2D	*Name	=	new	VERTEX2D[4];										// x2	만들 공간의 오른쪽 끝을 정하는 인자
	Name[0].x = x1;	Name[0].y =y1;	Name[0].z = z;	Name[0].w = 1.0f;				// y1	만들 공간의 맨 위를 정하는 인자
	Name[1].x = x2;	Name[1].y =y1;	Name[1].z = z;	Name[1].w = 1.0f;				// y2	만들 공간의 맨 아래를 정하는 인자
	Name[2].x = x1;	Name[2].y =y2;	Name[2].z = z;	Name[2].w = 1.0f;				// z	화면의 앞에 위치할지, 뒤에 위치할지 정하는 인자
	Name[3].x = x2;	Name[3].y =y2;	Name[3].z = z;	Name[3].w = 1.0f;				// z = 0.0f(화면의 앞, UI 등), z = 1.0f(화면의 뒤, 배경 등)

	Name[0].u = 0.0f;	Name[0].v = 0.0f;											// u와 v는 내가 텍스쳐를 만들 때에는 만든 공간과 같은 픽셀크기로 만들기 떄문에,
	Name[1].u = 1.0f;	Name[1].v = 0.0f;											// 값을 조정할 의미를 못느낌, 한장만 나오게 고정
	Name[2].u = 0.0f;	Name[2].v = 1.0f;
	Name[3].u = 1.0f;	Name[3].v = 1.0f;
	return Name;
}

void CUiBase::VerTexMove(float x, float y, VERTEX2D *Name)							// 만든 공간의 위치를 넣어준 인자만큼 x,y축으로 이동시킴
{
	Name[0].x += x;	Name[0].y += y;	
	Name[1].x += x;	Name[1].y += y;	
	Name[2].x += x;	Name[2].y += y;	
	Name[3].x += x;	Name[3].y += y;	
}

void CUiBase::VerTexTrans(float x1, float x2, float y1, float y2, VERTEX2D *Name)	// 만든 공간의 행렬안의 x,y값을 변화시켜 결과적으로 공간을 줄이거나 늘릴수 있게 함
{																					// HP바의 오른쪽 끝(x2)을 줄여 HP가 줄어들듯이 보이게 하는 데 쓰임
	Name[0].x += x1;	Name[0].y += y1;	
	Name[1].x += x2;	Name[1].y += y1;	
	Name[2].x += x1;	Name[2].y += y2;	
	Name[3].x += x2;	Name[3].y += y2;	
}

void CUiBase::Render(IDirect3DBaseTexture9 *pTexture, const void *VerTexName)		// 만든 텍스쳐가 들어간 포인터와 만든 2D버텍스 공간의 이름을 인자로 받아
{																					// 화면에 그려냄
	D3DFRAME->GetDevice()->SetFVF ( CUSTOM_VERTEX2D );
	D3DFRAME->GetDevice()->SetTexture ( 0, pTexture );
	D3DFRAME->GetDevice()->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, VerTexName, sizeof(VERTEX2D) );
}
