using UnityEngine;
using System.Collections;

public class CharactorMove : MonoBehaviour
{
                    CharactorControl        ChildControl        = null;
                    Vector3                 TempVector          = Vector3.zero;
    public          float                   CameraAngle         = 45.0f;


    void Awake ()
    {
        ChildControl = GetComponentInChildren<CharactorControl>();
        TempVector = transform.forward;
    }

	// Use this for initialization
	void Start () 
    {
	
	}
	
	// Update is called once per frame
	void Update ()
    {
        // 키보드 조작 코드
        //float horizontalkey = Input.GetAxis("Horizontal");
        //float verticalkey = Input.GetAxis("Vertical");

        //playerMove(horizontalkey, verticalkey);
	}

    public void playerMove(/*float h, float v*/ Vector3 _Dir, float _Distance)
    {
        // 키보드 조작 코드
        //// translate
        //transform.position += transform.forward * v * ChildControl.moveSpeed * Time.smoothDeltaTime;
        //// Rotation
        //Quaternion angle = Quaternion.AngleAxis(h * ChildControl.rotationSpeed, transform.up);
        //transform.rotation *= angle;
        ////this.transform.Rotate(transform.up, h * rotationSpeed * Time.smoothDeltaTime);
        //if (h != 0 || v != 0)
        //{
        //    ChildControl.animation.CrossFade("Walk", 0.2f);
        //}
        //else
        //{
        //    ChildControl.onAniEnd();
        //}

        if (ChildControl.CurrentHP <= 0)
        {
            return;
        }

        // Rotation
        if( _Dir != Vector3.zero)
        {
            TempVector = _Dir;
        }
                
        Quaternion angle = Quaternion.LookRotation(TempVector);
        angle *= Quaternion.AngleAxis(CameraAngle, Vector3.up);
        transform.rotation = angle;

        // translate
        transform.position += _Distance * transform.forward * ChildControl.moveSpeed * Time.deltaTime;

        // Animation
        if( _Distance > 0)
        {
            ChildControl.GetComponent<Animation>().CrossFade("Walk", 0.2f);
        }
        else
        {
            ChildControl.GetComponent<Animation>().CrossFade("Wait", 0.2f);
        }
    }
}