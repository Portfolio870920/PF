using UnityEngine;
using System.Collections;

public class State_Die : FSM_State<Slime>
{
    static readonly State_Die instance = new State_Die();
    public static State_Die Instance
    {
        get 
        { 
            return instance; 
        }
    }
    float Count = 2f;
    float time = 0f;

    static State_Die() { }
    private State_Die() { }

    public override void EnterState(Slime _Slime)
    {
        _Slime.GetComponent<Animation>().CrossFade("Dead");
        _Slime.IsDead = true;
        _Slime.CreateItem(Random.Range(0, 3), Random.Range(0, 4));
    }

    public override void UpdateState(Slime _Slime)
    {
        //time += Time.deltaTime;
        //_Slime.AlphaChange(time / Count, Count);
        //color = _Slime.renderer.material.color;
        //color.a *= time / Count;
        //if( color.a > Count)
        //    color.a = Count;
        //_Slime.renderer.material.color = color;
        //if( _Slime.isActiveAndEnabled && time >= Count)
        //{
            _Slime.gameObject.SetActive(false);
        //    time = 0f;
        //}
    }

    public override void ExitState(Slime _Slime)
    {

    }
}
