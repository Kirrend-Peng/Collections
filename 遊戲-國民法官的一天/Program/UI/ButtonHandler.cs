using UnityEngine;

public class ButtonHandler : MonoBehaviour
{
    public string scriptName;
    private ScriptManager scriptManager;
    void Start()
    {
       scriptManager = GameObject.Find("GameManager/ScriptManager").GetComponent<ScriptManager>(); 
    }
    public void LoadBranchScript()
    {
        // 處理確認操作
        scriptManager.LoadBranchScript(scriptName);
    }
}
