// MeshLoader.h: interface for the CMeshLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHLOADER_H__97536F1E_966C_4EBB_8B4C_206B43A73946__INCLUDED_)
#define AFX_MESHLOADER_H__97536F1E_966C_4EBB_8B4C_206B43A73946__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
struct SCUSTOMVERTEX 
{
	float	x, y, z;
	float	nx, ny, nz;
	float	u, v;
};
typedef	SCUSTOMVERTEX	*PCUSTOMVERTEX;

struct SOBJECT 
{
	std::string		strObjName;
	std::string		strParentName;

	float			pivotX, pivotY, pivotZ;

	int				nVCount, nICount;

	PCUSTOMVERTEX	pVertices;
	unsigned short*	pIndices;
	unsigned short	uTexIndex;
};
typedef	SOBJECT	*POBJECT;

class CMeshLoader
{
protected:
	int				nTexCount;
	int				nObjCount;
	unsigned char**	pTextureSource;
	int*			pTextureSize;
	POBJECT			pObjectSource;

public:
	CMeshLoader();
	virtual ~CMeshLoader();
	bool				Initial				( char* szFileName );
	void				Release				( void );
	void				DeleteTextureSource	( void );
	void				DeleteObjectSource	( void );
	unsigned char*		GetTextureSource	( int i, int* nLength );
	POBJECT				GetObjectSource		( int i );

	__inline	int		GetObjectCount		(void){return nObjCount;}
	__inline	int		GetTextureCount		(void){return nTexCount;}
};

#endif // !defined(AFX_MESHLOADER_H__97536F1E_966C_4EBB_8B4C_206B43A73946__INCLUDED_)
