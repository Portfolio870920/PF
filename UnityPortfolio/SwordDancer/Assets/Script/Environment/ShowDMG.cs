using UnityEngine;
using System.Collections;

public class ShowDMG : MonoBehaviour {
    float a = 0;
    int b = 1;
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
        HUDText test = GetComponent<HUDText>();
        test.Add(Time.deltaTime * 10f, Color.red, 0f);
        
        a = 1f * Mathf.Sin(b++);
        transform.position = new Vector3(0, 3, a);
	}
}
