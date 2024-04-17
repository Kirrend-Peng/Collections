
using System.Collections;
using UnityEngine;
using UnityEngine.Networking;

public class MeteorologyClass : MonoBehaviour
{
    private Light lt;
    private ParticleSystem rain;
    private string apiUrl = "https://opendata.cwa.gov.tw/api/v1/rest/datastore/F-C0032-001?Authorization=";
    // Start is called before the first frame update
    private SkyboxManager skyboxManager;
    void Start()
    {
        skyboxManager = transform.GetComponent<SkyboxManager>();
        lt = transform.transform.Find("Sun").GetComponent<Light>();
        rain = transform.transform.Find("RainFall").GetComponent<ParticleSystem>();
        // 啟動協程以呼叫API
        StartCoroutine(SetWeatherFromAPIServer());
    }

    private void SetSunLight( float lightValue)
    {
        lt.intensity = lightValue;
    }

    private void SetRain( float rainWeight) 
    {
        var main = rain.main;
        if (rainWeight > 0)
        {
            main.gravityModifier = 1f;
            if ( rainWeight <= 5 )
            {
                var emission = rain.emission;
                emission.rateOverTime = (int)(200*rainWeight);
                main = rain.main;
                main.maxParticles = (int)(1000*rainWeight);
            }
            else
            {
                var emission = rain.emission;
                emission.rateOverTime = 1000;
                main = rain.main;
                main.maxParticles = 5000;
            }
        }
        else {
            main.gravityModifier = 0f;
            //rain.gravityModifier = 0f;
        }

        rain.Stop();
        rain.Play();
    }

    public void SetWeather( string weatherDescription )
    {
        switch (weatherDescription)
        {
            case "多雲時陰":
                lt.intensity = 0.5f;
                break;
            case "多雲":
                lt.intensity = 0.7f;
                break;
            case "晴時多雲":
                lt.intensity = 0.8f;
                break;
            case "陰時多雲":
                lt.intensity = 0.3f;
                break;
            case "多雲時晴":
                lt.intensity = 1f;
                break;
            case "陰天":
                lt.intensity = 1f;
                break;
            case "陰短暫雨":
                lt.intensity = 1f;
                break;
            default:
                lt.intensity = 0f;
                break;
        }

    }

    IEnumerator SetWeatherFromAPIServer()
    {
        // 使用UnityWebRequest創建一個GET請求
        using (UnityWebRequest request = UnityWebRequest.Get(apiUrl))
        {
            // 發送請求並等待請求完成
            yield return request.SendWebRequest();

            // 檢查是否有錯誤發生
            if (request.result == UnityWebRequest.Result.ConnectionError || request.result == UnityWebRequest.Result.ProtocolError)
            {
                //Debug.LogError("Error: " + request.error);
                lt.intensity = 0.5f;
                skyboxManager.SwitchToCloudyDay();

            }
            else
            {
                // 處理響應的內容

                FC0032001Response fC0032001Response = JsonUtility.FromJson<FC0032001Response>(request.downloadHandler.text);
                switch (fC0032001Response.records.location[5].weatherElement[0].time[0].parameter.parameterName)
                {
                    case "多雲時陰":
                        lt.intensity = 0.5f;
                        skyboxManager.SwitchToCloudyDay();
                        break;
                    case "多雲":
                        lt.intensity = 0.7f;
                        skyboxManager.SwitchToCloudyDay();
                        break;
                    case "晴時多雲":
                        skyboxManager.SwitchToCloudyDay();
                        break;
                    case "陰時多雲":
                        lt.intensity = 0.3f;
                        skyboxManager.SwitchToCloudyDay();
                        break;
                    case "多雲時晴":
                        lt.intensity = 1f;
                        skyboxManager.SwitchToCloudyDay();
                        break;
                    case "陰天":
                        skyboxManager.SwitchToRainyDay();
                        lt.intensity = 0.3f;
                        break;
                    case "陰短暫雨":
                        skyboxManager.SwitchToRainyDay();
                        SetRain(1);
                        lt.intensity = 0.3f;
                        break;
                    case "晴天":
                        skyboxManager.SwitchToDay();
                        lt.intensity = 2f;
                        break;
                    default:
                        lt.intensity = 0f;
                        break;
                }

                Debug.Log("Received: " + fC0032001Response.records.location[0].weatherElement[0].time[0].parameter.parameterName);

                // 這裡可以根據需要處理或解析獲取的數據
            }
        }
    }
}
