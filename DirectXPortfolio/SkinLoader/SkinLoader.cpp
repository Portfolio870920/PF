// MeshLoader.cpp: implementation of the CSkinLoader class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "SkinLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinLoader::CSkinLoader()
{
}

CSkinLoader::~CSkinLoader()
{
	Release	();
}

bool	CSkinLoader::Initial				( char* szFileName )
{
	Release ();
	int		nTmp;
	char	szString[1024];
	int		nTexNameLength = 0;
	int		nTexImgLength = 0, i = 0, j = 0;

	FILE*	fp = fopen ( szFileName, "rb" );
	if ( !fp )	return false;

	memset ( szString, 0, 1024 );
	fread	( szString, 3, 1, fp );	// Header. "BSM"
	fread	( &nTexCount, 4, 1, fp );
	fread	( &nMatCount, 4, 1, fp );
	fread	( &nObjCount, 4, 1, fp );
	fread	( &nBoneCount, 4, 1, fp );
	fread	( &nFrameCount, 4, 1, fp );

	if ( nTexCount > 0 )	pTextureSource	= new TEXTURE			[nTexCount];
	if ( nMatCount > 0 )	pMaterialSource	= new SKINMATERIAL		[nMatCount];
	if ( nObjCount > 0 )	pObjectSource	= new SSKINOBJECT		[nObjCount];
	if ( nBoneCount > 0 )	pBoneInfo		= new BONE				[nBoneCount];

	// Texture read.
	for ( i = 0; i < nTexCount; i ++ )
	{
		fread	( &pTextureSource[i].nLength, 4, 1, fp );
		pTextureSource[i].pData = new unsigned char [pTextureSource[i].nLength];
		fread	( pTextureSource[i].pData, pTextureSource[i].nLength, 1, fp );
	}

	// Material read.
	fread	( pMaterialSource, sizeof(SKINMATERIAL), nMatCount, fp );

	// Object read.
	float	w[4];
	for ( i = 0; i < nObjCount; i ++ )
	{
		fread	( &pObjectSource[i].nVCount, 4, 1, fp );
		fread	( &pObjectSource[i].nICount, 4, 1, fp );

		pObjectSource[i].pVertices	= new SCUSTOMSKINVERTEX [pObjectSource[i].nVCount];
		pObjectSource[i].pIndices	= new unsigned short	[pObjectSource[i].nICount];
		fread	( pObjectSource[i].pVertices,	sizeof(SCUSTOMSKINVERTEX),	pObjectSource[i].nVCount, fp );
		fread	( pObjectSource[i].pIndices,	sizeof(unsigned short),		pObjectSource[i].nICount, fp );

		fread	( &pObjectSource[i].uMatIndex,	sizeof(unsigned long),		1, fp );
		for ( int j = 0; j < pObjectSource[i].nVCount; j ++ )
		{
			memcpy ( w, pObjectSource[i].pVertices[j].w, 16 );
			float	l		= w[0] + w[1] + w[2] + w[3];
			float	l1		= __max(0.0f, 1.0f-l)*0.25f;
			pObjectSource[i].pVertices[j].w[0]	+= l1;
			pObjectSource[i].pVertices[j].w[1]	+= l1;
			pObjectSource[i].pVertices[j].w[2]	+= l1;
			pObjectSource[i].pVertices[j].w[3]	+= l1;
		}
	}

	// Animation info read.
	for ( i = 0; i < nBoneCount; i ++ )
	{
		memset ( szString, 0, 1024 );
		fread ( &nTmp, 4, 1, fp );
		fread ( szString, nTmp, 1, fp );	// Name.
		pBoneInfo[i].strName = szString;

		fread ( &pBoneInfo[i].vPivot0, sizeof(float3), 1, fp );
		fread ( pBoneInfo[i].mBasis[0], sizeof(float3), 1, fp );	pBoneInfo[i].mBasis[0][3] = 0.0f;
		fread ( pBoneInfo[i].mBasis[1], sizeof(float3), 1, fp );	pBoneInfo[i].mBasis[1][3] = 0.0f;
		fread ( pBoneInfo[i].mBasis[2], sizeof(float3), 1, fp );	pBoneInfo[i].mBasis[2][3] = 0.0f;
		pBoneInfo[i].mBasis[3][0] = 0.0f;	pBoneInfo[i].mBasis[3][1] = 0.0f;	pBoneInfo[i].mBasis[3][2] = 0.0f;	pBoneInfo[i].mBasis[3][3] = 1.0f;
		fread ( &pBoneInfo[i].nParent, 4, 1, fp);

		pBoneInfo[i].pqRot = new float4 [nFrameCount];
		pBoneInfo[i].pvMov = new float3 [nFrameCount];
		fread	(pBoneInfo[i].pqRot, sizeof(float4), nFrameCount, fp);
		fread	(pBoneInfo[i].pvMov, sizeof(float3), nFrameCount, fp);
	}

	fclose (fp);
	return true;
}

void	CSkinLoader::Release				( void )
{
	DeleteTextureSource	();
	DeleteObjectSource	();
}

void	CSkinLoader::DeleteTextureSource	( void )
{
	if ( !pTextureSource )	return;
	for ( int i = 0; i < nTexCount; i ++ )
	{
		delete [] pTextureSource[i].pData;
		pTextureSource[i].pData = NULL;
	}
	if ( nTexCount == 0 )	return;
	if ( nTexCount == 1 )	delete pTextureSource;
	else					delete [] pTextureSource;
	pTextureSource = NULL;
}

void	CSkinLoader::DeleteObjectSource		( void )
{
	if ( !pObjectSource )	return;
	for ( int i = 0; i < nObjCount; i ++ )
	{
		delete [] pObjectSource[i].pVertices;
		delete [] pObjectSource[i].pIndices;
		pObjectSource[i].pVertices	= NULL;
		pObjectSource[i].pIndices	= NULL;
		pObjectSource[i].uMatIndex	= -1;
	}

	if ( nObjCount == 0 )	return;
	if ( nObjCount == 1 )	delete pObjectSource;
	else					delete [] pObjectSource;
	nObjCount = -1;
	nTexCount = -1;
	pObjectSource = NULL;
}

PSKINMATERIAL		CSkinLoader::GetMaterialSource	( int i )
{
	if ( nMatCount < 1 )	return NULL;
	return &pMaterialSource[i%nMatCount];
}

PBONE				CSkinLoader::GetBoneSource		( int i )
{
	if ( nBoneCount < 1 )	return NULL;
	return &pBoneInfo[i%nBoneCount];
}

unsigned char*		CSkinLoader::GetTextureSource	( int i, int* nLength )
{
	*nLength = 0;
	if ( nTexCount < 1 )	return NULL;
	*nLength		= pTextureSource[i%nTexCount].nLength;
	return (unsigned char*)pTextureSource[i%nTexCount].pData;
}

PSKINOBJECT				CSkinLoader::GetObjectSource	( int i )
{
	if ( nObjCount < 1 )	return NULL;
	return &pObjectSource[i%nObjCount];
}

