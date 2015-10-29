using UnityEngine;
using System.Collections;

public class MainCamera : MonoBehaviour {

    public          GameObject      Player;
    public          float           RotateX             = 45f;
    float                           OriRotateX          = 0.0f;
    public          float           RotateY             = 45f;
    public          float           Currentdistance     = 10;
                    float           OriDistance         = 0.0f;
                    Transform       TempTrans           = null;
    

	// Use this for initialization
	void Start () 
    {
        OriDistance = Currentdistance;
        OriRotateX = RotateX;
        transform.position = new Vector3(0, 0, 0);
        TempTrans = transform;
        Reset(RotateX, RotateY);
	}
	
	// Update is called once per frame
	void Update () 
    {
        transform.position = Player.transform.position + (-TempTrans.forward) * Currentdistance;
	}

    public void Reset(float _RotateX, float _RotateY)
    {
        TempTrans = transform;
        TempTrans.rotation = Quaternion.identity;
        TempTrans.Rotate(_RotateX, _RotateY, 0.0f);
    }

    public float GetOriDistance()
    {
        return OriDistance;
    }

    public float GetOriRotate()
    {
        return OriRotateX;
    }
}
