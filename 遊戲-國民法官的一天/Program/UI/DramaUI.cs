using UnityEngine;

public class DramaUI : MonoBehaviour
{
    private Animator animator;
    // Start is called before the first frame update
    void Start()
    {
        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void ResetAnimationTrigger()
    {
        // 假設控制動畫的bool變數名為IsVisible
        animator.SetInteger("Switch", 0);
    }
}
