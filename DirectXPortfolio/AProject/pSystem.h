#ifndef __pSystemH__
#define __pSystemH__

#include "D3Dx9.h"

#include <list>
#include <string>
#include <limits>

const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

struct BoundingBox
{
	BoundingBox();

	bool isPointInside(D3DXVECTOR3& p);

	D3DXVECTOR3 _min;
	D3DXVECTOR3 _max;
};

struct BoundingSphere
{
	BoundingSphere();

	D3DXVECTOR3 _center;
	float       _radius;
};

//��ƼŬ���� ������ �ʿ��ϹǷ� �Լ�2�� �߰�
float GetRandomFloat(float lowBound, float highBound);
void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);
//float�� DWORD�� ����ȯ, SetRenderState�� float�� �ƴ� DWORD�� �μ��� �޾Ƽ� �ʿ�
DWORD FtoDw(float f);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ƼŬ�� ��ġ�� �÷��� ǥ���ϴµ��� ������ ���� ���ؽ� ����ü�� �̿��
//��ƼŬ ������ ũ����� �����Ϸ��� �׷���ī�尡 D3DFVFCAPS_PSIZE�� �����ǰų� ���ؽ� ���̴��� �̿��ؾ��Ѵ�.
struct Particle
{
	D3DXVECTOR3 _position;
	D3DCOLOR    _color;
};
#define CUSTOM_PARTICLE		D3DFVF_XYZ | D3DFVF_DIFFUSE

//��ƼŬ �Ӽ�
//�ΰ����� �Ӽ����� ��ƼŬ�������ϴµ� �ʼ����� ���� �ƴϹǷ�
//�ʿ��� �����Ϳ� ��ƼŬ �Ӽ��� ������ ����ü ���� �����Ѵ�.
//��ƼŬ�� ����� �ı��ϰ� �����Ҷ��� �� �Ӽ��� �̿��� �۾��ϸ� 
//�������� �غ� �Ϸ�Ǹ� Particle ����ü�� ��ġ�� �Ӽ��� ������ �̿�
struct Attribute
{
	Attribute()
	{
		_lifeTime = 0.0f;
		_age      = 0.0f;
		_isAlive  = true;
	}

	//��ƼŬ �Ӽ��� �𵨸��Ϸ��� ��ƼŬ �ý��� ������ ���� �޶�������
	//�ణ�� �Ϲ�ȭ�� ��ġ�� �� ���� �������� �Ӽ��� ������ �� �ִ�.
	//��κ��� �ý��ۿ����� �̵� �Ӽ��� ��� �̿����� ������ �Ϻ� �ý���
	//������ ���⿡ ���Ե��� ���� �ΰ����� �Ӽ��� �ʿ�� �Ѵ�.

	D3DXVECTOR3 _position;		// ���� �����̽� ���� ��ƼŬ ��ġ
	D3DXVECTOR3 _velocity;		// ��ƼŬ �ӵ�, �ʴ� �̵������� �����
	D3DXVECTOR3 _acceleration;	// ��ƼŬ ����, �ʴ� �̵������� �����
	float       _lifeTime;		// ��ƼŬ�� �Ҹ��Ҷ����� �����Ǵ� �ð�  
	float       _age;			// ��ƼŬ ���� ����  
	D3DXCOLOR   _color;			// ��ƼŬ �÷�   
	D3DXCOLOR   _colorFade;		// ��ƼŬ �÷��� �ð��� �帧�� ���� ����ϴ� ���
	bool        _isAlive;		// ��ƼŬ�� ������ ��� True �Ҹ��� ��� False
};

//��ƼŬ �ý���
//��ƼŬ���� �����̸� ��ƼŬ�� �����ְ� �����ϴ� ������ ���
//��ƼŬ ���Ű� ���÷���, �Ҹ�, ���� ���� �����ϴ� ����
class PSystem
{
public:

	PSystem();
	virtual ~PSystem();

	virtual bool init(IDirect3DDevice9* device, char* texFileName);
	virtual void reset();
	
	//��ƼŬ�� �Ӽ��� ���µǴ� ����� ��ƼŬ �ý��ۿ� ���� �޶����Ƿ� �ڽ�Ŭ�������� �Լ��� �����ϵ��� ���������Լ��� ����
	virtual void resetParticle(Attribute* attribute) = 0;
	virtual void addParticle();

