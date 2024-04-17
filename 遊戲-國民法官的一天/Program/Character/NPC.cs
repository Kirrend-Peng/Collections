using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using TMPro;
using UnityEngine;
using static Chat;
using static UtilityClass;

public class NPC : MonoBehaviour
{
    public Rigidbody rb;
    public float groundDistance;
    public (string, Vector3) originLoc;
    public NPCRole NPC_Role;
    public List<PendingChatResponse> pendingResponse;
    public ChatBubble chatBubble;
    public LayerMask terrainMask;
    public Dialogue dialogue;
    public NPCStatus status;
    public Dictionary<NPCStatus, Task<string>> threadDictionary;
    public bool getTalking;
    public bool loadAudio;
    public bool InTeam;
    private static Player player;
    public Transform target; // 目標位置
    public float speed = 3f; // NPC的移動速度
    public float minDistance = 0.1f; // 判斷NPC是否到達目標位置的最小距離
    private DramaScript dramaScript;
    public Animator animator;
    public SpriteRenderer sr;

    // drama


    public enum NPCRole
    {
        VillageChief,
        Judge,
        Sister,
        CourtHouseStaff,
        DefenseCounsel,
        Prosecutor,
        PresidingJudge,
        Clerk,
        ProsecutionWitness,
        DefendantWitness,
        Defendant
    }

    public enum VoiceRole
    {
        Sister,
        CourtHouseStaff,
        PresidingJudge
    }

    public enum NPCStatus
    {
        Idle,         // 空閑
        Walking,      // 行走
        Talking,      // 對話
        Attacking,    // 攻擊
        Dead,         // 死亡
        FollowUser,
        Drama
    }

    [System.Serializable]
    public class NPC_Assistant_Body
    {
        public string AssistantName;
        public string[] ResponseList;
    }

    [System.Serializable]
    public class NPC_Assistant_Response
    {
        public int Attitude;
        public string Say;
        public int SpeakSpeed;
        public string TalkTo;
    }

    [System.Serializable]
    public class Judge_Assistant_Response : NPC_Assistant_Response
    {
        public int Judge;
    }

    void Start()
    {
        transform.TryGetComponent(out dramaScript);
        animator = GetComponent<Animator>();
        player = GameObject.Find(Player.PlayerObject.Player.ToString()).transform.GetComponent<Player>();
        rb = GetComponent<Rigidbody>();
        getTalking = false;
        originLoc = ("Village", transform.position);
        threadDictionary = new Dictionary<NPCStatus, Task<string>>();
        chatBubble = new ChatBubble(transform.transform.Find("Text").GetComponent<TextMeshPro>(), transform.transform.Find("chatbubble").GetComponent<SpriteRenderer>());
    }


