using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using UnityEngine;
using static Chat;
using static NPC;

public class UtilityClass 
{
    [System.Serializable]
    public class AudioInput
    {
        public string AudioBase64Str;
    }

    [System.Serializable]
    public class SpeechToTextOutput
    {
        public string text;
    }

    [System.Serializable]
    public class TextToSpeechInput
    {
        public string AssistantName;
        public string Text;
    }

    [System.Serializable]
    public class TextToSpeechOutput
    {
        public string AssistantName;
        public string AudioBase64;
        public float AudioDuration;
        public string Text;
    }

    public static string ConvertAudioFileToBase64(string filePath)
    {
        byte[] audioBytes = File.ReadAllBytes(filePath);
        string base64String = Convert.ToBase64String(audioBytes);
        return base64String;
    }

    public static string ConvertAudioFileToBase64(byte[] audioBytes)
    {
        string base64String = Convert.ToBase64String(audioBytes);
        return base64String;
    }

    public static byte[] ConvertBase64ToBytes(string base64Str)
    {
        return Convert.FromBase64String(base64Str);
    }

    public static void WriteBytesFile(string filePath, byte[] data) 
    {
        File.WriteAllBytes(filePath, data);
    }

    public static AudioClip CreateAudioClipFromSamples(float[] samples, int sampleRate, int numChannels)
    {
        // 創建一個新的AudioClip實例
        AudioClip audioClip = AudioClip.Create("MyAudioClip", samples.Length / numChannels, numChannels, sampleRate, false);

        // 將解析的樣本數據設置給AudioClip
        audioClip.SetData(samples, 0);

        return audioClip;
    }


    public static Dictionary<string, int> InitialNPCTrustValue() 
    {
        Dictionary<string,int> relationDic = new Dictionary<string,int>();
        foreach (var row in EnumToStringList<NPCRole>()) 
        {
            relationDic.Add(row.ToString(), 0);
        }

        return relationDic;
    }

    public static List<string> EnumToStringList<T>() where T : Enum
    {
        List<string> list = new List<string>();
        foreach (var value in Enum.GetValues(typeof(T)))
        {
            list.Add(value.ToString());
        }
        return list;
    }

    public static string Combine_Role_TrustValue_Sentence( string role, int trustValue, string sentence) 
    {
        return role+"(TrustValue:"+ trustValue.ToString() + "):\"" + sentence + "\"";    
    }

    public static List<string> SplitTextByPunctuation(string text,int attitude)
    {
        List<string> parts = new List<string>();
        string pattern = @"(，|。)"; // 匹配逗號和句號

        // 使用正則表達式拆分文本
        string[] tempParts = Regex.Split(text, pattern);

        // 重新組合，每兩個標點符號作為一個部分
        string currentPart = "";
        int punctuationCount = 0;

        foreach (string part in tempParts)
        {
            currentPart += part;
            if (Regex.IsMatch(part, pattern))
            {
                punctuationCount++;
                if (punctuationCount == 2) // 每兩個標點符號拆分一次
                {
                    parts.Add(JsonUtility.ToJson(new Chat.Normal_Response{  Say=currentPart, Attitude=attitude }));
                    currentPart = "";
                    punctuationCount = 0;
                }
            }
        }

        // 如果最後一部分不為空，添加到結果中
        if (!string.IsNullOrEmpty(currentPart))
        {
            parts.Add(JsonUtility.ToJson(new Chat.Normal_Response { Say = currentPart, Attitude = attitude }));
        }

        return parts;
    }

    public static List<PendingChatResponse> SplitTextByPunctuation(string text, int attitude, string role)
    {
        List<PendingChatResponse> parts = new List<PendingChatResponse>();
        string pattern = @"(，|。)"; // 匹配逗號和句號

        // 使用正則表達式拆分文本
        string[] tempParts = Regex.Split(text, pattern);

        // 重新組合，每兩個標點符號作為一個部分
        string currentPart = "";
        int punctuationCount = 0;

        foreach (string part in tempParts)
        {
            currentPart += part;
            if (Regex.IsMatch(part, pattern))
            {
                punctuationCount++;
                if (punctuationCount == 2) // 每兩個標點符號拆分一次
                {
                    if (Enum.IsDefined(typeof(VoiceRole), role))
                    {
                        parts.Add(new PendingChatResponse(JsonUtility.ToJson(new Chat.Normal_Response { Say = currentPart, Attitude = attitude }), ChatBubble.TextToAudio(currentPart,role)));
                    }
                    else
                        parts.Add(new PendingChatResponse(JsonUtility.ToJson(new Chat.Normal_Response { Say = currentPart, Attitude = attitude }), null));
                    currentPart = "";
                    punctuationCount = 0;
                }
            }
        }

        // 如果最後一部分不為空，添加到結果中
        if (!string.IsNullOrEmpty(currentPart))
        {
            if (Enum.IsDefined(typeof(VoiceRole), role))
            {
                parts.Add(new PendingChatResponse(JsonUtility.ToJson(new Chat.Normal_Response { Say = currentPart, Attitude = attitude }), ChatBubble.TextToAudio(currentPart, role)));
            }
            else
                parts.Add(new PendingChatResponse(JsonUtility.ToJson(new Chat.Normal_Response { Say = currentPart, Attitude = attitude }), null));
        }

        return parts;
    }
}
