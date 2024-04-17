using Cinemachine;
using UnityEngine;

public class CameraManager : MonoBehaviour
{
    public CinemachineVirtualCamera VCam1;
    public CinemachineVirtualCamera VCam2;

    void Start()
    {
        VCam1 = GameObject.Find("CameraManager").transform.Find("VCam1").GetComponent<CinemachineVirtualCamera>();
        VCam2 = GameObject.Find("CameraManager").transform.Find("VCam2").GetComponent<CinemachineVirtualCamera>();
    }

    // 切換到虛擬相機1
    public void SwitchToCamera1()
    {
        VCam1.Priority = 10;
        VCam2.Priority = 0;
    }

    // 切換到虛擬相機2
    public void SwitchToCamera2()
    {
        VCam1.Priority = 0;
        VCam2.Priority = 10;
    }

    // 切換到虛擬相機2
    public void CameraZoom(string vCamName, float value)
    {
        GameObject.Find("CameraManager").transform.Find(vCamName).GetComponent<CinemachineVirtualCamera>().m_Lens.FieldOfView = value;
    }

    public void CameraFollowSpecificObject(string vCamName, string objectName)
    {
        var vCam = GameObject.Find("CameraManager").transform.Find(vCamName).GetComponent<CinemachineVirtualCamera>();
        if (objectName.Equals("None"))
            vCam.Follow = null;
        else
            vCam.Follow = GameObject.Find(objectName).transform;
    }

    public void CameraLookAtSpecificObject(string vCamName, string objectName)
    {
        var vCam = GameObject.Find("CameraManager").transform.Find(vCamName).GetComponent<CinemachineVirtualCamera>();
        if (objectName.Equals("None"))
            vCam.LookAt = null;
        else
        {
            var target = GameObject.Find(objectName).transform;
            vCam.LookAt = target;
        }
    }

    public void CameraMoveToSpecificCamPoint(string vCamName, string objectName)
    {
        var vCam = GameObject.Find("CameraManager").transform.Find(vCamName).GetComponent<CinemachineVirtualCamera>();
        var target = GameObject.Find(objectName).transform;
        vCam.transform.position = target.transform.position;
    }
}

