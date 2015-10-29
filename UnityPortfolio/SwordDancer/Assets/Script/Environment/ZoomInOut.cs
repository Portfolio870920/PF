using UnityEngine;
using System.Collections;

public class ZoomInOut : MonoBehaviour 
{
                    bool                 zoom                       = false;
                    bool                 zoomEnd                    = false;
    public          float                zoomvalue                  = 5;
    public          float                zoomSpeed                  = 1.0f;
    public          float                RotateOffset                = 2.0f;
                    MainCamera           MainCam                    = null;

	void Awake()
    {
        // MainCamera를 찾음
        MainCam = GameObject.Find("MainCamera").GetComponent<MainCamera>();
    }

    void OnClick()
    {
        if ( !zoomEnd )
        {
            StartCoroutine(Zoom_InOut(!zoom));
            zoom = !zoom;
        }        
    }

    IEnumerator Zoom_InOut(bool _Zoom)
    {
        bool        DistanceEnd         = false;
        bool        RotateEnd           = false;
        zoomEnd = true;

        // 삼항연산자 버전
        int         signe               = 1;
        if (!zoom)
        {
            signe *= -1;
        }
        
        while (true)
        {
            // 거리
            MainCam.Currentdistance += (zoomvalue * Time.smoothDeltaTime * zoomSpeed) * signe;
            if (zoom ? MainCam.Currentdistance < MainCam.GetOriDistance() - zoomvalue : MainCam.Currentdistance > MainCam.GetOriDistance())
            {
                if (zoom)
                {
                    MainCam.Currentdistance = MainCam.GetOriDistance() - zoomvalue;
                }
                else
                {
                    MainCam.Currentdistance = MainCam.GetOriDistance();
                }
                DistanceEnd = true;
            }

            // 각도
            MainCam.RotateX += (zoomvalue * Time.smoothDeltaTime * (zoomSpeed + RotateOffset)) * signe;
            if (zoom ? MainCam.RotateX < MainCam.GetOriRotate() - (zoomvalue + RotateOffset) : MainCam.RotateX > MainCam.GetOriRotate())
            {
                if (zoom)
                {
                    MainCam.RotateX = MainCam.GetOriRotate() - (zoomvalue + RotateOffset);
                }
                else
                {
                    MainCam.RotateX = MainCam.GetOriRotate();
                }
                RotateEnd = true;
            }

            if (RotateEnd && DistanceEnd)
            {
                break;
            }

            MainCam.Reset(MainCam.RotateX, MainCam.RotateY);

            yield return null;
        }
        zoomEnd = false;

        //// 일반버전
        //if (_Zoom)
        //{
        //    // zoomin
        //    while (true)
        //    {
        //        // 거리
        //        MainCam.Currentdistance -= zoomvalue * Time.smoothDeltaTime * zoomSpeed;
        //        if (MainCam.Currentdistance < MainCam.GetOriDistance() - zoomvalue)
        //        {
        //            MainCam.Currentdistance = MainCam.GetOriDistance() - zoomvalue;
        //            DistanceEnd = true;
        //        }                   

        //        // 각도
        //        MainCam.RotateX -= zoomvalue * Time.smoothDeltaTime * zoomSpeed * RotateOffset;
        //        if (MainCam.RotateX < MainCam.GetOriRotate() - (zoomvalue * RotateOffset))
        //        {
        //            MainCam.RotateX = MainCam.GetOriRotate() - (zoomvalue * RotateOffset);
        //            RotateEnd = true;
        //        }

        //        if (RotateEnd && DistanceEnd)
        //        {
        //            break;
        //        }

        //        MainCam.Reset(MainCam.RotateX, MainCam.RotateY);
                
        //        yield return null;
        //    }
        //}
        //else
        //{
        //    // zoomout
        //    while (true)
        //    {
        //        // 거리
        //        MainCam.Currentdistance += zoomvalue * Time.smoothDeltaTime * zoomSpeed;
        //        if (MainCam.Currentdistance > MainCam.GetOriDistance())
        //        {
        //            MainCam.Currentdistance = MainCam.GetOriDistance();
        //            DistanceEnd = true;
        //        }

        //        // 각도
        //        MainCam.RotateX += zoomvalue * Time.smoothDeltaTime * zoomSpeed * RotateOffset;
        //        if (MainCam.RotateX > MainCam.GetOriRotate())
        //        {
        //            MainCam.RotateX = MainCam.GetOriRotate();
        //            RotateEnd = true;
        //        }

        //        if (RotateEnd && DistanceEnd)
        //        {
        //            break;
        //        }

        //        MainCam.Reset(MainCam.RotateX, MainCam.RotateY);

        //        yield return null;
        //    }
        //}
        //zoomEnd = false;
    }
}
