using UnityEngine;
using System.Collections;

// 셰이더가 알파값을 변경할 수 없어 적용안됨

public class InvisibleObstacle : MonoBehaviour 
{
    public Transform playerTransform = null;

	// Use this for initialization
	void Start () 
    {
	    
	}
	
	// Update is called once per frame
	void Update () 
    {
        
	}

    void InvisibleObject(ref Collider _col)
    {
        if(_col.gameObject.tag == "Obstacle")
        {
            Color ori_color = _col.gameObject.GetComponent<Renderer>().material.color;
            ori_color.a = 0.1f;
            _col.gameObject.GetComponent<Renderer>().material.color = ori_color;
        }
    }

    void VisibleObject(ref Collider _col)
    {
        if (_col.gameObject.tag == "Obstacle")
        {
            Color ori_color = _col.gameObject.GetComponent<Renderer>().material.color;
            ori_color.a = 1;
            _col.gameObject.GetComponent<Renderer>().material.color = ori_color;
        }
    }


    void OnTriggerEnter(Collider _Other)
    {
        //InvisibleObject(ref _Ohter);
        if (_Other.gameObject.tag == "Obstacle")
        {
            Color ori_color = _Other.gameObject.GetComponent<Renderer>().material.color;
            ori_color.a = 0.1f;
            _Other.gameObject.GetComponent<Renderer>().material.color = ori_color;
        }
    }

    void OnTriggerExit(Collider _Other)
    {
        //VisibleObject(ref _Other);
        if (_Other.gameObject.tag == "Obstacle")
        {
            Color ori_color = _Other.gameObject.GetComponent<Renderer>().material.color;
            ori_color.a = 1;
            _Other.gameObject.GetComponent<Renderer>().material.color = ori_color;
        }
    }
}
