//#include "pSystem.h"

#include <cstdlib>

#include "MainHeader.h"

BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = FLT_MAX;
	_min.y = FLT_MAX;
	_min.z = FLT_MAX;

	_max.x = -FLT_MAX;
	_max.y = -FLT_MAX;
	_max.z = -FLT_MAX;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if( p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z )
	{
		return true;
	}
	else
	{
		return false;
	}
}

BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

//��ƼŬ���� ������ �ʿ��ϹǷ� �Լ�2�� �߰�
//�����Լ� srand()�� �̿��� �����߻����� ���� �����ϴ� ������ ����������
//[lowBound,highBound]������ �ִ� ������ float�� ����
float GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) //�߸��� �Է�
		return lowBound;

	//[0,1]������ ���� float�� ��´�.
	float f = (rand() % 10000) * 0.0001f; 

	//[lowBound,highBound]������ float�� �����Ѵ�. 
	return (f * (highBound - lowBound)) + lowBound; 
}
//�ּ��� min�� �ִ��� max�� ���ǵ� ���� ���� ���� ���͸� ����Ѵ�.
void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

//float�� DWORD�� ����ȯ, SetRenderState�� float�� �ƴ� DWORD�� �μ��� �޾Ƽ� �ʿ�
DWORD FtoDw(float f)
{
	return *((DWORD*)&f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//����Ʈ ���� �ʱ�ȭ�ϴ� ������
PSystem::PSystem()
{
	_device = 0;
	_vb     = 0;
	_tex    = 0;
}
//���ؽ����� �ؽ�ó�� �����ϴ� �ı���
PSystem::~PSystem()
{
	_vb->Release();
	_tex->Release();
}
//����Ʈ ��������Ʈ�� �����ϱ� ���� ���ؽ� ���۸� ����� �ؽ�ó�� ����� ���� �ʱ�ȭ �۾��� ó��
bool PSystem::init(IDirect3DDevice9* device, char* texFileName)
{
	_device = device; // save a ptr to the device

	HRESULT hr = 0;
	
	//���� ���ؽ� ����
	//�� �����Ӹ��� ��ƼŬ�� �����ؾ� �ϸ� �̴� �� ���ؽ� ������ �޸𸮿� �����ؾ� ���� �ǹ� 	
	// D3DPOOL_MANAGED�� D3DUSAGE_DYNAMIC�� �Բ� ����� ������. 
	//���ؽ����۰� ����Ʈ ��������Ʈ�� ������ ������ �����ϴ� D3DUSAGE_POINTS�� �̿������� Ȯ��
	//���ؽ�����ũ�Ⱑ _vbSize�� ���� �̸� ���ǵǸ� �ý��� ���� ��ƼŬ ���ʹ� ������ ������ ����
	//�̴� ��ƼŬ �ý����� �������� �ѹ��� ������� �ʰ� �ܰ������� �̷������ ����
	hr = device->CreateVertexBuffer(_vbSize * sizeof(Particle), 
									D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 
									CUSTOM_PARTICLE, 
									D3DPOOL_DEFAULT, 
									&_vb, 
									0);
	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
		return false;
	}

	//�ؽ�ó
	hr = D3DXCreateTextureFromFile(device, texFileName, &_tex);

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateTextureFromFile() - FAILED", "PSystem", 0);
		return false;
	}

	return true;
}

//�ý��� ���� ��� ��ƼŬ �Ӽ��� ����
void PSystem::reset()
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		//��ƼŬ�� �Ӽ��� ���µǴ� ����� ��ƼŬ �ý��ۿ� ���� �޶����Ƿ� 
		//�ڽ�Ŭ�������� �Լ��� �����ϵ��� ���������Լ��� ����
		resetParticle( &(*i) );
	}
}

//�ý��ۿ� ��ƼŬ�� �߰�. ����Ʈ�� �߰��ϱ� ���� ��ƼŬ�� �ʱ�ȭ �ϴµ� resetParticle()�� �̿�
void PSystem::addParticle()
{
	Attribute attribute;

	resetParticle(&attribute);

	_particles.push_back(attribute);
}

