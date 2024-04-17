using System.Collections;
using System.IO;
using System.Threading.Tasks;
using TMPro;
using UnityEngine;
using UnityEngine.Networking;
using static UtilityClass;

public class ChatBubble
{
    public float fadeInDuration = 2.0f; // 完成淡入效果所需的時間（秒
    public float idleTime = 1.0f;
    public float fadeOutDuration = 1.0f;

    private float currentTime = 0.0f; // 初始counter
    public bool chatAnim = false;
    private bool chatFadeIn = false;
    public bool chatTextVisual = false;
    private bool chatIdle = false;
    private bool chatFadeOut = false;
    public bool waitToLoadVoiceAudio = false;
    private Task<string> textToAudioTask;
    private string originText;
    private int currentTextIndex;
    private float defaultTextDuration = 0.1f;

    private AudioSource audioSource = null;

    private static readonly float text_initial_y = 1.7f;
    private static readonly float chat_bubble_initial_scale_y = 1f;

    private static readonly float text_interval_y = 0.65f;
    private static readonly float chat_bubble_scale_interval_y = 0.45f;

    private TextMeshPro textMesh;
    private SpriteRenderer chatbubble;


    public ChatBubble(TextMeshPro textMesh, SpriteRenderer chatbubble)
    {
        this.textMesh = textMesh;
        this.chatbubble = chatbubble;
    }

    #region --Enum--

    enum Attitude
    {
        Angry,
        Sad,
        Normal,
        Surprised,
        Evil
    }
    private enum FontColor
    {
        Red,
        Blue,
        LightBlue,
        Green,
        Yellow,
        Black,
        White
    }
    private enum FontSize
    {
        Small,
        Medium,
        Large
    }

    public enum EnumTalkingStatus
    {
        Running,
        Anime,
        Idle
    }
    #endregion



    public void SetUp(string text)
    {
        textMesh.text = "<alpha=#00>" + text;
        originText = text;
        ResetSetUpPara();
        textToAudioTask = null;
        Vector3 textMeshSize = textMesh.GetRenderedValues(false);
        SelectChatBubbleSpriteAndTextColor(4);
    }

    public void SetUp(Chat.Normal_Response npcResponse)
    {
        var text = npcResponse.Say;
        textMesh.text = "<alpha=#00>" + text;
        originText = text;
        ResetSetUpPara();
        textToAudioTask = null;
        Vector3 textMeshSize = textMesh.GetRenderedValues(false);
        SelectChatBubbleSpriteAndTextColor(npcResponse.Attitude);
    }

    public void SetUpWithVoice(Chat.Normal_Response npcResponse, Task<string> voiceThread)
    {
        var text = npcResponse.Say;
        textMesh.text = "<alpha=#00>" + text;
        originText = text;
        ResetSetUpPara();
        Vector3 textMeshSize = textMesh.GetRenderedValues(false);
        textToAudioTask = voiceThread;
        SelectChatBubbleSpriteAndTextColor(npcResponse.Attitude);
    }

    private void ResetSetUpPara()
    {
        chatFadeIn = false;
        chatIdle = false;
        chatFadeOut = false;
        chatTextVisual = false;
        waitToLoadVoiceAudio = false;
        currentTime = 0;
    }

    private void SelectChatBubbleSpriteAndTextColor(int attitude)
    {
        if (attitude == 1)
        {
            chatbubble.sprite = Resources.Load<Sprite>("Chatbubble/chat_bubble_badmood");
            SetFontColor(FontColor.LightBlue);
        }
        else if (attitude == 2)
        {
            SetFontColor(FontColor.Black);
            chatbubble.sprite = Resources.Load<Sprite>("Chatbubble/chat_bubble_angry");
        }
        else if (attitude == 3 || attitude == 4 || attitude == 5)
        {
            SetFontColor(FontColor.Black);
            chatbubble.sprite = Resources.Load<Sprite>("Chatbubble/chat_bubble_normal2");
        }
        else if (attitude == 6)
        {
            SetFontColor(FontColor.Black);
            chatbubble.sprite = Resources.Load<Sprite>("Chatbubble/chat_bubble_think");
        }
    }