    // Update is called once per frame
    void Update()
    {
        Task<string> tmpTask;
        if (getTalking && chatBubble.chatAnim == false)
        {
            if (threadDictionary.TryGetValue(NPCStatus.Talking, out tmpTask))
            {
                if (tmpTask.IsCompleted)
                {
                    PendingChatResponse pendingChatRs = new PendingChatResponse();
                    if (tmpTask.Result != null)
                    {
                        Chat.Response assistantResponse = JsonUtility.FromJson<Chat.Response>(tmpTask.Result);
                        if (assistantResponse != null)
                        {
                            foreach (var row in assistantResponse.ResponseList)
                            {

                                if (NPC_Role == NPCRole.VillageChief)
                                {
                                    try
                                    {
                                        var rs = JsonUtility.FromJson<Chat.Movable_Response>(row);
                                        pendingChatRs.say = row;
                                        SetUpVoiceThread(pendingChatRs, rs.Say);
                                        if (rs.FollowUser == 1)
                                            player.teamMember.Add(this);
                                        else if (rs.FollowUser == -1)
                                            player.teamMember.Remove(this);
                                    }
                                    catch (System.Exception)
                                    {
                                        pendingChatRs.say = JsonUtility.ToJson(new Chat.Movable_Response { Attitude = 4, Say = row, FollowUser = 0 });
                                        SetUpVoiceThread(pendingChatRs, row);
                                    }

                                    pendingResponse.Add(pendingChatRs);
                                }
                                else if (NPC_Role == NPCRole.Sister)
                                {
                                    try
                                    {
                                        var rs = JsonUtility.FromJson<Chat.Sister_Response>(row);
                                        if (rs.SisterFindsComfortInUser)
                                            ++dramaScript.currentPoint;

                                        if (rs.SisterFollowsUser == 1)
                                        {
                                            animator.SetBool("isKickBoard", true);
                                            InTeam = true;
                                            target = player.transform;
                                            player.teamMember.Add(this);
                                        }
                                        else if (rs.SisterFollowsUser == -1) // go back to origin place
                                        {
                                            target = null;
                                            InTeam = false;
                                            player.teamMember.Remove(this);
                                        }
                                        pendingChatRs.say = row;
                                        SetUpVoiceThread(pendingChatRs, rs.Say);
                                    }
                                    catch (System.Exception)
                                    {
                                        pendingChatRs.say = JsonUtility.ToJson(new Chat.Sister_Response { Attitude = 4, Say = row, SisterFindsComfortInUser = false });
                                        SetUpVoiceThread(pendingChatRs, row);
                                    }

                                    pendingResponse.Add(pendingChatRs);
                                }
                                else if (NPC_Role == NPCRole.PresidingJudge)
                                {
                                    try
                                    {
                                        var rs = JsonUtility.FromJson<Chat.Normal_Response>(row);
                                        SetUpVoiceThread(pendingChatRs, rs.Say);
                                        pendingResponse.AddRange(SplitTextByPunctuation(rs.Say, rs.Attitude,NPC_Role.ToString()));
                                    }
                                    catch (System.Exception)
                                    {
                                        pendingChatRs.say = JsonUtility.ToJson(new Chat.Normal_Response { Attitude = 4, Say = row });
                                        SetUpVoiceThread(pendingChatRs, row);
                                    }
                                }
                                else
                                {
                                    try
                                    {
                                        var rs = JsonUtility.FromJson<Chat.Normal_Response>(row);
                                        SetUpVoiceThread(pendingChatRs,rs.Say);
                                        pendingChatRs.say = row;
                                    }
                                    catch (System.Exception)
                                    {
                                        pendingChatRs.say = JsonUtility.ToJson(new Chat.Normal_Response { Attitude = 4, Say = row });
                                        SetUpVoiceThread(pendingChatRs, row);
                                    }

                                    pendingResponse.Add(pendingChatRs);
                                }
                            }
                        }

                        if (pendingResponse.Count > 0)
                        {
                            var jsondata = JsonUtility.FromJson<Chat.Normal_Response>(pendingResponse[0].say);
                            SetUpChatBubble(jsondata, pendingResponse[0].audioThread);
                            pendingResponse.RemoveAt(0);
                            chatBubble.chatAnim = true;
                        }
                    }
                    else
                    {
                        pendingChatRs.say  = JsonUtility.ToJson(new Chat.Normal_Response { Attitude = 4, Say = "<<連線失敗>>" });
                        SetUpVoiceThread(pendingChatRs, "<<連線失敗>>");
                        pendingResponse.Add(pendingChatRs);
                        if (pendingResponse.Count > 0)
                        {
                            var jsondata = JsonUtility.FromJson<Chat.Normal_Response>(pendingResponse[0].say);
                            SetUpChatBubble(jsondata, pendingResponse[0].audioThread);
                            pendingResponse.RemoveAt(0);
                            chatBubble.chatAnim = true;
                        }
                    }


                    if (status == NPCStatus.Drama)
                        getTalking = false;
                    threadDictionary.Remove(NPCStatus.Talking);
                }
            }
            else if (pendingResponse.Count > 0)
            {
                var jsondata = JsonUtility.FromJson<Chat.Normal_Response>(pendingResponse[0].say);
                SetUpChatBubble(jsondata, pendingResponse[0].audioThread);
                pendingResponse.RemoveAt(0);
                chatBubble.chatAnim = true;
                loadAudio = false;
            }
        }
        else if (getTalking && chatBubble.chatAnim)
        {
            if ( pendingResponse.Count > 0 )
                chatBubble.ChatBubbleAnime(false);
            else
                chatBubble.ChatBubbleAnime(true);

            if (loadAudio == false && chatBubble.waitToLoadVoiceAudio)
            {
                loadAudio = true;
                StartCoroutine(chatBubble.LoadVoiceAudio(NPC_Role.ToString()));
            }
            if (chatBubble.chatAnim == false && pendingResponse.Count == 0)
            {
                getTalking = false;
                loadAudio = false;
            }
        }
    }