//����Ʈ ��������Ʈ�� ���۵��� ���� ��κ� �������¿� ���� ����ȴ�.
//�ʱ� �������¸� ����. �ý��ۿ� ���� �޶��� �� �����Ƿ� �����Լ��� ����.
void PSystem::preRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, false);
	
	//true	���������� ��ü �ؽ�ó�� ����Ʈ ��������Ʈ�� �ؽ�ó ���ο� �̿��� ������ �ǹ�
	//false	����Ʈ ��������Ʈ�� �ؽ�ó ��ǥ�� ������ �ؼ��� �̿��� ����Ʈ ��������Ʈ�� �ؽ�ó�� ������ �ϵ��� ����
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	
	//true	����Ʈ ��������Ʈ ũ��� ī�޶���� �Ÿ��� ���� �����ϰ� ������
	//false	����Ʈ ũ�⸦ ȭ����� �ȼ� ������ �ؼ��ϵ��� ����
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	
	//����Ʈ ��������Ʈ ũ�⸦ ����
	_device->SetRenderState(D3DRS_POINTSIZE, FtoDw(_size));
	
	//����Ʈ ��������Ʈ �ּ� ũ�� ����
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	//����Ʈ ��������Ʈ �ִ� ũ�� ����
	//_device->SetRenderState(D3DRS_POINTSIZE_MAX, �ִ�ũ��);

	//�� �� ���� ����� (ī�޶�� ����Ʈ ��������Ʈ)�Ÿ��� ���� ����Ʈ ��������Ʈ�� ũ�Ⱑ ���ϴ� ����� ����
	//���� �ڵ�� ����Ʈ ��������Ʈ�� �۾������� ����Ʈ ��������Ʈ �Ÿ� ��� ���� ������
	_device->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));
		
	//�ؽ�ó�� ���ĸ� �̿�.
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//���ĺ����� Ȱ��ȭ�Ͽ� �ؽ�ó�� ���� ä���� �ؽ�ó�� �ȼ� ������ �����ϵ��� ����.
	//���� ��� ����� ������ �������� ����ä���� ���� ��� �ؽ�ó�� �̿��ϸ� ��� �� ����� ��ƼŬ�� ���� �� ����.
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
//	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//�߰�
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

//���� ���¸� �����ϴµ� �̿�. �ý��ۿ� ���� �޶��� �� �����Ƿ� �����Լ��� ����.
void PSystem::postRender()
{
	_device->SetRenderState(D3DRS_LIGHTING,          true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
	//�߰�
	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void PSystem::render()
{
	if( !_particles.empty() )
	{
		//�������¸� �����Ѵ�.
		preRender();
		
		_device->SetTexture(0, _tex);
		_device->SetFVF(CUSTOM_PARTICLE);
		_device->SetStreamSource(0, _vb, 0, sizeof(Particle));

		//���ؽ����۸� ��� ��� ó������ �����Ѵ�.
		if(_vbOffset >= _vbSize) _vbOffset = 0;

		Particle* v = 0;

		_vb->Lock(	_vbOffset * sizeof( Particle ), 
					_vbBatchSize * sizeof( Particle ), 
					(void**)&v, 
					_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		//��� ��ƼŬ�� �������ɶ�����
		std::list<Attribute>::iterator i;
		for(i = _particles.begin(); i != _particles.end(); i++)
		{
			if( i->_isAlive )
			{
				//�Ѵܰ��� ������ ��ƼŬ�� ���� ���ؽ����� ���׸�Ʈ�� �����Ѵ�.
				v->_position = i->_position;
				v->_color    = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; //�ܰ�ī���͸� ������Ų��.

				//����ܰ谡 ��� ä���� �ִ°�?
				if(numParticlesInBatch == _vbBatchSize) 
				{
					//���ؽ����۷� ����� �������ܰ��� ��ƼŬ���� �׸���.
					_vb->Unlock();

					_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, _vbBatchSize);

					//�ܰ谡 �׷����� ���� �����ܰ踦 ��ƼŬ�� ä���.

					//�����ܰ��� ó�� ���������� �̵��Ѵ�.
					_vbOffset += _vbBatchSize; 

					//���ؽ������� ��踦 �Ѵ� �޸𸮷� �������� �������� �ʴ´�.
					//��踦 ���� ��� ó������ �����Ѵ�.
					if(_vbOffset >= _vbSize) _vbOffset = 0;       

					_vb->Lock(	_vbOffset * sizeof( Particle ), 
								_vbBatchSize * sizeof( Particle ), 
								(void**)&v, 
								_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; //�����ܰ踦 ���� ����
				}	
			}
		}

		_vb->Unlock();

		//������ �������� ���Ͽ� �������ܰ谡 �����������ʴ� ��찡 �߻��� �� �ִ�.
		//�Ϻθ� ä���� �ܰ�� �ٷ� �̰����� �������ȴ�.
		if( numParticlesInBatch )
		{
			_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, numParticlesInBatch);
		}

		//���� ���
		_vbOffset += _vbBatchSize; 

		//�������¸� �����Ѵ�.
		postRender();
	}
}

//���� �ý��ۿ� ��ƼŬ�� ���� ��� True�� �����ϸ� �׷��� ������ False�� ����.
bool PSystem::isEmpty()
{
	return _particles.empty();
}

//�ý��� ���� ��ƼŬ�� ��� ���� ��� True�� �����ϸ� �׷��� ������ False�� ����.
//�� ����		�ý��� ���� ��ƼŬ�� ���� ���� 
//���� ����		��ƼŬ�� ���������� ���� ������ ǥ�õ� ����
bool PSystem::isDead()
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		//�ּ� �ϳ��� ��ƼŬ�̶� �����ִٸ� ���� �ʾ����� ����.
		if( i->_isAlive )
			return false;
	}
	//�ϳ��� ��ƼŬ�� ���ٸ� �׾����� ����.
	return true;
}

