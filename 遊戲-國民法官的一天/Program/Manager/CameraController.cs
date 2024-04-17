using UnityEngine;

public class CameraControl : MonoBehaviour
{
    public bool lockCamera = true;
    public float mouseSensitivity = 100f; // 滑鼠靈敏度
    public Transform playerBody; // 玩家的Transform組件

    private float xRotation = 0f;

    void Start()
    {
        Cursor.lockState = CursorLockMode.Locked; // 鎖定游標到遊戲視窗中心
    }

    void Update()
    {
        if (lockCamera == false)
        {
            // 獲取滑鼠移動量
            float mouseX = Input.GetAxis("Mouse X") * mouseSensitivity * Time.deltaTime;
            float mouseY = Input.GetAxis("Mouse Y") * mouseSensitivity * Time.deltaTime;

            xRotation -= mouseY;
            xRotation = Mathf.Clamp(xRotation, -90f, 90f); // 限制垂直旋轉範圍

            // 套用旋轉
            transform.localRotation = Quaternion.Euler(xRotation, 0f, 0f);
        }
    }
}