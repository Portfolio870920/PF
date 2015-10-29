using UnityEngine;
using System.Collections;

public class Profile : MonoBehaviour {


	// Use this for initialization
	void Start () 
    {
	
	}
	
	// Update is called once per frame
	void Update () 
    {
	
	}

    void OnClick()
    {
        GameObject.Find("UI Root/UICam/UIPanel/Status").SetActive(true);
    }
}