//�Ӽ�����Ʈ _particle�� �˻��Ͽ� ���� ��ƼŬ�� ����Ʈ���� ����
void PSystem::removeDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = _particles.begin();

	while( i != _particles.end() )
	{
		if( i->_isAlive == false )
		{
			// erase�� ���� �ݺ��ڸ� �����ϹǷ� �츮�� �ݺ��ڸ� ������ų �ʿ䰡 ����.
			i = _particles.erase(i); 
		}
		else
		{
			i++; //����� ��� ���� ��ƼŬ
		}
	}
}

//removeDeadParticles()
//�Ϻ� ��ƼŬ �ý��ۿ����� ���� ��ƼŬ�� �����ϱ⺸�ٴ� ��Ȱ���ϴ� ���� ȿ������ �� �ִ�.
//��, ��ƼŬ�� ��������� ���ŵɶ� �Ҵ�� ������ �ݺ��ϴ� ���� �ƴ϶� �����ϰ� ���� ��ƼŬ�� �����Ͽ� ���ο� ��ƼŬ�� �̿�

//�߰��Լ�
//�浹�����Լ�
bool PSystem::isCollision(D3DXVECTOR3 vPos)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//������ ��ƼŬ�� �浹����
		if( i->_isAlive) { 
			if ((vPos.x >= i->_position.x - 10.0f && vPos.x <= i->_position.x + 10.0f) && 
				(vPos.z >= i->_position.z - 10.0f && vPos.z <= i->_position.z + 10.0f)) return true;
		}
	}
}

//�߰��Լ�
//�ݺ������� �ƴѰ�� Ű�� �߰����� �Լ�
//������ ��ƼŬ�� ���� �����ϰ� �Ϸ��� �̷������� �ֵ���ƼŬ�� �ݺ������� ������.
void PSystem::addParticles(int numParticles)
{
	for(int i = 0; i < numParticles; i++) addParticle();
}

