using UnityEngine;
using System.Collections;

public class DamageText : MonoBehaviour
{
    public float LifeTime = 30.0f;
    UILabel Label = null;
    Camera UIcam = null;
    Vector3 Target = Vector3.zero;
    
    public void Init(uint _Damage, Vector3 _Target)
    {
        Target = _Target;
        Label = GetComponent<UILabel>();
        Label.text = _Damage.ToString();
        LifeTime = 30.0f;
        if ( UIcam == null)
        {
            Camera guiCam = NGUITools.FindCameraForLayer(gameObject.layer);
        }
    }
    
	void Update ()
    {
	    if( (LifeTime -= Time.deltaTime) > 0)
        {
            Vector3 pos = UIcam.WorldToScreenPoint(Target);
            transform.position = UIcam.ScreenToWorldPoint(new Vector3(pos.x, pos.y + 10.0f, 0));

            Vector3 Position = transform.position;
            Position.y += 10 * Time.deltaTime;
            transform.position = Position;
        }
        else
        {
            gameObject.SetActive(false);
            Init(0,Vector3.zero);
        }
	}
}
