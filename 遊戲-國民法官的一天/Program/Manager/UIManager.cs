using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UIManager : MonoBehaviour
{
    private Animator animator;
    private Image introductionImage;
    private Image stageIntroImage;
    private TextMeshProUGUI introductionText;
    private TextMeshProUGUI introductionTitle;
    private TextMeshProUGUI stageIntroText;
    public Introduction[] introductionCollection;
    public StageIntro[] stageIntroCollection;
    public SelectionIntro[] selectionCollection;
    private TextMeshProUGUI selectionQuestionText;
    private ButtonHandler leftButton;
    private TextMeshProUGUI leftButtonText;
    private ButtonHandler rightButton;
    private TextMeshProUGUI rightButtonText;


    void Start()
    {
        animator = GetComponent<Animator>();
        introductionImage = GameObject.Find("UI/Introduction/Panel/Image").GetComponent<Image>();
        introductionText = GameObject.Find("UI/Introduction/Panel/Text").GetComponent<TextMeshProUGUI>();
        introductionTitle = GameObject.Find("UI/Introduction/Panel/Title").GetComponent<TextMeshProUGUI>();

        stageIntroImage = GameObject.Find("UI/StageIntro/Panel").GetComponent<Image>();
        stageIntroText = GameObject.Find("UI/StageIntro/Panel/Text").GetComponent<TextMeshProUGUI>();

        selectionQuestionText = GameObject.Find("UI/Selection/Panel/SelectionQuestion/Text").GetComponent<TextMeshProUGUI>();
        leftButton = GameObject.Find("UI/Selection/Panel/ButtonLeft").GetComponent<ButtonHandler>();
        leftButtonText = GameObject.Find("UI/Selection/Panel/ButtonLeft/Text").GetComponent<TextMeshProUGUI>();
        rightButton = GameObject.Find("UI/Selection/Panel/ButtonRight").GetComponent<ButtonHandler>();
        rightButtonText = GameObject.Find("UI/Selection/Panel/ButtonRight/Text").GetComponent<TextMeshProUGUI>();
    }


    [System.Serializable]
    public class Introduction
    {
        public string id;
        public Sprite image;
        public string text;
        public string title;
    }

    [System.Serializable]
    public class StageIntro
    {
        public string id;
        public Color color;
        public string text;
    }

    [System.Serializable]
    public class SelectionIntro
    {
        public string id;
        public string questionText;
        public string left_ButtonStatement;
        public string left_ScriptName;
        public string right_ButtonStatement;
        public string right_ScriptName;
    }

    #region --Selection--
    private SelectionIntro Find_SelectionIntro_ByID(string id)
    {
        return Array.Find(selectionCollection, item => item.id.Equals(id));
    }

    public void Set_SelectionIntro(string id)
    {
        SelectionIntro item = Find_SelectionIntro_ByID(id);
        selectionQuestionText.text = item.questionText;
        leftButtonText.text= item.left_ButtonStatement;
        leftButton.scriptName = item.left_ScriptName;
        rightButtonText.text = item.right_ButtonStatement;
        rightButton.scriptName = item.right_ScriptName;
    }

    public void Set_SelectionIntro_Appear()
    {
        animator.SetBool("SelectionSwitch", true);
    }

    public void Set_SelectionIntro_Disappear()
    {
        animator.SetBool("SelectionSwitch", false);
    }
    #endregion

    #region --Stage--
    private StageIntro FindStageIntroByID(string id)
    {
        return Array.Find(stageIntroCollection, item => item.id.Equals(id));
    }

    public void SetStageIntro(string id)
    {
        StageIntro item = FindStageIntroByID(id);
        stageIntroImage.color = item.color;
        stageIntroText.text = item.text;
    }

    public void SetStageIntroPushIn()
    {
        animator.SetBool("StageIntroSwitch", true);
    }

    public void SetStageIntroPushOut()
    {
        animator.SetBool("StageIntroSwitch", false);
    }
    #endregion

    #region-Introduction-
    private Introduction FindIntroductionByID(string id)
    {
        return Array.Find(introductionCollection, item => item.id.Equals(id));
    }

    public void SetIntroduction(string id) 
    { 
        Introduction item = FindIntroductionByID(id);
        introductionImage.sprite = item.image;
        introductionText.text = item.text;
        introductionTitle.text = item.title;
    }

    public void IntroductionFlyIn()
    {
        animator.SetBool("IntroductionSwitch", true);
    }

    public void IntroductionFlyOut()
    {
        animator.SetBool("IntroductionSwitch", false);
    }
    #endregion

    #region --
    #endregion
    // Start is called before the first frame update
    public void InDramaMode() 
    {
        animator.SetBool("Switch", true);
    }

    public void OutDramaMode()
    {
        animator.SetBool("Switch", false);
    }
}