//�߰��Լ�
//��ƼŬ�� �������� ���������� �ٸ� ��ƼŬ�� �׸��� �Լ�
void PSystem::SerialParticles(PSystem** pParticle1, PSystem** pParticle2)
{
	std::list<Attribute>::iterator i;

	for ( i = _particles.begin(); i != _particles.end(); i++ ) {
		if( i->_isAlive) { 

			//(*pParticle1)->_origin.y = Terrain->GetHeight(i->_position.x, i->_position.y);
			
			if ( i->_position.y <= MAPMGR->GetHeight(i->_position.x, i->_position.z) + 0.3f
				&& i->_position.y >= MAPMGR->GetHeight( i->_position.x, i->_position.z ) - 0.3f ) {
				(*pParticle1)->_origin	= i->_position;
				(*pParticle1)->_origin.y = i->_position.y;	
				(*pParticle1)->addParticles(20);

				//(*pParticle2)->_origin = i->_position;
				//(*pParticle2)->_origin.y = 35.0f;
				//(*pParticle2)->addParticles(70);
				break;
			}
		}
	}

	//�����ͷ� ���� ��ƼŬ�� �����ǿ� ������ƼŬ �������� ���Խ�Ų�� �� ��ġ�� y��ǥ�� 0�� �Ǹ� ������ƼŬ�� ��Ʈ����.
}

//�Ҳ�//�ּ��Ϸ�////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//�ý��� ��õ������ �����Ϳ� �ý����� ���� ��ƼŬ�� ���� ����
//�ý����� ��õ�̶� �Ҳ��� ������ ��Ҹ� �ǹ�
Fire::Fire(D3DXVECTOR3* origin, int numParticles)
{
	_porigin        = origin;
	_size          = 5.0f;
	_vbSize        = 2048;
	_vbOffset      = 0;   
	_vbBatchSize   = 512; 

	//�������ڸ��� �׷�����. �ּ�ó���ϸ� �����.
	//for(int i = 0; i < numParticles; i++) addParticle();
}

//���������Լ� ����
//�ý��� ��õ�� ��ƼŬ�� �ʱ�ȭ�ϰ� ��ü ������ ������ �ӵ��� �����,
//�ý��� ���� ��ƼŬ���� ������ �÷��� �ο��޴´�.
//���������� �� ��ƼŬ���� 2�� ���� �����ϵ��� ������ �����Ѵ�.
void Fire::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;
	attribute->_position = *_porigin;

	//y�� ����
	//attribute->_position.y += 5.0f;  

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f,  1.0f,  1.0f);

	GetRandomVector(&attribute->_velocity, &min, &max);
	
	//��ü�� ����� ���� ����ȭ
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);

	attribute->_velocity *= 5.0f;

	//attribute->_color = D3DXCOLOR(GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	attribute->_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f;
	attribute->_lifeTime = 5.0f;
}

//���������Լ� ����
//�� ��ƼŬ�� ��ġ�� �����ϰ� ������ �ʰ��� ��ƼŬ�� ������ ó���Ѵ�.
//�� �ý����� ���� ��ƼŬ�� ���������� �ʴ´ٴ� �� ��������.
//�̰��� ���ο� �Ҳ��� ���� �� ������ ���� Fire�ý����� ��Ȱ���� �� �ֱ� ����.
void Fire::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//������ ��ƼŬ�� ����
		if( i->_isAlive ) {
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;
			
			//���δ�.
			if(i->_age > i->_lifeTime) i->_isAlive = false;
		}
	}
	//�߰��ϸ� �ݺ����� �ʴ´ٱ�
	removeDeadParticles();
}

void Fire::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	if ( isDead() ) reset();

	lastTime = currTime;
}


//�����Լ� ������
//�ٸ� ���� �μ��� �̿�ȴ�. ���� ���� ���۷��� ���⵵ ������ �ʴ´�.
//����Ʈ �����μ��� ���� ���θ� �����ϰ��� �Ѵٸ� �������� ����Ʈ������ �������̵��ϸ� �ȴ�.
//�� �޼��� ��� �θ�޼��带 ȣ���ϰ� ������ Ȯ������.
void Fire::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z���� �б�� ���������� ����� ������� �ʴ´�.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

//�����Լ� ������
void Fire::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//������//�ּ��Ϸ�///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ī�޶���� �����͸� �޴´�. 
//���ο� ��ƼŬ�� ���鶧���� ī�޶��� ��ġ�� ���⿡ ���� ������ �ʿ��ϱ� ����
//meĳ������ ��ġ�� ������ �޵��� ����
Gun::Gun(D3DXVECTOR3* vPos, D3DXVECTOR3* vDir)
{
	_vPos			= vPos;
	_vDir			= vDir;

	_size           = 20.0f;
	_vbSize         = 2048;
	_vbOffset       = 0;  
	_vbBatchSize    = 512; 
}

