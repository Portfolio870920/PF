using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class CreateLine : MonoBehaviour
{
    struct MyLine
    {
        public Vector3 StartPos;
        public Vector3 EndPos;
    }

    public Camera UICam = null;
    public GameObject LinePrefab = null;
    public GameObject LineParent = null;
    private GameObject TempObject = null;
    private LineRenderer Lines = null;
    private List<Vector3> LinePoints = new List<Vector3>();
    private List<Vector2> ColliderPoints = new List<Vector2>();
    private EdgeCollider2D Edges = null;
    private Vector2[] ColliderVertexList = null;
    private Vector3 MousePos = Vector3.zero;
    private bool isMousePressed = false;
    private Rigidbody2D rigidbody = null;
    private float Smooth = 0.5f;

    Vector3 GetMousePosition()
    {
        MousePos = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        MousePos = transform.worldToLocalMatrix * MousePos;
        MousePos.z = 0;
        return MousePos;
    }

    void OnDrag()
    {
        if(isMousePressed)
        {
            GetMousePosition();
            AddVertex();
        }
    }

    //void Update()
    //{
    //    //Debug.Log("x : " + UICam.ScreenToWorldPoint(Input.mousePosition).x + ", x2 : " + Camera.main.ScreenToWorldPoint(Input.mousePosition).x
    //    //    + "y : " + UICam.ScreenToWorldPoint(Input.mousePosition).y + ", y2 : " + Camera.main.ScreenToWorldPoint(Input.mousePosition).y);
    //    //Debug.Log("center x : " + rigidbody.centerOfMass.x + ", center y : " + rigidbody.centerOfMass.y);
    //}

    public int VertexCount
    {
        get
        {
            return LinePoints.Count;
        }
    }

    void OnPress(bool _OnOff)
    {
        if(_OnOff)
        {
            if (ColliderVertexList != null)
            {
                ColliderVertexList.Initialize();
            }
            if (LinePoints.Count > 0)
            {
                LinePoints.Clear();
            }
            if (ColliderPoints.Count > 0)
            {
                ColliderPoints.Clear();
            }
            if (rigidbody != null)
            {
                rigidbody = null;
            }
            TempObject = NGUITools.AddChild(LineParent, LinePrefab);
            Lines = TempObject.GetComponent<LineRenderer>();
            Edges = TempObject.GetComponent<EdgeCollider2D>();
            rigidbody = TempObject.GetComponent<Rigidbody2D>();
            Lines.SetWidth(0.01f, 0.01f);
            Vector3 tempvector = Camera.main.ScreenToWorldPoint(Input.mousePosition);
            tempvector = transform.worldToLocalMatrix * tempvector;
            tempvector.z = 0;
            TempObject.transform.position = tempvector;
            isMousePressed = true;
        }
        else
        {
            isMousePressed = false;
            rigidbody.gravityScale = 0.1f;
            //ChangeCenterofMass();
        }
    }

    void ChangeCenterofMass()
    {
        float centerX = 0, centerY = 0;
        float area = 0;

        Vector2 CenterPoint = Vector2.zero;
        int firstIndex, secondIndex, sizeOfVertexs = LinePoints.Count;

        Vector2 firstPoint;
        Vector2 secondPoint;

        float factor = 0;

        for (firstIndex = 0; firstIndex < sizeOfVertexs; firstIndex++)
        {
            secondIndex = (firstIndex + 1) % sizeOfVertexs;

            firstPoint = ColliderPoints[firstIndex];
            secondPoint = ColliderPoints[secondIndex];

            factor = ((firstPoint.x * secondPoint.y) - (secondPoint.x * firstPoint.y));

            area += factor;

            centerX += (firstPoint.x + secondPoint.x) * factor;
            centerY += (firstPoint.y + secondPoint.y) * factor;
        }

        area /= 2.0f;
        area *= 6.0f;

        factor = 1 / area;

        centerX *= factor;
        centerY *= factor;

        rigidbody.centerOfMass = new Vector2(centerX, centerY);
    }
    public void AddVertex()
    {
        if(!LinePoints.Contains(MousePos))
        {
            rigidbody.mass += 0.3f;
            LinePoints.Add(MousePos);
            Lines.SetVertexCount(LinePoints.Count);
            Vector3 TempPos;

            // 곡선 부드럽게
            //if( LinePoints.Count >= 2)
            //{
            //    TempPos = new Vector3((LinePoints[LinePoints.Count - 2].x + LinePoints[LinePoints.Count - 1].x) * Smooth,
            //                         (LinePoints[LinePoints.Count - 2].y + LinePoints[LinePoints.Count - 1].y) * Smooth,
            //                         (LinePoints[LinePoints.Count - 2].z + LinePoints[LinePoints.Count - 1].z) * Smooth);
            //}

            //else
            //{
            TempPos = LinePoints[LinePoints.Count - 1];
            //}

            Lines.SetPosition(LinePoints.Count - 1, TempPos);

            // 엣지콜라이더 설정
            if( Edges != null)
            {
                ColliderPoints.Add(new Vector2(TempPos.x, TempPos.y));

                ColliderVertexList = new Vector2[ColliderPoints.Count];

                for( int i = 0; i < ColliderPoints.Count; i++)
                {
                    ColliderVertexList[i] = ColliderPoints[i];
                }                
                if( ColliderVertexList.Length >= 2)
                {
                    Edges.points = ColliderVertexList;
                }
            }
        }
    }

}
