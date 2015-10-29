using System;
using System.Collections.Generic;
using UnityEngine;

public class StateMachine <T>
{
    private T Owner;
    private FSM_State<T> CurrentState;
    private FSM_State<T> PreviousState;

    // 변수 초기화
    public void Awake()
    {
        CurrentState = null;
        PreviousState = null;
    }

    // 상태 변경
    public void ChangeState(FSM_State<T> _NewState)
    {
        // 같은 상태를 변환하려 한다면 나감
        if(_NewState == CurrentState)
        {
            return;
        }

        PreviousState = CurrentState;
        
        // 현재 상태가 있다면 종료
        if( CurrentState != null)
        {
            CurrentState.ExitState(Owner);
        }

        CurrentState = _NewState;

        // 새로 적용된 상태가 null이 아니면 실행
        if( CurrentState != null)
        {
            CurrentState.EnterState(Owner);
        }
    }

    // 초기상태설정
    public void Initial_Setting(T _Owner, FSM_State<T> _InitialState)
    {
        Owner = _Owner;
        ChangeState(_InitialState);
    }

    // 상태 업데이트
	public void Update ()
    {
        //if(GlobalState != null)
        //{
        //    GlobalState.UpdateState(Owner);
        //}
        if(CurrentState != null)
        {
            CurrentState.UpdateState(Owner);
        }
	}

    // 이전 상태로 회귀
    public void StateRevert()
    {
        if(PreviousState != null)
        {
            ChangeState(PreviousState);
        }
    }
}
