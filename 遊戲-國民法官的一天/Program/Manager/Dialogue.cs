using System;
using System.Collections.Generic;

[System.Serializable]
public class Dialogue
{
    public Dictionary<string, int> roleWithSentencesIndex;
    public string id; // 對話的唯一標識
    public List<string> sentences; // 對話內容
    public bool running;
    public string current_speaker;
    public string last_speaker;
    public bool begin;
    public bool end;


    public Dialogue(Dictionary<string, int> roleWithSentencesIndex)
    {
        id = null;
        this.roleWithSentencesIndex = roleWithSentencesIndex;
        last_speaker = null;
        current_speaker = "Judge";
        running = false;
        this.sentences = new List<string>();
        begin = true;
        end = false;
    }


    public string GetDialogueFromIndex( string role ) {
        string result = "";
        int index = -1;
        roleWithSentencesIndex.TryGetValue(role, out index);
        if (index > -1 )
            result = string.Join('\n',sentences.GetRange(index, sentences.Count - index));
        roleWithSentencesIndex[role] = sentences.Count;
        return result;
    }

    public string GetAllDialogue()
    {
        return string.Join('\n', sentences); 
    }

    public void AddResponse_To_Dialogue( string role, string response )
    {
        sentences.Add(role+":\"" + response + "\"");
        roleWithSentencesIndex[role] = sentences.Count;
    }
}
