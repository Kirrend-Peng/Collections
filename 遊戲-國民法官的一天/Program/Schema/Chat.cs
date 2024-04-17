using System.Threading.Tasks;

[System.Serializable]
public class Chat
{

    [System.Serializable]
    public class Normal_Response
    {
        public int Attitude;
        public string Say;
    }

    [System.Serializable]
    public class Movable_Response : Normal_Response
    {
        public int FollowUser;
    }

    [System.Serializable]
    public class Judge_Response : Normal_Response
    {
        public float SpeakSpeed;
        public string SpeakTo;
        public int Judge;
    }

    [System.Serializable]
    public class Sister_Response : Normal_Response
    {
        public bool SisterFindsComfortInUser;
        public int SisterFollowsUser;
    }

    [System.Serializable]
    public class Request
    {
        public string AssistantName;
        public string Input;
    }

    [System.Serializable]
    public class Response
    {
        public string AssistantName;
        public string[] ResponseList;
    }

    [System.Serializable]
    public class PendingChatResponse
    {
        public string say;
        public Task<string> audioThread;

        public PendingChatResponse(string say, Task<string> thread)
        {
            this.say = say;
            this.audioThread = thread;
        }

        public PendingChatResponse()
        {
            this.say = null;
            this.audioThread = null;
        }
    }
}