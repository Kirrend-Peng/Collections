using System;
using System.Collections.Generic;
using UnityEngine;

public class ScriptManager : MonoBehaviour
{
    private CameraManager cameraManager;
    private AudioManager audioManager;
    private RecordAudio recorder;
    private DramaObject currentDramaObject;
    private List<EventItem> currentEvents;
    private Dictionary<string, int> dialogueIndex;
    private UIManager uiManager; 
    public List<string> dialogueLog;
    public bool lockDrama = false;

    private float timer = 0.0f; // 計時器初始值
    private float timerForSeconds = 0.0f; // 計時器初始值


    [Serializable]
    public class EventItem
    {
        public string Type;
        public string Object;
        public string TargetObject;
        public bool HaveRead;
    }

    [Serializable]
    public class Event
    {
        public List<EventItem> eventItem;
    }

    [Serializable]
    public class DramaObject
    {
        public List<Participant> Participants;
        public List<Event> Events;
    }

    [System.Serializable]
    public class Participant
    {
        public string Object;
        public string Type;
    }

    // Start is called before the first frame update
    void Start()
    {
        cameraManager = GameObject.Find("CameraManager").GetComponent<CameraManager>();
        dialogueIndex = new Dictionary<string, int>();
        audioManager = GameObject.Find("GameManager").transform.Find("MusicManager").GetComponent<AudioManager>();
        recorder = GameObject.Find("SystemSettingObject").GetComponent<RecordAudio>();
        uiManager = GameObject.Find("UI").GetComponent<UIManager>();
        dialogueLog = new List<string>();
    }

    // Update is called once per frame
    void Update()
    {
        CheckTimer();
        CheckEvents();
    }

