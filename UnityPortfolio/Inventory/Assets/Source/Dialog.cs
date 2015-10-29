using UnityEngine;
using System.Collections;

public class Dialog : MonoBehaviour
{
    private Animator my_animator = null;

    void Start()
    {
        my_animator = gameObject.GetComponent<Animator>();
    }

    public void closewindow()
    {
        my_animator.SetBool("Isclose", true);
    }

    public void disable()
    {
        gameObject.SetActive(false);
    }
}
