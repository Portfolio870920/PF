using UnityEngine;
using System.Collections;
using System.Collections.Generic;
//[RequireComponent(typeof(LineRenderer))]
public class DrawLine : MonoBehaviour
{
    public bool selfUpdate = false;
    public bool checkSameLineCollisionOnSelfUpdate = false;

    public Camera cam = null;
    public GameObject LinePrefab = null;
    public GameObject lineRenderer = null;
    private GameObject TempObject = null;
    private LineRenderer line = null;
    private bool isMousePressed = false;    
    private List<Vector3> pointsList = new List<Vector3>();
    private List<Vector2> colliderList = new List<Vector2>();
    private Vector3 mousePos = Vector3.zero;
    private EdgeCollider2D _collider = null;
    Vector2[] _colliderVertexPositions = null;
    float smooth = .5f;
    

    // Structure for line points
    struct myLine
    {
        public Vector3 StartPoint;
        public Vector3 EndPoint;
    };

    public int VertexCount
    {
        get
        {
            return pointsList.Count;
        }
    }

    void Awake()
    {
        //line = gameObject.GetComponent<LineRenderer>();
        //line.SetVertexCount(0);

        //if (GetComponent<EdgeCollider2D>() != null)
        //{
        //    _collider = GetComponent<EdgeCollider2D>();
        //}
        //else
        //{
        //    _collider = gameObject.AddComponent<EdgeCollider2D>();
        //}
        //_collider.enabled = true;
    }
    //    -----------------------------------    
    //void Update()
    //{
    //    //Debug.Log(Camera.main.ScreenToWorldPoint(Input.mousePosition).x + ", " + Camera.main.ScreenToWorldPoint(Input.mousePosition).y);
    //    if (selfUpdate)
    //    {
    //        // If mouse button down, remove old line and set its color to green
    //        if (Input.GetMouseButtonDown(0))
    //        {
    //            // 라인렌더러 생성
    //            if(line == null)
    //            {
    //                //line = null;
    //                //TempObject = null;
    //                //_collider = null;
    //                //colliderList.Clear();
    //                //pointsList.Clear();
    //                //_colliderVertexPositions = null;
    //                TempObject = NGUITools.AddChild(lineRenderer, LinePrefab);
    //                line = TempObject.GetComponent<LineRenderer>();
    //                line.SetWidth(0.01f, 0.01f);
    //                _collider = TempObject.GetComponent<EdgeCollider2D>();
    //            }

    //            isMousePressed = true;
    //            line.SetVertexCount(0);
    //            pointsList.RemoveRange(0, pointsList.Count);
    //            //line.SetColors(Color.green, Color.green);
    //        }
    //        else if (Input.GetMouseButtonUp(0))
    //        {
    //            stopLine();
    //        }
    //        // Drawing line when mouse is moving(presses)
    //        if (isMousePressed)
    //        {
    //            mousePos = Camera.main.ScreenToWorldPoint(Input.mousePosition);//cam.ScreenToWorldPoint(Input.mousePosition);
    //            mousePos.z = 0;
    //            addVertex(mousePos, true);
    //            //if (isLineCollide() && checkSameLineCollisionOnSelfUpdate)
    //            //{
    //            //    stopLine();
    //            //}
    //        }
    //    }
    //}

    void OnDrag()
    {
        if (isMousePressed)
        {
            mousePos = Camera.main.ScreenToWorldPoint(Input.mousePosition);//cam.ScreenToWorldPoint(Input.mousePosition);
            mousePos.z = 0;
            addVertex(mousePos, true);
        }
    }

    void OnPress(bool _OnOff)
    {
        if (_OnOff)
        {
            // 라인렌더러 생성
            //if (line == null)
            //{
            TempObject = NGUITools.AddChild(lineRenderer, LinePrefab);
            line = TempObject.GetComponent<LineRenderer>();
            line.SetWidth(0.01f, 0.01f);
            _collider = TempObject.GetComponent<EdgeCollider2D>();
            //}

            isMousePressed = true;
            line.SetVertexCount(0);
            pointsList.RemoveRange(0, pointsList.Count);
            //line.SetColors(Color.green, Color.green);
        }
        else if (!_OnOff)
        {
            stopLine();
            line = null;
            TempObject = null;
            _collider = null;
            colliderList.Clear();
            pointsList.Clear();
            _colliderVertexPositions = null;
        }
    }

