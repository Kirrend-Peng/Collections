using System.Collections.Generic;
using UnityEngine;

public class DialogueManager : MonoBehaviour
{
    public static DialogueManager Instance; // 單例實例
    private Dictionary<string, Dialogue> dialogues = new Dictionary<string, Dialogue>(); // 存儲所有對話
    public Dialogue currentDialogue; // 當前對話

    void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            Destroy(gameObject);
        }
    }

    public void AddDialogue(Dialogue dialogue)
    {
        if (!dialogues.ContainsKey(dialogue.id))
        {
            dialogues.Add(dialogue.id, dialogue);
        }
    }

    public void StartDialogue(string id)
    {
        if (dialogues.TryGetValue(id, out Dialogue dialogue))
        {
            currentDialogue = dialogue;
            // 在這裡實現開始對話的邏輯，例如更新UI顯示對話內容
        }
    }

    // 其他方法，例如結束對話、獲取下一句對話等
}