    private void CheckEvents()
    {
        if (currentDramaObject != null)
        {
            if (currentDramaObject != null && currentDramaObject.Events.Count > 0)
                currentEvents = currentDramaObject.Events[0].eventItem;
            else
            {
                foreach (var person in currentDramaObject.Participants)
                {
                    if (person.Type.Equals("NPC"))
                        GameObject.Find(person.Object).GetComponent<NPC>().status = NPC.NPCStatus.Idle;
                    else if (person.Type.Equals("Player"))
                        GameObject.Find(person.Object).GetComponent<Player>().player_status = Player.PlayerStatus.Idle;
                }

                lockDrama = false;
                recorder.lockMicrophone = false;
                currentEvents = null;
                currentDramaObject = null;
                dialogueIndex.Clear();
                dialogueLog.Clear();
            }

            if (currentEvents != null)
            {
                for (int i = 0; i < currentEvents.Count;)
                {
                    bool deleteItem = false;
                    if (currentEvents[i].HaveRead == false)
                    {
                        switch (currentEvents[i].Type)
                        {
                            case "FirstPerson":
                                break;
                            case "Teleport":
                                GameObject.Find(currentEvents[i].Object).transform.position = GameObject.Find(currentEvents[i].TargetObject).transform.position;
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "ForcedToLoadBranchScript":
                                string targetScript = new string (currentEvents[i].Object);
                                // reset begin
                                currentEvents.Clear();
                                currentEvents = null;
                                currentDramaObject.Events.Clear();
                                LoadBranchScript(targetScript);
                                currentEvents = currentDramaObject.Events[0].eventItem;
                                i = 0;
                                deleteItem = true;
                                // reset end
                                break;
                            case "WaitNPCChatBubble":
                                break;
                            case "WaitKeyPress":
                                KeyCode keyCode;
                                if (Enum.TryParse(currentEvents[i].Object, out keyCode)) 
                                {
                                    if (Input.GetKeyDown(keyCode)) 
                                    {
                                        currentEvents.RemoveAt(i);
                                        deleteItem = true;
                                    }
                                }

                                break;
                            case "WaitRecorderInput":
                                recorder.lockMicrophone = false;
                                break;
                            case "GetJudgeResult":
                                GameObject.Find(NPC.NPCRole.PresidingJudge.ToString()).GetComponent<NPC>().threadDictionary.Add(NPC.NPCStatus.Talking, ChatBubble.GetJudgeResultAsync(string.Join("\n", dialogueLog)));
                                GameObject.Find(NPC.NPCRole.PresidingJudge.ToString()).GetComponent<NPC>().getTalking = true;
                                break;
                            case "NPCGetTalkFromDialogue":
                                // 將尚未讀取的Dialogue內容(根據指標判別)當作input, 第一次執行先建立npc talk執行序
                                int index = -1;
                                dialogueIndex.TryGetValue(currentEvents[i].Object, out index);
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().threadDictionary.Add(NPC.NPCStatus.Talking, ChatBubble.GetChatContentAsync(currentEvents[i].Object, string.Join("\n", dialogueLog.GetRange(index, dialogueLog.Count - index))));
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().getTalking = true;
                                dialogueIndex[currentEvents[i].Object] = dialogueLog.Count;
                                break;
                            case "NPCGetTalk":
                                // 將TargetObject內容當作input, 第一次執行先建立npc talk執行序,npc talk執行序完成後,檢查npc 待演出對話變數是否有值, 動畫設定用animator parameter trigger。
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().threadDictionary.Add(NPC.NPCStatus.Talking, ChatBubble.GetChatContentAsync(currentEvents[i].Object, currentEvents[i].TargetObject));
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().getTalking = true;
                                break;
                            case "AddPendingTalkToDialogue":
                                //  將pendingResponse內容放入ScriptManager-Dialogue
                                foreach (var item in GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().pendingResponse) {
                                    dialogueLog.Add(currentEvents[i].Object+":\""+item.say+"\"");
                                }
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "LetNPCSay":
                                // 將TargetObject 放在npc 變數 pendingResponse
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().AddPendingResponse(currentEvents[i].TargetObject);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "LetPlayerSay":
                                // 將TargetObject 放在npc 變數 pendingResponse
                                GameObject.Find(currentEvents[i].Object).GetComponent<Player>().pendingResponse.Add(currentEvents[i].TargetObject);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "PlayerDisplayTalk":
                                // 將npc 變數 getTalking 設定true
                                GameObject.Find(currentEvents[i].Object).GetComponent<Player>().getTalking = true;
                                break;
                            case "NPCDisplayTalk":
                                // 將npc 變數 getTalking 設定true
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().getTalking=true;
                                break;
                            case "NPCMove":
                                // 第一次執行先設定npc target參數
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().target = GameObject.Find(currentEvents[i].TargetObject).transform;
                                Debug.Log("處理NPC移動事件");
                                break;
                            case "PlayerMove":
                                // 第一次執行先設定npc target參數
                                GameObject.Find(currentEvents[i].Object).GetComponent<Player>().target = GameObject.Find(currentEvents[i].TargetObject).transform;
                                Debug.Log("處理Player移動事件");
                                break;
                            case "SetIntroductionByID":
                                uiManager.SetIntroduction(currentEvents[i].Object);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "IntroductionFlyIn":
                                uiManager.IntroductionFlyIn();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "IntroductionFlyOut":
                                uiManager.IntroductionFlyOut();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "SetStageIntroByID":
                                uiManager.SetStageIntro(currentEvents[i].Object);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "StageIntroPushIn":
                                uiManager.SetStageIntroPushIn();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "StageIntroPushOut":
                                uiManager.SetStageIntroPushOut();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "WideScreen":
                                uiManager.InDramaMode();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "NormalScreen":
                                uiManager.OutDramaMode();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "CameraFollow":
                                // 待實作-Cinemachine ,第一次執行切換為Object:Camera1的Camera,慢運鏡?(保留), 指定目標
                                cameraManager.CameraFollowSpecificObject(currentEvents[i].Object, currentEvents[i].TargetObject);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "CameraLookAt":
                                // 待實作-Cinemachine ,第一次執行切換為Object:Camera1的Camera,慢運鏡?(保留), 指定目標
                                cameraManager.CameraLookAtSpecificObject(currentEvents[i].Object, currentEvents[i].TargetObject);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "CameraZoom":
                                // 待實作-Cinemachine ,第一次執行切換為Object:Camera1的Camera,慢運鏡?(保留), 指定目標
                                cameraManager.CameraZoom(currentEvents[i].Object, float.Parse(currentEvents[i].TargetObject));
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;

                            case "CameraSwitch":
                                if (currentEvents[i].Object.Equals("VCam1"))
                                    cameraManager.SwitchToCamera1();
                                else if (currentEvents[i].Object.Equals("VCam2"))
                                    cameraManager.SwitchToCamera2();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "CameraMoveToCamPoint":
                                cameraManager.CameraMoveToSpecificCamPoint(currentEvents[i].Object, currentEvents[i].TargetObject);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "PlayBGM":
                                // 第一次執行切換為指定音樂
                                Debug.Log("處理背景音樂事件");
                                audioManager.PlayMusic(currentEvents[i].TargetObject);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "PauseBGM":
                                // 第一次執行切換為指定音樂
                                Debug.Log("處理背景音樂事件");
                                audioManager.PauseMusic();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "ContinueBGM":
                                // 第一次執行切換為指定音樂
                                Debug.Log("處理背景音樂事件");
                                audioManager.ContinueMusic();
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "PlaySoundEffect":
                                // 播放特定音效
                                audioManager.PlaySoundEffect(currentEvents[i].TargetObject);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "Sleep":  // 需要計時器計時
                                // 延遲或暫停一段時間
                                timer = 0.0f;
                                timerForSeconds = float.Parse(currentEvents[i].Object);
                                Debug.Log("處理暫停/睡眠事件");
                                break;
                            case "EnableObj":
                                GameObject.Find(currentEvents[i].Object).SetActive(true);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "DisableObj":
                                GameObject.Find(currentEvents[i].Object).SetActive(false);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "SetNPCAnimatiorParaBool":
                                bool boolValue = bool.Parse(currentEvents[i].TargetObject.Split('/')[1]);
                                string boolparaName = currentEvents[i].TargetObject.Split('/')[0];
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().animator.SetBool(boolparaName, boolValue);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "SetNPCAnimatiorParaFloat":
                                float floatValue = float.Parse(currentEvents[i].TargetObject.Split('/')[1]);
                                string floatparaName = currentEvents[i].TargetObject.Split('/')[0];
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().animator.SetFloat(floatparaName, floatValue);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            case "SetNPCAnimatiorParaInt":
                                int intValue = int.Parse(currentEvents[i].TargetObject.Split('/')[1]);
                                string intparaName = currentEvents[i].TargetObject.Split('/')[0];
                                GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().animator.SetInteger(intparaName, intValue);
                                currentEvents.RemoveAt(i);
                                deleteItem = true;
                                break;
                            default:
                                Debug.LogWarning($"未知的事件類型: {currentEvents[i].Type}");
                                break;
                        }
                        // 標記為已讀，以避免重複執行
                        if (deleteItem == false)
                        {
                            currentEvents[i].HaveRead = true;
                            ++i;
                        }
                    }
                    else
                    {
                        switch (currentEvents[i].Type)
                        {
                            case "WaitKeyPress":
                                KeyCode keyCode;
                                if (Enum.TryParse(currentEvents[i].Object, out keyCode))
                                {
                                    if (Input.GetKeyDown(keyCode))
                                    {
                                        currentEvents.RemoveAt(i);
                                        deleteItem = true;
                                    }
                                }

                                break;
                            case "WaitNPCChatBubble":
                                var tmp_npc = GameObject.Find(currentEvents[i].Object).GetComponent<NPC>();
                                if (tmp_npc.chatBubble.chatAnim == false && tmp_npc.getTalking == false) { 
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }
                                break;
                            case "PlayerDisplayTalk":
                                // 將npc 變數 getTalking 設定true
                                if (GameObject.Find(currentEvents[i].Object).GetComponent<Player>().getTalking == false ) { 
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }
                                break;
                            case "NPCDisplayTalk":
                                // 將npc 變數 getTalking 設定true
                                if (GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().getTalking == false) { 
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }
                                break;
                            case "WaitRecorderInput":
                                if (recorder.lockMicrophone == true && GameObject.Find(currentEvents[i].Object).GetComponent<Player>().getTalking == false)
                                {
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }

                                break;
                            case "GetJudgeResult":
                                // 檢查npc getTalking是否有值, 動畫設定用animator parameter trigger。
                                if (GameObject.Find(NPC.NPCRole.PresidingJudge.ToString()).GetComponent<NPC>().getTalking == false)
                                {
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                    Debug.Log("JudgeResultDone");
                                }
                                
                                break;
                            case "NPCGetTalkFromDialogue":
                                // 檢查npc getTalking是否有值, 動畫設定用animator parameter trigger。
                                if (GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().getTalking == false)
                                {
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }

                                break;
                            case "NPCGetTalk":
                                // 檢查npc getTalking是否有值, 動畫設定用animator parameter trigger。
                                if (GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().getTalking == false) 
                                {
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }

                                break;
                            case "NPCMove":
                                // 第一次執行先設定npc target參數
                                if (GameObject.Find(currentEvents[i].Object).GetComponent<NPC>().target == null) { 
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }
                                break;
                            case "PlayerMove":
                                // 第一次執行先設定npc target參數
                                if (GameObject.Find(currentEvents[i].Object).GetComponent<Player>().target == null)
                                {
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                }
                                break;
                            case "Sleep":  // 需要計時器計時
                                if (timer >= timerForSeconds)
                                {
                                    currentEvents.RemoveAt(i);
                                    deleteItem = true;
                                    timer = 0.0f;
                                    timerForSeconds = 0.0f;
                                }
                                break;
                            default:
                                Debug.LogWarning($"未知的事件類型: {currentEvents[i].Type}");
                                break;
                        }
                        // 標記為已讀，以避免重複執行
                        if (deleteItem == false)
                        {
                            currentEvents[i].HaveRead = true;
                            ++i;
                        }
                    }  // 2次以上狀況
                }

                if (currentEvents.Count == 0)
                    currentDramaObject.Events.RemoveAt(0);
            }
        }
    }

