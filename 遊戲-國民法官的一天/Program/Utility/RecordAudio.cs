using System.IO;
using UnityEngine;
using static EnumUtility;

public class RecordAudio : MonoBehaviour
{
    public static TaskRecordEnum audioTaskStatus;
    private bool isRecording = false;

    private AudioClip recording;
    private string microphone;
    private int maxAudioLength;
    private bool isTiming = false; // 是否開始計時
    private float timer = 0.0f; // 計時器
    private Player player;
    public bool lockMicrophone;

    public enum RecordStatus
    {
        Disable,
        Enable
    }

    void Start()
    {
        lockMicrophone = false;
        player = GameObject.Find(Player.PlayerObject.Player.ToString()).transform.GetComponent<Player>();
        audioTaskStatus = TaskRecordEnum.Idle;
        maxAudioLength = 20;
        // 檢查是否有可用的麥克風裝置
        if (Microphone.devices.Length > 0)
        {
            microphone = Microphone.devices[0];
        }
        else
        {
            Debug.LogError("No microphone devices found.");
        }
    }

    void Update()
    {
        if (lockMicrophone == false)
        {
            // 當按下'P'按鍵時開始錄音
            if (Input.GetKeyDown(KeyCode.Space) && audioTaskStatus == TaskRecordEnum.Idle && player.threadDictionary.ContainsKey(Player.PlayerStatus.Talking) == false)
            {
                StartRecording();
                audioTaskStatus = TaskRecordEnum.Running;
            }

            // 當放開'P'按鍵時停止錄音並匯出音檔
            if (Input.GetKeyUp(KeyCode.Space) && audioTaskStatus == TaskRecordEnum.Running)
            {
                StopRecordingAndSave();
                if (player.player_status == Player.PlayerStatus.Drama)
                    lockMicrophone = true;
            }

            if (isTiming)
            {
                timer += Time.deltaTime; // 增加計時器
            }

            // 檢查是否放開了'P'按鍵
            if (Input.GetKeyUp(KeyCode.Space))
            {
                isTiming = false; // 停止計時
                int roundedSeconds = Mathf.CeilToInt(timer); // 無條件進位秒數
                audioTaskStatus = TaskRecordEnum.Idle;
                Debug.Log("按鍵'P'被按下的時間為：" + roundedSeconds + "秒"); // 輸出進位後的持續時間
            }
        }
    }

    void StartRecording()
    {
        if (microphone != null)
        {
            isTiming = true; // 開始計時
            timer = 0.0f; // 重置計時器
            isRecording = true;
            // 開始無限循環錄音，注意這裡不設置錄音時長
            recording = Microphone.Start(microphone, true, maxAudioLength, 44100);
            Debug.Log("Recording started...");
        }
    }

    void StopRecordingAndSave()
    {
        if (isRecording)
        {
            Microphone.End(microphone);
            isRecording = false;
            Debug.Log("Recording stopped.");
            isTiming = false; // 停止計時
            int roundedSeconds = Mathf.CeilToInt(timer); // 無條件進位秒數
            Debug.Log("按鍵'P'被按下的時間為：" + roundedSeconds + "秒"); // 輸出進位後的持續時間
            // 將錄音數據保存為音檔
            SaveRecordingToFile("Recording.wav", roundedSeconds);
        }
    }

    void SaveRecordingToFile(string fileName, int roundedSeconds)
    {
        var filepath = System.IO.Path.Combine(Application.persistentDataPath, fileName);

        int sampleRate = recording.frequency; // 使用原始 AudioClip 的採樣率
        int totalSamples = (int)(roundedSeconds * sampleRate); // 計算新長度的總樣本數
        int channels = recording.channels; // 使用原始 AudioClip 的聲道數

        // 創建一個新的空 AudioClip
        AudioClip newClip = AudioClip.Create("New AudioClip", totalSamples, channels, sampleRate, false);

        // 準備一個數組來存儲原始 AudioClip 的數據
        float[] originalData = new float[recording.samples * recording.channels];
        recording.GetData(originalData, 0);

        // 準備一個數組來存儲新的 AudioClip 數據，並將原始數據複製到這個新數組中，根據需要調整長度
        float[] newData = new float[totalSamples * channels];
        for (int i = 0; i < newData.Length && i < originalData.Length; i++)
        {
            newData[i] = originalData[i];
        }

        // 將新數據設置到新的 AudioClip 對象中
        newClip.SetData(newData, 0);
        File.WriteAllBytes(filepath, EncodeAsWAV(newData, sampleRate, newClip.channels));
        EntityAPIClient entityAPIClient = new EntityAPIClient();

        player.threadDictionary.Add(Player.PlayerStatus.Talking, ChatBubble.AudioBytesToSpeech(EncodeAsWAV(newData, sampleRate, newClip.channels)));
        player.getTalking = true;

        //SavWav.Save(fileName, newClip);
        // 這裡僅為範例，具體實現取決於音檔格式和所需的數據處理方式
        // 你可能需要使用額外的庫來處理音頻數據轉換和文件寫入
        Debug.Log($"Recording saved to: {filepath}");
    }

    private byte[] EncodeAsWAV(float[] samples, int frequency, int channels)
    {
        using (var memoryStream = new MemoryStream(44 + samples.Length * 2))
        {
            using (var writer = new BinaryWriter(memoryStream))
            {
                writer.Write("RIFF".ToCharArray());
                writer.Write(36 + samples.Length * 2);
                writer.Write("WAVE".ToCharArray());
                writer.Write("fmt ".ToCharArray());
                writer.Write(16);
                writer.Write((ushort)1);
                writer.Write((ushort)channels);
                writer.Write(frequency);
                writer.Write(frequency * channels * 2);
                writer.Write((ushort)(channels * 2));
                writer.Write((ushort)16);
                writer.Write("data".ToCharArray());
                writer.Write(samples.Length * 2);

                foreach (var sample in samples)
                {
                    writer.Write((short)(sample * short.MaxValue));
                }
            }
            return memoryStream.ToArray();
        }
    }

    public static byte[] ReadWavFileBytes(string filePath)
    {
        // 使用ReadAllBytes方法讀取指定路徑的檔案內容
        byte[] fileBytes = File.ReadAllBytes(filePath);
        return fileBytes;
    }


}