    public void ChatBubbleAnime(bool endChatBubble)
    {
        if (chatFadeIn == false)
        {
            Color color = chatbubble.color;
            float alpha = currentTime / fadeInDuration;
            currentTime += Time.deltaTime;
            if (color.a < 1f)
            {
                color.a = alpha;
            }
            else if (color.a >= 1f)
            {
                color.a = 1f;
                currentTime = 0;
                chatFadeIn = true;
                currentTextIndex = 0;
            }

            chatbubble.color = color;
        }
        else if (textToAudioTask != null)
        {
            if (textToAudioTask.IsCompleted)
            {
                if (textToAudioTask.Result != null)
                {
                    var bodyData = JsonUtility.FromJson<TextToSpeechOutput>(textToAudioTask.Result);
                    audioSource = GameObject.Find(bodyData.AssistantName).GetComponent<AudioSource>();
                    string filePath = Path.Combine(Application.persistentDataPath, bodyData.AssistantName + ".mp3");
                    WriteBytesFile(filePath, ConvertBase64ToBytes(bodyData.AudioBase64));
                    textToAudioTask = null;
                    waitToLoadVoiceAudio = true;
                }
                else
                {
                    waitToLoadVoiceAudio = false;
                    textToAudioTask = null;
                }
            }
        }
        else if (waitToLoadVoiceAudio) { }
        else if (chatTextVisual == false)
        {
            if (currentTextIndex < originText.Length)
            {
                if (currentTime >= defaultTextDuration)
                {
                    currentTime = 0;
                    currentTextIndex += 1;
                    textMesh.text = originText.Insert(currentTextIndex, "<alpha=#00>");
                }
                else
                {
                    currentTime += Time.deltaTime;
                }
            }
            else
            {
                chatTextVisual = true;
            }
        }
        else if (audioSource != null && audioSource.isPlaying) { }
        else if (chatIdle == false) 
        {
            currentTime += Time.deltaTime;
            if ( currentTime >= idleTime) { 
                currentTime = 0;
                chatIdle = true;
            }
        }
        else if (chatFadeOut == false)
        {
            if (endChatBubble == false)
            {
                currentTime += Time.deltaTime;
                if (currentTime >= fadeOutDuration)
                {
                    if (audioSource != null)
                    {
                        if (audioSource.clip != null)
                        {
                            audioSource.Stop();
                            audioSource.clip = null;
                            audioSource = null;
                        }
                    }

                    currentTime = 0;
                    chatFadeOut = true;
                    chatAnim = false;
                }
                return;
            }

            Color color = chatbubble.color;
            float alpha = 1 - (currentTime / fadeOutDuration);
            string alphaHex = Mathf.FloorToInt(alpha * 255).ToString("X2");
            color.a = alpha;
            currentTime += Time.deltaTime;
            chatbubble.color = color;
            if (color.a <= 0)
            {
                if (audioSource != null)
                {
                    if (audioSource.clip != null)
                    {
                        audioSource.Stop();
                        audioSource.clip = null;
                        audioSource = null;
                    }
                }
                textMesh.text = $"<alpha=#00>" + originText;
                color.a = 0f;
                currentTime = 0;
                chatFadeOut = true;
                chatAnim = false;
            }
            else
            {
                textMesh.text = $"<alpha=#{alphaHex}>" + originText;
            }
        }
    }


    private void SetChatBubbleScale(int line)
    {
        Vector3 chatbubbleScale = chatbubble.transform.localScale;
        Vector3 textPosition = textMesh.transform.position;
        if (line > 1)
        {
            chatbubble.transform.localScale = new Vector3(chatbubbleScale.x, chat_bubble_initial_scale_y + chat_bubble_scale_interval_y * (line - 1), chatbubbleScale.z);
            textMesh.transform.position = new Vector3(textPosition.x, text_initial_y + text_interval_y * (line - 1), textPosition.z);
        }
        else
        {
            chatbubble.transform.localScale = new Vector3(chatbubbleScale.x, chat_bubble_initial_scale_y, chatbubbleScale.z);
            textMesh.transform.position = new Vector3(textPosition.x, text_initial_y, textPosition.z);
        }
    }

