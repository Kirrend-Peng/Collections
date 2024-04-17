using UnityEngine;

public class SkyboxManager : MonoBehaviour
{
    public Material skyboxDay; // 日間的Skybox材質
    public Material cloudyDay; // 多雲日間的Skybox材質
    public Material rainyDay; // 雨天日間的Skybox材質
    public Material skyboxNight; // 夜間的Skybox材質

    // 這個函數將Skybox切換到日間模式
    public void SwitchToDay()
    {
        RenderSettings.skybox = skyboxDay;
        // 重要的是要更新天空盒以反映變化
        DynamicGI.UpdateEnvironment();
    }

    public void SwitchToCloudyDay()
    {
        RenderSettings.skybox = cloudyDay;
        // 重要的是要更新天空盒以反映變化
        DynamicGI.UpdateEnvironment();
    }

    public void SwitchToRainyDay()
    {
        RenderSettings.skybox = rainyDay;
        // 重要的是要更新天空盒以反映變化
        DynamicGI.UpdateEnvironment();
    }

    // 這個函數將Skybox切換到夜間模式
    public void SwitchToNight()
    {
        RenderSettings.skybox = skyboxNight;
        // 重要的是要更新天空盒以反映變化
        DynamicGI.UpdateEnvironment();
    }
}