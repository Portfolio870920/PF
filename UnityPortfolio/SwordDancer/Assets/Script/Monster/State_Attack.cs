using UnityEngine;
using System.Collections;

public class State_Attack : FSM_State<Slime>
{
    static readonly State_Attack instance          = new State_Attack();
    public static State_Attack  Instance
    {
        get 
        { 
            return instance; 
        }
    }
    private float                AttackTimer       = 0f;

    static State_Attack() { }
    private State_Attack() { }

    public override void EnterState(Slime _Slime)
    {
        // 타겟 유무 확인
        if( _Slime.myTarget == null)
        {
            return;
        }
        AttackTimer = _Slime.AttackSpeed;
    }

    public override void UpdateState(Slime _Slime)
    {
        // 죽음 유무 확인
        if (_Slime.CurrentHP <= 0)
        {
            _Slime.ChangeState(State_Die.Instance);
        }

        AttackTimer += Time.deltaTime;
        if (!_Slime.myTarget.GetComponent<CharactorControl>().IsDead && _Slime.CheckRange() && _Slime.CheckAngle())
        {
            if (AttackTimer >= _Slime.AttackSpeed)
            {
                int Damage = Random.Range(_Slime.SlimeDamage, _Slime.SlimeDamage + _Slime.DamageRandValue);
                _Slime.MGR.SlimeAttack(Damage);
                _Slime.GetComponent<Animation>().CrossFade("Attack", 0.2f);
                AttackTimer = 0.0f;
                _Slime.ChaseTime = 0.0f;
            }
        }
        else
        {
            _Slime.ChangeState(State_Move.Instance);
        }
    }

    public override void ExitState(Slime _Slime)
    {
        Debug.Log("State_Attack 빠져나옴");
    }

}
