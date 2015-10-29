using UnityEngine;
using System;
using System.Collections;

public class State_Move : FSM_State<Slime>
{
    static      readonly State_Move         instance        = new State_Move();
    public      static State_Move           Instance
    {
        get 
        { 
            return instance; 
        }
    }

    private     float                       ResetTime       = 3f;
    private     float                       CurrenntTime;

    static State_Move() { }
    private State_Move() { }

    public override void EnterState(Slime _Slime)
    {
        CurrenntTime = ResetTime;
    }

    public override void UpdateState(Slime _Slime)
    {
        // 죽음 유무 확인
        if (_Slime.CurrentHP <= 0)
        {
            _Slime.ChangeState(State_Die.Instance);
        }

        // 타겟 유무 확인
        if(_Slime.myTarget != null)
        {
            if(!_Slime.CheckRange())
            {
                // 추적시간을 초과하면 타겟을 잃음
                _Slime.ChaseTime += Time.deltaTime;
                if (_Slime.ChaseTime >= _Slime.ChaseCancleTime)
                {
                    _Slime.myTarget = null;
                    _Slime.ChaseTime = 0.0f;
                    return;
                }

                RaycastHit RayInfo;
                if ( Physics.Raycast(_Slime.transform.position, _Slime.transform.forward, out RayInfo, _Slime.AttackRange) && RayInfo.collider.tag == "Obstacle")
                {
                    Debug.Log(_Slime.name + "이 길찾기를 수행합니다.");
                    _Slime.navi.SetDestination(_Slime.myTarget.position);
                }
                else
                {
                    // 회전각 구하기
                    Vector3 Dir = _Slime.myTarget.position - _Slime.transform.position;
                    Vector3 NorDir = Dir.normalized;

                    Quaternion angle = Quaternion.LookRotation(NorDir);

                    // 회전
                    _Slime.transform.rotation = angle;

                    // 이동
                    Vector3 Pos = _Slime.transform.position;
                    Pos += _Slime.transform.forward * Time.smoothDeltaTime * _Slime.MoveSpeed;
                    _Slime.transform.position = Pos;
                }
            }
            else
            {
                _Slime.ChangeState(State_Attack.Instance);
            }
              
        }
        else
        {
            // 타겟이 없을 때는 임의의 방향으로 방황함
            // 방향 재설정
            SetRandDir(_Slime);
            //#if DEBUG
            // 몬스터의 진행방향 표시
            // 시각적으로 알기 쉽게 y좌표만 높여줌
            Vector3 endPoint = _Slime.transform.position + (_Slime.transform.forward * 2f);
            endPoint.y += 1f;
            Debug.DrawLine(_Slime.transform.position, endPoint, Color.red);
            //Debug.Log("x : " + _Slime.transform.forward.x + ", y : " + _Slime.transform.forward.y + ", z : " + _Slime.transform.forward.z);
            //#endif
            endPoint.y = 0;
            Vector3 pos = _Slime.transform.position;
            pos += _Slime.transform.forward * Time.smoothDeltaTime * (_Slime.MoveSpeed / 3f);
            _Slime.transform.position = pos;
        }
        _Slime.Ani.CrossFade("Walk");
    }

    public override void ExitState(Slime _Slime)
    {
#if DEBUG
        Debug.Log("State_Move를 종료합니다.");
#endif
    }

    // ResetTime 때 마다 임의의 방향으로 설정
    void SetRandDir(Slime _Slime)
    {
        CurrenntTime += Time.smoothDeltaTime;
        if (CurrenntTime >= ResetTime)
        {
            _Slime.transform.forward = Quaternion.AngleAxis(UnityEngine.Random.Range(0, 360f), Vector3.up) * Vector3.forward;
            // 시간 재설정
            ResetTime = UnityEngine.Random.Range(1f, 4f);
            CurrenntTime = 0f;
        }
    }
}
