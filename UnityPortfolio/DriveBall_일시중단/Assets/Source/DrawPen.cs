using UnityEngine;
using System.Collections;

public class DrawPen : MonoBehaviour {

    public GameObject Pen = null;

    public GameObject DrawStart = null;

    public Camera UICam = null;

    public GameObject PenPrefab = null;

    public GameObject Dragprefab = null;

    GameObject DragTemp = null;

    Rigidbody2D PenRigidbody = null;

    bool Can_i_draw = true;

    void OnPress()
    {
        if(Can_i_draw)
        {
            DrawStart = NGUITools.AddChild(Pen, PenPrefab);
            DrawStart.transform.position = UICam.ScreenToWorldPoint(new Vector3(UICamera.currentTouch.pos.x, UICamera.currentTouch.pos.y, 0));
            PenRigidbody = DrawStart.GetComponent<Rigidbody2D>();
            PenRigidbody.gravityScale = 0;
            Can_i_draw = false;
        }
        else
        {
            Can_i_draw = true;
            DrawStart.GetComponent<Rigidbody2D>().gravityScale = 1;
        }
    }
    void OnDrag(Vector2 delta)
    {
        DragTemp = NGUITools.AddChild(DrawStart, Dragprefab);
        DragTemp.transform.position = UICam.ScreenToWorldPoint(new Vector3(UICamera.currentTouch.pos.x, UICamera.currentTouch.pos.y, 0));
        PenRigidbody.mass += 1;
    }

    void OnDrop(GameObject drag)
    {
        PenRigidbody.gravityScale = 1;
    }
}
