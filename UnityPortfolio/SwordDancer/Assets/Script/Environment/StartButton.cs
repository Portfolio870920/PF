using UnityEngine;
using System.Collections;

public class StartButton : MonoBehaviour {
    Camera          UICam           = null;

	void Awake () 
    {
        UICam = GameObject.Find("UI Root/UICamera").GetComponent<Camera>();
        
        // button collider resize
        GetComponent<BoxCollider2D>().size = GetComponent<UISprite>().localSize;
	}
	
	// Update is called once per frame
	void Update () 
    {
        Ray ray = UICam.ScreenPointToRay(Input.mousePosition);
        RaycastHit2D rayhit = Physics2D.GetRayIntersection(ray, Mathf.Infinity);
        if ( Input.GetKeyDown(KeyCode.Mouse0)&& rayhit.collider == this.GetComponent<Collider2D>())
        {
            Application.LoadLevel("PlayScene");
        }
	}

    // Ngui doesn't work
    //void OnClick()
    //{
    //    Application.LoadLevel("PlayScene");
    //}
}
