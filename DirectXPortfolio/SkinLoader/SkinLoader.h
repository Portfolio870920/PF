// MeshLoader.h: interface for the CSkinLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHLOADER_H__97536F1E_966C_4EBB_8B4C_206B43A73947__INCLUDED_)
#define AFX_MESHLOADER_H__97536F1E_966C_4EBB_8B4C_206B43A73947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
struct SCUSTOMSKINVERTEX
{
	float			x, y, z;
	float			w[4];
	unsigned int	dwWeightIndex;
	float			nx, ny, nz;
	float			u, v;
};
typedef	SCUSTOMSKINVERTEX	*PCUSTOMSKINVERTEX;

struct float3
{
	float	x, y, z;
};
typedef	float3	*Pfloat3;

struct float4
{
	float	x, y, z, w;
};
typedef	float4	*Pfloat4;


struct BONE
{
	std::string			strName;

	float3				vPivot0;
	float				mBasis[4][4];

	int					nParent;

	Pfloat4				pqRot;
	Pfloat3				pvMov;

// 실제 파일에서 불러오지는 않지만 애니메이션시 필요한 인자들.
	float				mInvBasis[4][4];
	float				mWorld[4][4];
	float4				qCurRot;
	float				dmSumRot[4][4];
	float3				vMov;
	float3				vPivot;
	float4				qSumRot;
};
typedef	BONE *PBONE;


struct SSKINOBJECT 
{
	int					nVCount, nICount;
	PCUSTOMSKINVERTEX	pVertices;
	unsigned short*		pIndices;
	unsigned short		uMatIndex;
};
typedef	SSKINOBJECT	*PSKINOBJECT;


struct	TEXTURE
{
	int			nLength;
	void*		pData;
};
typedef	TEXTURE	*PTEXTURE;


struct SKINMATERIAL 
{
	float	a, r, g, b;
	float	Shininess;
	float	Shine_Str;

	int		nTextureIdx[2];
};
typedef SKINMATERIAL	*PSKINMATERIAL;


class CSkinLoader
{
protected:
	int				nMatCount;
	int				nTexCount;
	int				nObjCount;
	int				nBoneCount;
	int				nFrameCount;

	PTEXTURE		pTextureSource;
	PSKINOBJECT		pObjectSource;
	PSKINMATERIAL	pMaterialSource;
	PBONE			pBoneInfo;

public:
	CSkinLoader();
	virtual ~CSkinLoader();
	bool				Initial				( char* szFileName );
	void				Release				( void );
	void				DeleteTextureSource	( void );
	void				DeleteObjectSource	( void );
	PSKINMATERIAL		GetMaterialSource	( int i );
	unsigned char*		GetTextureSource	( int i, int* nLength );
	PSKINOBJECT			GetObjectSource		( int i );
	PBONE				GetBoneSource		( int i );

	__inline	int		GetObjectCount		(void){return nObjCount;}
	__inline	int		GetTextureCount		(void){return nTexCount;}
	__inline	int		GetMaterialCount	(void){return nMatCount;}
	__inline	int		GetBoneCount		(void){return nBoneCount;}
	__inline	int		GetFrameCount		(void){return nFrameCount;}
};

#endif // !defined(AFX_MESHLOADER_H__97536F1E_966C_4EBB_8B4C_206B43A73946__INCLUDED_)