//���������Լ� ����
//��ƼŬ�� ��ġ�� ī�޶��� ���� ��ġ�� �����ϰ� ��ƼŬ�� �ӵ���
//ī�޶� �ٶ󺸰� �ִ� ������ 100��� �����Ѵ�.
void Gun::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;

	//��ƼŬ ��ġ�� ī�޶� ��ġ�� �̿��Ѵ�.
	attribute->_position	=	*_vPos;
	//meĳ���Ϳ��� �� �������� �߻�ǵ���  
	attribute->_position	+=	*_vDir * 10.0f;//* 280.0f;
	//ī�޶󺸴� �ణ �Ʒ������� �̵��� ���� ��� �ִ� �� ���� ȿ���� ��´�.
	//attribute->_position.y -= 1.0f; 
	//attribute->_position.y +=	500.0f;

	
	//�¿���Ϸ� �Ѹ���
	_vDir->x += GetRandomFloat(-0.05f, 0.05f);
	_vDir->y += GetRandomFloat(-0.05f, 0.05f);
	
	//ī�޶� �ٶ󺸴� �������� �߻��Ѵ�.
	//me�ѹ߿��� �������� ������ �������� ����
	attribute->_velocity = *_vDir /* * 200.0f;*/ * GetRandomFloat(0.0f, 1.0f) * 300.0f;

	attribute->_color =   D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f; 
	attribute->_lifeTime = 1.5f; //������ 1��
}

//���������Լ� ����
//��ƼŬ�� ��ġ�� �����ϰ� ������ ���� ��� ���� ������ ó��
//�ڿ��� ��ƼŬ ����Ʈ�� �˻��Ͽ� ���� ��ƼŬ�� �����Ѵ�.
void Gun::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * timeDelta;

		i->_age += timeDelta;

		if(i->_age > i->_lifeTime)	//������ ���ϸ� ���δ�.
			i->_isAlive = false;
	}
	removeDeadParticles();			//���� ��ƼŬ�� ����.
}

void Gun::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	lastTime = currTime;
}


//�Լ��߰�
void Gun::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z���� �б�� ���������� ����� ������� �ʴ´�.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

//�Լ��߰�
void Gun::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//����Ʈ//�ݺ��Ҵ�////////////////////////////////////////////////////////////////////////////////////////////////////////////
blast::blast(D3DXVECTOR3* origin, int numParticles)
{
	_origin        = *origin;
	_size          = 50.0f;
	_vbSize        = 2048;
	_vbOffset      = 0;   
	_vbBatchSize   = 512; 

	//�������ڸ��� �׷�����. �ּ�ó���ϸ� �����.
	//for(int i = 0; i < numParticles; i++) addParticle();
}

void blast::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;
	attribute->_position = _origin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f,  0.0f,  1.0f);

	GetRandomVector(&attribute->_velocity, &min, &max);

	//��ü�� ����� ���� ����ȭ
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);

	attribute->_velocity *= 70.0f;

	//attribute->_color = D3DXCOLOR(GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	attribute->_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f;
	attribute->_lifeTime = 1.0f;
}

void blast::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//������ ��ƼŬ�� ����
		if( i->_isAlive ) {
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			//���δ�.
			if(i->_age > i->_lifeTime) i->_isAlive = false;
		}
	}
	//�߰��ϸ� �ݺ����� �ʴ´ٱ�
	removeDeadParticles();
}

void blast::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	if ( isDead() ) reset();

	lastTime = currTime;
}


void blast::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z���� �б�� ���������� ����� ������� �ʴ´�.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void blast::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//����//_porigin////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�̻��ϰ� _origin�� �����Ͱ� �Ǹ� &D3DXVECTOR3()�� �μ��� ������ �ݺ��� �ʵȴ�... ����? 
buff::buff(D3DXVECTOR3* origin, int numParticles)
{
	_porigin        = origin;
	_size          = 5.0f;
	_vbSize        = 2048;
	_vbOffset      = 0;   
	_vbBatchSize   = 512; 

	for(int i = 0; i < numParticles; i++) addParticle();
}