    public bool LoadBranchScript(string scriptName)
    {
        lockDrama = true;
        TextAsset jsonFile = Resources.Load<TextAsset>("Script/Branch/" + scriptName);

        // 檢查是否成功加載檔案
        if (jsonFile != null)
        {
            currentDramaObject.Events = JsonUtility.FromJson<DramaObject>(jsonFile.text).Events;
            return true;
        }

        return false;
    }

    public bool LoadScript( string scriptName)
    {
        lockDrama = true;
        TextAsset jsonFile = Resources.Load<TextAsset>("Script/"+scriptName);

        // 檢查是否成功加載檔案
        if (jsonFile != null)
        {
            currentDramaObject = JsonUtility.FromJson<DramaObject>(jsonFile.text);
            foreach (var person in currentDramaObject.Participants)
            {
                dialogueIndex.Add(person.Object, 0);
                var tmpGameObject = GameObject.Find(person.Object);
                if ( person.Type.Equals("NPC"))
                    tmpGameObject.GetComponent<NPC>().status = NPC.NPCStatus.Drama;
                else if (person.Type.Equals("Player")) {
                    recorder.lockMicrophone = true;
                    tmpGameObject.GetComponent<Player>().player_status = Player.PlayerStatus.Drama;
                }
            }

            return true;
        }

        return false;
    }

    private bool AnyParticipantsIsBusy(List<Participant> participants) 
    { 
        foreach ( var person in participants) 
        {
            var tmpGameObject = GameObject.Find(person.Object);
            if (person.Type.Equals("NPC")) {
                var tmpNPC = tmpGameObject.GetComponent<NPC>();
                if (tmpNPC.status != NPC.NPCStatus.Idle || tmpNPC.chatBubble.chatAnim == true || tmpNPC.getTalking == true)
                    return true;
            }
            else if (person.Type.Equals("Player"))
            {
                var tmpPlayer = tmpGameObject.GetComponent<Player>();
                if (tmpPlayer.player_status != Player.PlayerStatus.Idle || tmpPlayer.chatBubble.chatAnim == true || tmpPlayer.getTalking == true)
                    return true;
            }
        }

        return false;
    }

    
    private void CheckTimer()
    {
        if (timerForSeconds > 0)
        {
            timer += Time.deltaTime;
        }
    }
}
