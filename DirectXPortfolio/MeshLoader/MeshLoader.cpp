// MeshLoader.cpp: implementation of the CMeshLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshLoader::CMeshLoader()
{
}

CMeshLoader::~CMeshLoader()
{
	Release	();
}

bool	CMeshLoader::Initial				( char* szFileName )
{
	Release ();
	float	fTmp;
	int		nTmp;
	char	szString[1024];
	int		nTexNameLength = 0;
	int		nTexImgLength = 0, i = 0, j = 0;

	FILE*	fp = fopen ( szFileName, "rb" );
	if ( !fp )	return false;

	memset ( szString, 0, 1024 );
	fread	( szString, 8, 1, fp );	// Header.
	fread	( &nTexCount, 4, 1, fp );
	fread	( &nObjCount, 4, 1, fp );

	if ( nTexCount > 0 ) pTextureSource	= new unsigned char*	[nTexCount];
	if ( nTexCount > 0 ) pTextureSize	= new int				[nTexCount];
	if ( nObjCount > 0 ) pObjectSource	= new SOBJECT			[nObjCount];

	// Texture read.
	for ( i = 0; i < nTexCount; i ++ )
	{
		fread	( &fTmp, 4, 1, fp );
		fread	( &fTmp, 4, 1, fp );
		fread	( &fTmp, 4, 1, fp );
		fread	( &fTmp, 4, 1, fp );
		fread	( &fTmp, 4, 1, fp );
		fread	( &fTmp, 4, 1, fp );

		fread	( szString, 10, 1, fp );
		fread	( &fTmp, 4, 1, fp );
		fread	( szString, 10, 1, fp );
		fread	( &fTmp, 4, 1, fp );

		fread	( &nTexNameLength, 4, 1, fp );
		fread	( szString, nTexNameLength, 1, fp );
		fread	( &nTmp, 4, 1, fp );
		fread	( &fTmp, 4, 1, fp );

		fread	( &nTexImgLength, 4, 1, fp );
		pTextureSource[i]	= new unsigned char [nTexImgLength];
		pTextureSize[i]		= nTexImgLength;
		fread	( pTextureSource[i], nTexImgLength, 1, fp );
	}

	for ( i = 0; i < nObjCount; i ++ )
	{
		memset ( szString, 0, 1024 );
		fread	( &nTmp, 4, 1, fp );
		fread	( szString, nTmp, 1, fp );
		pObjectSource[i].strObjName = szString;

		memset ( szString, 0, 1024 );
		fread	( &nTmp, 4, 1, fp );
		fread	( szString, nTmp, 1, fp );
		pObjectSource[i].strParentName = szString;
		fread	( &pObjectSource[i].pivotX, 4, 1, fp );
		fread	( &pObjectSource[i].pivotY, 4, 1, fp );
		fread	( &pObjectSource[i].pivotZ, 4, 1, fp );

		fread	( &pObjectSource[i].nVCount, 4, 1, fp );
		fread	( &pObjectSource[i].nICount, 4, 1, fp );

		pObjectSource[i].pVertices	= new SCUSTOMVERTEX [pObjectSource[i].nVCount];
		pObjectSource[i].pIndices	= new unsigned short [pObjectSource[i].nICount];
		for ( j = 0; j < pObjectSource[i].nVCount; j ++ )
		{
			fread ( &pObjectSource[i].pVertices[j].x, 4, 1, fp );
			fread ( &pObjectSource[i].pVertices[j].y, 4, 1, fp );
			fread ( &pObjectSource[i].pVertices[j].z, 4, 1, fp );
		}
		for ( j = 0; j < pObjectSource[i].nVCount; j ++ )
		{
			fread ( &pObjectSource[i].pVertices[j].nx, 4, 1, fp );
			fread ( &pObjectSource[i].pVertices[j].ny, 4, 1, fp );
			fread ( &pObjectSource[i].pVertices[j].nz, 4, 1, fp );
		}
		for ( j = 0; j < pObjectSource[i].nVCount; j ++ )
		{
			fread ( &pObjectSource[i].pVertices[j].u, 4, 1, fp );
			fread ( &pObjectSource[i].pVertices[j].v, 4, 1, fp );
		}
		fread ( pObjectSource[i].pIndices, 2, pObjectSource[i].nICount, fp );
		fread ( &pObjectSource[i].uTexIndex,2, 1, fp );
	}
	fclose (fp);
	return true;
}

void	CMeshLoader::Release				( void )
{
	DeleteTextureSource	();
	DeleteObjectSource	();
}

void	CMeshLoader::DeleteTextureSource	( void )
{
	if ( !pTextureSource )	return;
	for ( int i = 0; i < nTexCount; i ++ )
	{
		delete [] pTextureSource[i];
		pTextureSource[i] = NULL;
	}
	if ( nTexCount == 0 )	return;
	if ( nTexCount == 1 )	delete pTextureSource;
	else					delete [] pTextureSource;
	pTextureSource = NULL;
}

void	CMeshLoader::DeleteObjectSource		( void )
{
	if ( !pObjectSource )	return;
	for ( int i = 0; i < nObjCount; i ++ )
	{
		delete [] pObjectSource[i].pVertices;
		delete [] pObjectSource[i].pIndices;
		pObjectSource[i].pVertices	= NULL;
		pObjectSource[i].pIndices	= NULL;
		pObjectSource[i].uTexIndex	= -1;
	}

	if ( nObjCount == 0 )	return;
	else					delete [] pObjectSource;
	nObjCount = -1;
	nTexCount = -1;
	pObjectSource = NULL;
}

unsigned char*		CMeshLoader::GetTextureSource	( int i, int* nLength )
{
	*nLength = 0;
	if ( nTexCount < 1 )	return NULL;
	*nLength		= pTextureSize[i%nTexCount];
	return pTextureSource[i%nTexCount];
}

POBJECT				CMeshLoader::GetObjectSource	( int i )
{
	if ( nObjCount < 1 )	return NULL;
	return &pObjectSource[i%nObjCount];
}