void buff::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;
	attribute->_position = *_porigin;

	attribute->_position.y += 5.0f;  

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 0.0f, 1.0f);

	GetRandomVector(&attribute->_velocity, &min, &max);

	//��ü�� ����� ���� ����ȭ
	D3DXVec3Normalize(&attribute->_velocity, &attribute->_velocity);

	attribute->_velocity *= 30.0f;

	//attribute->_color = D3DXCOLOR(GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	attribute->_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	attribute->_age      = 0.0f;
	attribute->_lifeTime = 0.7f;
}

void buff::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++) {
		//������ ��ƼŬ�� ����
		if( i->_isAlive ) {
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			//���δ�.
			if(i->_age > i->_lifeTime) i->_isAlive = false;
		}
	}
	//�߰��ϸ� �ݺ����� �ʴ´ٱ�
	//removeDeadParticles();
}

void buff::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime) * 0.001f;

	update(timeDelta);

	if ( isDead() ) reset();
	
	lastTime = currTime;
}

void buff::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//z���� �б�� ���������� ����� ������� �ʴ´�.
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void buff::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

//��//�ּ��Ϸ�////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//������ ����ü�� ����Ű�� �����Ϳ� �ý��� �� ��ƼŬ�� ���� ����
//�����ڴ� �����̰� ������ ���Ǹ� �����ϸ� ���� �����̰� �̹��� ������
//����� ��� �Ҹ��ϰ� �ٽ� ���������.
Snow::Snow(BoundingBox* boundingBox, int numParticles)
{
	_boundingBox   = *boundingBox;
	_size          = 7.0f;
	_vbSize        = 2048;
	_vbOffset      = 0; 
	_vbBatchSize   = 512; 
	
	for(int i = 0; i < numParticles; i++) addParticle();
}

//���������Լ� ����
//������ �� ������ x z��ǥ ��ġ���� �����̸� ����� y��ǥ�� �������� �ֻ�ܰ� ������ ����.
//�̾� �����̿� �ӵ��� �ο��Ͽ� �Ʒ� �������� �׸��� �ణ �������� ���������� �Ѵ�.
void Snow::resetParticle(Attribute* attribute)
{
	attribute->_isAlive  = true;

	//�������� ��ġ ������ ���� ������ x, z ��ǥ�� ��´�.
	GetRandomVector(&attribute->_position, &_boundingBox._min, &_boundingBox._max);

	//y��ǥ�� �������� ���� �ʰ� �׻� �������� �ֻ���� �ȴ�.
	attribute->_position.y = _boundingBox._max.y; 

	//�����̴� �Ʒ������� �������� �ణ ������ ���Ѵ�.
	attribute->_velocity.x = GetRandomFloat(0.0f, 1.0f) * -10.0f/*0.0f*/;
	attribute->_velocity.y = GetRandomFloat(0.0f, 1.0f) * -50.0f/*GetRandomFloat(0.8f, 1.0f) * -100.0f*/;
	attribute->_velocity.z = 0.0f;

	//����� ������
	attribute->_color = WHITE;
}

//���������Լ� ����
//��ƼŬ�� ��ġ�� �����ϸ� �ý����� �����ڸ� ������� Ȯ���Ѵ�. 
//���� �����ڸ� ����ٸ� �ش� ��ƼŬ�� �ʱ�ȭ�Ѵ�.
void Snow::update(float timeDelta)
{
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * timeDelta;

		//����Ʈ�� ��踦 ����°�?
		if( _boundingBox.isPointInside( i->_position ) == false ) 
		{
			//��踦 ��� ��ƼŬ�� ��Ȱ���Ѵ�.
			resetParticle( &(*i) );
		}
	}
}

void  Snow::update()
{
	static float lastTime = (float)timeGetTime(); 

	float currTime  = (float)timeGetTime();
	float timeDelta = (currTime - lastTime)*0.001f;

	update(timeDelta);

	lastTime = currTime;
}