    private void SetUpChatBubble(Chat.Normal_Response jsondata, Task<string> voiceThread)
    {
        if (Enum.IsDefined(typeof(VoiceRole), NPC_Role.ToString()))
        {
            chatBubble.SetUpWithVoice(jsondata, voiceThread);
        }
        else
            chatBubble.SetUp(jsondata);
    }

    void FixedUpdate()
    {
        OnTheGround();
        if (target != null) 
        {
            if (InTeam)
                FollowTarget();
            else
                MoveToTarget();
        }
    }

    private void OnTheGround()
    {
        RaycastHit hit;
        Vector3 castPos = transform.position;
        castPos.y += 1;

        // check if penguin is on the ground
        if (Physics.Raycast(castPos, -transform.up, out hit, Mathf.Infinity, terrainMask))
        {
            // if on the ground, move forward
            Vector3 movePos = transform.position;
            movePos.y = hit.point.y + groundDistance;
            transform.position = movePos;
        }
    }

    private void FollowTarget()
    {
        if (target != null)
        {
            // 計算目標方向
            Vector3 distance = target.position - transform.position;
            if (distance.magnitude > 2)
            {
                Vector3 direction = distance.normalized;
                // 計算這一幀要移動的距離
                Vector3 moveAmount = direction * speed * Time.deltaTime;

                animator.SetFloat("move", Mathf.Abs(moveAmount.x) + Mathf.Abs(moveAmount.z));
                // 更新NPC的位置
                transform.position += moveAmount;
                float x = moveAmount.x;
                if (sr != null && x != 0 && x < 0)
                {
                    sr.flipX = true;
                }
                else if (sr != null && x != 0 && x > 0)
                {
                    sr.flipX = false;
                }
                // 面向目標方向
            }
            else
            {
                animator.SetFloat("move", 0f);
            }
        }
    }

    private void MoveToTarget()
    {
        if (target != null)
        {
            // 計算目標方向
            Vector3 distance = target.position - transform.position;
            if (distance.magnitude > 2)
            {
                Vector3 direction = distance.normalized;
                // 計算這一幀要移動的距離
                Vector3 moveAmount = direction * speed * Time.deltaTime;

                animator.SetFloat("move", Mathf.Abs(moveAmount.x) + Mathf.Abs(moveAmount.z));
                // 更新NPC的位置
                transform.position += moveAmount;
                float x = moveAmount.x;
                if (sr != null && x != 0 && x < 0)
                {
                    sr.flipX = true;
                }
                else if (sr != null && x != 0 && x > 0)
                {
                    sr.flipX = false;
                }
                // 面向目標方向
            }
            else
            {
                target = null;
                animator.SetFloat("move", 0f);
            }
        }
    }

    private void SetUpVoiceThread( PendingChatResponse pendingChat, string say )
    {
        if (Enum.IsDefined(typeof(VoiceRole), NPC_Role.ToString()))
        {
            pendingChat.audioThread = ChatBubble.TextToAudio(say, NPC_Role.ToString());
        }
    }

    public void AddPendingResponse(string chatFormat)
    {
        PendingChatResponse pendingChatResponse = new PendingChatResponse();
        Normal_Response chat = JsonUtility.FromJson<Normal_Response>(chatFormat);
        pendingChatResponse.say = chatFormat;
        if (Enum.IsDefined(typeof(VoiceRole), NPC_Role.ToString()))
        {
            pendingChatResponse.audioThread = ChatBubble.TextToAudio(chat.Say, NPC_Role.ToString());
        }

        pendingResponse.Add(pendingChatResponse);
    }
}