    private int GetTextLength(string text)
    {
        int length = 0;
        for (int i = 0; i < text.Length; i++)
        {
            // if the character is a chinese character
            if (text[i] >= 0x4e00 && text[i] <= 0x9fbb)
            {
                length += 2;
            }
            else
            {
                length += 1;
            }
        }

        return length;
    }


    private void SetFontColor(FontColor color)
    {
        if (color == FontColor.Red)
        {
            textMesh.color = new Color(1, 0, 0);
        }
        else if (color == FontColor.Blue)
        {
            textMesh.color = new Color(0, 0, 1);
        }
        else if (color == FontColor.LightBlue)
        {
            textMesh.color = new Color(0.68f, 0.85f, 0.9f);
        }
        else if (color == FontColor.Green)
        {
            textMesh.color = new Color(0, 1, 0);
        }
        else if (color == FontColor.Yellow)
        {
            textMesh.color = new Color(1, 1, 0);
        }
        else if (color == FontColor.Black)
        {
            textMesh.color = new Color(0, 0, 0);
        }
        else if (color == FontColor.White)
        {
            textMesh.color = new Color(1, 1, 1);
        }
    }

    public static Task<string> GetJudgeResultAsync(string input)
    {
        EntityAPIClient entityAPIClient = new EntityAPIClient();
        Chat.Request npc_chat = new Chat.Request { Input = input };
        var sendRequestTask = entityAPIClient.SendHttpRequestToAPIServer<Chat.Request, Chat.Response>(npc_chat, "OpenAI", "GetJudgeResult");
        return sendRequestTask;
    }

    public static Task<string> GetChatContentAsync(string NPC_Name, string input)
    {
        EntityAPIClient entityAPIClient = new EntityAPIClient();
        Chat.Request npc_chat = new Chat.Request { AssistantName = NPC_Name, Input = input };
        var sendRequestTask = entityAPIClient.SendHttpRequestToAPIServer<Chat.Request, Chat.Response>(npc_chat, "OpenAI", "GetAssistantResponse");
        return sendRequestTask;
    }

    public static Task<string> AudioBytesToSpeech(byte[] audioBytes)
    {
        EntityAPIClient entityAPIClient = new EntityAPIClient();
        UtilityClass.AudioInput audioBodyData = new UtilityClass.AudioInput { AudioBase64Str = UtilityClass.ConvertAudioFileToBase64(audioBytes) };
        var sendRequestTask = entityAPIClient.SendHttpRequestToAPIServer<UtilityClass.AudioInput, UtilityClass.SpeechToTextOutput>(audioBodyData, "OpenAI", "SpeechToText");
        return sendRequestTask;
    }

    public static Task<string> TextToAudio(string text, string roleName)
    {
        EntityAPIClient entityAPIClient = new EntityAPIClient();
        UtilityClass.TextToSpeechInput bodyData = new UtilityClass.TextToSpeechInput { AssistantName = roleName, Text = text };
        var sendRequestTask = entityAPIClient.SendHttpRequestToAPIServer<UtilityClass.TextToSpeechInput, UtilityClass.TextToSpeechOutput>(bodyData, "VoiceManager", "GenerateAudio");
        return sendRequestTask;
    }

    public IEnumerator LoadVoiceAudio(string role)
    {
        // 假設你已經將MP3字節數據保存到了這個路徑
        string path = "file://" + Path.Combine(Application.persistentDataPath, role + ".mp3");
        using (UnityWebRequest uwr = UnityWebRequestMultimedia.GetAudioClip(path, AudioType.MPEG))
        {
            yield return uwr.SendWebRequest();

            if (uwr.result == UnityWebRequest.Result.ConnectionError || uwr.result == UnityWebRequest.Result.ProtocolError)
            {
                Debug.LogError(uwr.error);
            }
            else
            {
                AudioClip clip = DownloadHandlerAudioClip.GetContent(uwr);
                audioSource.clip = clip;
                audioSource.Play();
                waitToLoadVoiceAudio = false;
            }
        }
    }
}

