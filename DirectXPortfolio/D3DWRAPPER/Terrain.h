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

	LPDIRECT3DVERTEXBUFFER9			*m_pvBUFFER;	// ū ������ 1���� �ƴ϶� ���� ũ�⸸ŭ �߶󳻹Ƿ� VertexBuffer�� �������� �ȴ�.
	LPDIRECT3DINDEXBUFFER9			*m_piBUFFER;	// LOD ó���� ���� LOD������ŭ �ε��� ���۰� �����ȴ�.
	int								*m_piBUFFERLen;	// ������ LOD�� ������ �ε����� ������ �پ���. ����ص� ������ �����صδ� ����� �������.
	LPD3DXVECTOR3					m_piCenterPos;	// LOD�� �ٰŰ� �� Cell�� ��ġ.
	int								m_CellSize;		// ��ü ���� �����ŭ�� ũ��� �ڸ����� ũ��. �Ѱ��� ������ 64X64�� ���ڷ� �̷�����ٴ� �ǹ�.
	int								m_CellCnt;		// CellSize���� ũ��� ������ ����� ������ ��� �������� �����ϴ� ����. �׸���/CellSize�� �ǹ̰� ����.
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