/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include <al/al.h>
#include <al/alc.h>

#include <string>
#include <vector>
#include <map>


struct TALSystem
{
	ALCcontext *pAlcContext;
	ALCdevice *pAlcDevice;
};

class TSoundTrack
{
    public:
        TSoundTrack(const TSoundTrack& Other)
        {
            this->strName = Other.strName;
            this->nBufferId = Other.nBufferId;
            this->nSourceId = Other.nSourceId;
        }

        TSoundTrack(std::string strName, ALuint nBufferId, ALuint nSourceId)
        {
            this->strName = strName;
            this->nBufferId = nBufferId;
            this->nSourceId = nSourceId;
        }

    public:
        std::string strName;
        ALuint nBufferId, nSourceId;
};

typedef std::vector<TSoundTrack> TSoundTracks;
typedef std::map< std::string, TSoundTrack > TMapSoundTracks;

class TSoundManager
{
	public:
        TSoundManager();
        ~TSoundManager();

        double GetMasterVolume();
        void SetMasterVolume(double Volume);

        void IncreaseMasterVolume();
        void DecreaseMasterVolume();

        bool LoadTheSounds(std::vector<std::string> strSounds);
        void FreeTheSounds();
        bool LoadTheSound(std::string strFileName);
        void PlayTheSound(std::string strSound, bool bLoop = false);
        void StopTheSound(std::string strSound);
        void StopAllSounds();

    protected:
        TALSystem *m_pALSystem;
        TMapSoundTracks m_SoundTracks;

        char* LoadWAV(std::string strFileName, int& nChannels, int& nSampleRate, int& nBps, int& nSize);
};

#endif


