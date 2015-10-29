abstract public class FSM_State<T>
{
    abstract public void EnterState(T _Slime);

    abstract public void UpdateState(T _Slime);

    abstract public void ExitState(T _Slime);
}
