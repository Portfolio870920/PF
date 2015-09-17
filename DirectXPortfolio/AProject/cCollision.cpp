#include "MainHeader.h"

cCollision::cCollision(void)
{
}

cCollision::~cCollision(void)
{
}

void cCollision::CollisionPos(D3DXVECTOR3* Player_Pos, D3DXVECTOR3* Enemy_Pos)
{
	fLen	=	D3DXVec3Length(&(*Player_Pos - *Enemy_Pos));
	if ( fLen < 50.0f )
	{
		fBackDist	=	50 - fLen;
		D3DXVECTOR3	vEnemyToMyDir;
		D3DXVec3Normalize( &vEnemyToMyDir, &(*Player_Pos - *Enemy_Pos));
		*Player_Pos = *Player_Pos + vEnemyToMyDir * fBackDist;
	}
}

D3DXVECTOR3 cCollision::CollisionCheck(D3DXVECTOR3 Player_Pos, D3DXVECTOR3 Enemy_Pos, float* fTargetAngle, D3DXVECTOR3 Enemy_Look)
{
	D3DXVECTOR3	vMonster_dir, vCross, New_EnemyPos;
	float fLen;
	
	vMonster_dir = ( Player_Pos - Enemy_Pos );			//	���� �÷��̾ �ٶ󺸴� ����
	D3DXVec3Normalize( &vMonster_dir, &vMonster_dir );	//	�� ���� ����ȭ
	D3DXVec3Normalize( &Enemy_Look, &Enemy_Look);		//	���� ���� ����ȭ
	fLen	=	D3DXVec3Length( &(Player_Pos - Enemy_Pos) );	//	�÷��̾�� ������ �Ÿ�
	New_EnemyPos	=	Enemy_Pos;						//	���� ���� ��ġ
	if( fLen < 50.0f )		//	����
	{
		D3DXVec3Cross( &vCross, &vMonster_dir, &Enemy_Look);	//	���� �÷��̾� �ٶ󺸴� ����� ���� �������� ����
		//D3DXVec3Normalize(&vCross, &vCross );
		if( vCross.y < 0.99f )		//	�þ� ��
		{
			*fTargetAngle	-=	vCross.y*0.045f;
			if(fLen > 18.0f)		//	���� ����
			{
				New_EnemyPos.x += ( vMonster_dir.x * 0.5f );
				New_EnemyPos.z += ( vMonster_dir.z * 0.5f );
			}
			return New_EnemyPos;
		}
	}
	return Enemy_Pos;
}