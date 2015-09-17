#pragma once
#include	"D3DWRAPPER.h"

struct STERRAINVERTEX
{
	float		x,y,z;
	float		nx,ny,nz;
	D3DXFLOAT16	yt[8];
};

class CTerrain
{

	LPDIRECT3DVERTEXBUFFER9			*m_pvBUFFER;	// 큰 데이터 1개가 아니라 일정 크기만큼 잘라내므로 VertexBuffer는 여러개가 된다.
	LPDIRECT3DINDEXBUFFER9			*m_piBUFFER;	// LOD 처리를 위해 LOD갯수만큼 인덱스 버퍼가 생성된다.
	int								*m_piBUFFERLen;	// 저수준 LOD로 갈수록 인덱스의 갯수는 줄어든다. 계산해도 되지만 저장해두는 방법을 사용하자.
	LPD3DXVECTOR3					m_piCenterPos;	// LOD의 근거가 될 Cell의 위치.
	int								m_CellSize;		// 전체 맵을 어느만큼의 크기로 자를지의 크기. 한개의 조각은 64X64의 격자로 이루어진다는 의미.
	int								m_CellCnt;		// CellSize만한 크기로 조각을 낼경우 한줄이 몇개로 나뉠지를 저장하는 변수. 그림폭/CellSize와 의미가 같다.
	int								m_IndexCnt;
	int								m_nWidth, m_nDepth;

	LPDIRECT3DTEXTURE9				m_pNormalMap;
	LPDIRECT3DTEXTURE9				m_pPerturbation;
	LPDIRECT3DTEXTURE9				m_pChannel;
	LPDIRECT3DTEXTURE9				m_pChannelA;
	LPDIRECT3DTEXTURE9				m_pChannelR;
	LPDIRECT3DTEXTURE9				m_pChannelG;
	LPDIRECT3DTEXTURE9				m_pChannelB;

	void							CreateNormalMap	( void );
	CTerrain(void);
	static CTerrain*	Map_Inst;

public:
	virtual ~CTerrain(void);
	static CTerrain* GetInst();

	static		LPD3DXEFFECT					s_pEffect;
	static		LPDIRECT3DVERTEXDECLARATION9	s_pVDecl;
	static		void			LoadEffect	( const char* szFileName );

	void			CreatePerturbationTexture	( const char* szFileName );
	void			CreateChannelTexture		( const char* szFileName );
	void			CreateTextureUsingA			( const char* szFileName );
	void			CreateTextureUsingR			( const char* szFileName );
	void			CreateTextureUsingG			( const char* szFileName );
	void			CreateTextureUsingB			( const char* szFileName );

	unsigned	int	Init	( const char* szFileName );
	void		FreeInst	( void );
	void		Render		( void );
	float		GetHeight	( float x, float z, LPD3DXVECTOR3 vCell = NULL, LPD3DXVECTOR3 vNormal = NULL );
};