    public void stopLine()
    {
        isMousePressed = false;
        //TempObject.GetComponent<Rigidbody2D>().gravityScale = 1;        
    }
    public void addVertex(Vector3 pos, bool isCollidable)
    {
        if (!pointsList.Contains(mousePos))
        {
            pointsList.Add(pos);
            line.SetVertexCount(pointsList.Count);
            Vector3 p;
            if (pointsList.Count >= 2)
            {
                p = new Vector3((pointsList[pointsList.Count - 2].x + pointsList[pointsList.Count - 1].x) * smooth,
                                (pointsList[pointsList.Count - 2].y + pointsList[pointsList.Count - 1].y) * smooth,
                                (pointsList[pointsList.Count - 2].z + pointsList[pointsList.Count - 1].z) * smooth);
            }
            else
                p = pointsList[pointsList.Count - 1];
            line.SetPosition(pointsList.Count - 1, p);
            // If collidable also set vertex positions
            if (isCollidable && _collider != null)
            {
                //colliderList.Add(new Vector2(pos.x + .3f, pos.y + .9f));
                colliderList.Add(new Vector2(pos.x * 225f, pos.y * 225f));

                _colliderVertexPositions = new Vector2[colliderList.Count];
                for (int i = 0; i < colliderList.Count; i++)
                {
                    _colliderVertexPositions[i] = colliderList[i];
                }
                if (_colliderVertexPositions.Length >= 2)
                    _collider.points = _colliderVertexPositions;
            }
        }
    }
    /// <summary>
    /// called when drawing completed for opening edge collider
    /// </summary>
    public void onDrawLine()
    {
        if (_collider != null)
            _collider.enabled = true;
    }
    /// <summary>
    /// Resets line
    /// </summary>
    public void reset()
    {
        line.SetVertexCount(0);
        pointsList = new List<Vector3>();
        if (_collider != null)
        {
            colliderList = new List<Vector2>();
            _collider.Reset();
            _collider.enabled = false;
        }
    }

    #region PRIVATES
    //    -----------------------------------    
    //  Following method checks is currentLine(line drawn by last two points) collided with line
    //    -----------------------------------    
    private bool isLineCollide()
    {
        if (pointsList.Count < 2)
            return false;
        int TotalLines = pointsList.Count - 1;
        myLine[] lines = new myLine[TotalLines];
        if (TotalLines > 1)
        {
            for (int i = 0; i < TotalLines; i++)
            {
                lines[i].StartPoint = (Vector3)pointsList[i];
                lines[i].EndPoint = (Vector3)pointsList[i + 1];
            }
        }
        for (int i = 0; i < TotalLines - 1; i++)
        {
            myLine currentLine;
            currentLine.StartPoint = (Vector3)pointsList[pointsList.Count - 2];
            currentLine.EndPoint = (Vector3)pointsList[pointsList.Count - 1];
            if (isLinesIntersect(lines[i], currentLine))
                return true;
        }
        return false;
    }
    //    -----------------------------------    
    //    Following method checks whether given two points are same or not
    //    -----------------------------------    
    private bool checkPoints(Vector3 pointA, Vector3 pointB)
    {
        return (pointA.x == pointB.x && pointA.y == pointB.y);
    }
    //    -----------------------------------    
    //    Following method checks whether given two line intersect or not
    //    -----------------------------------    
    private bool isLinesIntersect(myLine L1, myLine L2)
    {
        if (checkPoints(L1.StartPoint, L2.StartPoint) ||
            checkPoints(L1.StartPoint, L2.EndPoint) ||
            checkPoints(L1.EndPoint, L2.StartPoint) ||
            checkPoints(L1.EndPoint, L2.EndPoint))
            return false;

        return ((Mathf.Max(L1.StartPoint.x, L1.EndPoint.x) >= Mathf.Min(L2.StartPoint.x, L2.EndPoint.x)) &&
               (Mathf.Max(L2.StartPoint.x, L2.EndPoint.x) >= Mathf.Min(L1.StartPoint.x, L1.EndPoint.x)) &&
               (Mathf.Max(L1.StartPoint.y, L1.EndPoint.y) >= Mathf.Min(L2.StartPoint.y, L2.EndPoint.y)) &&
               (Mathf.Max(L2.StartPoint.y, L2.EndPoint.y) >= Mathf.Min(L1.StartPoint.y, L1.EndPoint.y))
               );
    }
    #endregion
}