	//�ý��� ���� ��� ��ƼŬ���� ����. Ư�� ��ƼŬ �ý��ۿ� ���� �޶����Ƿ� �ڽ�Ŭ�������� �����ϵ��� ���������Լ��� ����
	virtual void update(float timeDelta) = 0;
	//�߰��Լ�
	virtual void update() = 0;

	virtual void preRender();
	virtual void postRender();

	virtual void render();

	bool isEmpty();
	bool isDead();

	//�߰��Լ�
	bool isCollision(D3DXVECTOR3 vPos);
	void addParticles(int numParticles);
	void SerialParticles(PSystem** pParticle1, PSystem** pParticle2);

protected:
	virtual void removeDeadParticles();

protected:
	IDirect3DDevice9*       _device;
	D3DXVECTOR3	            _origin;		//�ý��� ������ ��ƼŬ�� ���۵Ǵ� ��
	BoundingBox				_boundingBox;	//�����ڴ� ��ƼŬ�� �̵��� �� �ִ� ���Ǹ� �����ϴµ� �̿�� 
											//�����ڷ� �����ϸ� �� ������ ��� ��ƼŬ���� ��ٷ� �Ҹ�.
	float                   _emitRate;		//�ý��ۿ� ���ο� ��ƼŬ�� �߰��Ǵ� ����. �ʴ� ��ƼŬ ���� ���.
	float                   _size;			//�ý��� �� ��� ��ƼŬ�� ũ��
	IDirect3DTexture9*      _tex;			//�ؽ�ó
	IDirect3DVertexBuffer9* _vb;			//���ؽ�����
	std::list<Attribute>    _particles;		//��ƼŬ�� ����� �����ϰ� �����ϴµ� �� ����Ʈ�� �̿���.
	int                     _maxParticles;	//�־��� �ð� ���� �ý����� ���� �� �ִ� �ִ� ��ƼŬ�� ��.

	//
	//���� 3���� �Ӽ��� ��ƼŬ�ý��� ȿ�������� �������ϱ� ����
	//

	DWORD _vbSize;      //���ؽ� ���۰� �ѹ��� ������ �� �ִ� ��ƼŬ�� ��. 
						//�� ���� ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
	DWORD _vbOffset;    //���ؽ����ۿ��� ���縦 ������ ��ƼŬ �� ���� �ܰ���� ������(����Ʈ�� �ƴ� ��ƼŬ����) 
	DWORD _vbBatchSize; //�ϳ��� �ܰ迡 ���ǵ� ��ƼŬ�� ��
};

///////////////////////////////////////////////////////////////////////////////////////////////////

//�Ҳ�
class Fire : public PSystem
{
public:
	Fire(D3DXVECTOR3* origin, int numParticles);
	//���������Լ� ����
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
	//�����Լ� ������
	void preRender();
	void postRender();
protected:
	//me����ϰ� ���ͷ� ������ �ݺ��� �ʵǼ� �����͸� ���� �߰�
	D3DXVECTOR3* _porigin;		
};

//������
class Gun : public PSystem
{
public:
	Gun(D3DXVECTOR3* vPos, D3DXVECTOR3* vDir);
	//���������Լ� ����
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
	//�߰��Լ�
	void Gun::preRender();
	void Gun::postRender();

private:
	D3DXVECTOR3* _vPos;
	D3DXVECTOR3* _vDir;
};

//����Ʈ
class blast : public PSystem
{
public:
	blast(D3DXVECTOR3* origin, int numParticles);
	//���������Լ� ����
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
	//�����Լ� ������
	void preRender();
	void postRender();
};

//����
class buff : public PSystem
{
public:
	buff(D3DXVECTOR3* origin, int numParticles);

	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();

	void preRender();
	void postRender();
protected:
	//me����ϰ� ���ͷ� ������ �ݺ��� �ʵǼ� �����͸� ���� �߰�
	D3DXVECTOR3* _porigin;		
};

//��
class Snow : public PSystem
{
public:
	Snow(BoundingBox* boundingBox, int numParticles);
	//���������Լ� ����
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
	void update();
};


#endif // __pSystemH__