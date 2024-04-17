// unity 3d game programming
using System.Collections.Generic;
using System.Threading.Tasks;
using TMPro;
using UnityEngine;


public class Player : MonoBehaviour
{
    public Dictionary<PlayerStatus, Task<string>> threadDictionary;
    private Dictionary<string, NPC> nearNPCs;
    public List<string> pendingResponse;
    public float speed;
    public float groundDistance;
    public Rigidbody rb;
    public LayerMask terrainMask;
    public SpriteRenderer sr;
    public Transform target; // 目標位置
    Animator myanim;
    public ChatBubble chatBubble;
    public bool getTalking;
    public List<NPC> teamMember;
    public Dictionary<string, int> trustValueDictionary;
    public PlayerStatus player_status;
    private ScriptManager scriptManager;

    public enum PlayerObject
    {
        Player
    }

    public enum PlayerStatus
    {
        Idle,       // 空閑
        Walking,    // 行走
        Talking,    // 對話
        Attacking,  // 攻擊
        Dead,       // 死亡
        Drama
    }
    void Start()
    {
        scriptManager = GameObject.Find("GameManager").transform.Find("ScriptManager").GetComponent<ScriptManager>();
        trustValueDictionary = UtilityClass.InitialNPCTrustValue();  // 之後改為先從檔案讀取
        teamMember = new List<NPC>();
        getTalking = false;
        threadDictionary = new Dictionary<PlayerStatus, Task<string>>();
        nearNPCs = new Dictionary<string, NPC>();
        player_status = PlayerStatus.Idle;
        rb = GetComponent<Rigidbody>();
        myanim = GetComponent<Animator>();
        chatBubble = new ChatBubble(transform.Find("Text").GetComponent<TextMeshPro>(), transform.Find("chatbubble").GetComponent<SpriteRenderer>());
    }

    void Update()
    {
        Task<string> tmpTask;
        if (getTalking && chatBubble.chatAnim == false)
        {
            if (threadDictionary.TryGetValue(PlayerStatus.Talking, out tmpTask))
            {
                if (tmpTask.IsCompleted)
                {
                    if (tmpTask.Result != null)
                    {
                        UtilityClass.SpeechToTextOutput audioResponse = JsonUtility.FromJson<UtilityClass.SpeechToTextOutput>(tmpTask.Result);
                        foreach (var npc in nearNPCs.Values)
                        {
                            if (npc.status != NPC.NPCStatus.Drama && npc.chatBubble.chatAnim == false && npc.threadDictionary.ContainsKey(NPC.NPCStatus.Talking) == false)
                            {
                                npc.getTalking = true;
                                int trustValue = trustValueDictionary[npc.NPC_Role.ToString()];
                                npc.threadDictionary.Add(NPC.NPCStatus.Talking, ChatBubble.GetChatContentAsync(npc.NPC_Role.ToString(), UtilityClass.Combine_Role_TrustValue_Sentence("User", trustValue, audioResponse.text)));
                            }
                        }

                        pendingResponse.Add(JsonUtility.ToJson(new Chat.Normal_Response { Attitude = 4, Say = audioResponse.text }));
                        if (pendingResponse.Count > 0)
                        {
                            var jsondata = JsonUtility.FromJson<Chat.Normal_Response>(pendingResponse[0]);
                            chatBubble.SetUp(jsondata);
                            pendingResponse.RemoveAt(0);
                            chatBubble.chatAnim = true;
                        }
                    }
                    else 
                    {
                        pendingResponse.Add(JsonUtility.ToJson(new Chat.Normal_Response { Attitude = 4, Say = "<<連線失敗>>" }));
                        if (pendingResponse.Count > 0)
                        {
                            var jsondata = JsonUtility.FromJson<Chat.Normal_Response>(pendingResponse[0]);
                            chatBubble.SetUp(jsondata);
                            pendingResponse.RemoveAt(0);
                            chatBubble.chatAnim = true;
                        }
                    }

                    if (player_status == PlayerStatus.Drama)
                        getTalking = false;
                    threadDictionary.Remove(PlayerStatus.Talking);
                }
            }
            else if (pendingResponse.Count > 0)
            {
                var jsondata = JsonUtility.FromJson<Chat.Normal_Response>(pendingResponse[0]);
                chatBubble.SetUp(jsondata);
                pendingResponse.RemoveAt(0);
                chatBubble.chatAnim = true;
            }
        }
        else if (getTalking && chatBubble.chatAnim)
        {
            if (pendingResponse.Count > 0)
                chatBubble.ChatBubbleAnime(false);
            else
                chatBubble.ChatBubbleAnime(true);

            if (chatBubble.chatAnim == false && pendingResponse.Count == 0)
            {
                getTalking = false;
            }
        }
    }

    void FixedUpdate()
    {
        if (player_status != PlayerStatus.Drama)
        {
            PlayerInteractAsync();
            // set light intensity

            float x = Input.GetAxis("Horizontal");
            float z = Input.GetAxis("Vertical");

            myanim.SetFloat("move", Mathf.Abs(x) + Mathf.Abs(z));
            Vector3 moveDir = new Vector3(x, 0, z);
            rb.velocity = moveDir * speed;

            if (x != 0 && x < 0)
            {
                sr.flipX = true;
            }
            else if (x != 0 && x > 0)
            {
                sr.flipX = false;
            }
        }

        OnTheGround();
        if (target != null)
            MoveToTarget();
    }

    private void PlayerInteractAsync()
    {
        float interactRange = 2f;
        Collider[] colliders = Physics.OverlapSphere(transform.position, interactRange);
        nearNPCs.Clear();
        foreach (Collider collider in colliders)
        {
            if (collider.TryGetComponent(out NPC npc_component))
            {
                nearNPCs.Add(npc_component.name, npc_component);
            }

            if (scriptManager.lockDrama == false)
            {
                if (collider.TryGetComponent(out DramaScript dramascript_component))
                {
                    if (dramascript_component.playOnce && dramascript_component.havePlayed == false && dramascript_component.currentPoint == dramascript_component.triggerPoint)
                    {
                        dramascript_component.havePlayed = true;
                        scriptManager.LoadScript(dramascript_component.dramaScriptName);
                    }
                    else if (dramascript_component.playOnce == false && dramascript_component.currentPoint == dramascript_component.triggerPoint)
                        scriptManager.LoadScript(dramascript_component.dramaScriptName);
                }
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

                myanim.SetFloat("move", Mathf.Abs(moveAmount.x) + Mathf.Abs(moveAmount.z));
                // 更新NPC的位置
                transform.position += moveAmount;
                float x = moveAmount.x;
                if (x != 0 && x < 0)
                {
                    sr.flipX = true;
                }
                else if (x != 0 && x > 0)
                {
                    sr.flipX = false;
                }
                // 面向目標方向
            }
            else
            {
                target = null;
                myanim.SetFloat("move", 0f);
            }
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
}