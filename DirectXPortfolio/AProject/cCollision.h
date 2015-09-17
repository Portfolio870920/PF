#pragma once

class cCollision
{
private:
	float	fBackDist;
	float	fLen;

public:
	cCollision(void);
	~cCollision(void);
	void CollisionPos(D3DXVECTOR3* Player_Pos, D3DXVECTOR3* Enemy_Pos);
	D3DXVECTOR3 CollisionCheck(D3DXVECTOR3 Player_Pos, D3DXVECTOR3 Enemy_Pos, float* fTargetAngle, D3DXVECTOR3 Enemy_Look);

}; 