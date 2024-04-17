using System;
using UnityEngine;

public class AudioManager : MonoBehaviour
{
    public Music currentMusicPlayer;
    public AudioSource sfxPlayer;
    public Music[] MusicCollection;
    public AudioSource bgm;


    [System.Serializable]
    public class Music
    {
        public AudioClip musicClip;
        public string musicID;
    }

    public Music FindMusicByID(string musicID)
    {
        return Array.Find(MusicCollection, music => music.musicID.Equals(musicID));
    }
    void Start()
    {
        bgm = transform.Find("BGM").GetComponent<AudioSource>();
    }

    private void SetCurrentMusic(Music music)
    {
        bgm.clip = music.musicClip;
        currentMusicPlayer = music;
    }

    public void PlayMusic(Music musicClip)
    {
        SetCurrentMusic(musicClip);
        bgm.Stop();
        bgm.Play();
    }

    public void PlayMusic(string musicID)
    {
        if (musicID.Equals(currentMusicPlayer.musicID) == false)
        {
            Music tmp = FindMusicByID(musicID);
            if (tmp != null)
            {
                PlayMusic(tmp);
            }
        }
    }

    public void ContinueMusic()
    {
        bgm.Play();
    }

    public void PauseMusic()
    {
        bgm.Pause();
    }

    public void StopMusic()
    {
        bgm.Stop();
    }

    public void PlaySoundEffect(string musicID)
    {
        Music tmp = FindMusicByID(musicID);
        if (tmp != null)
        {
            sfxPlayer.PlayOneShot(tmp.musicClip);
        }
    }
}