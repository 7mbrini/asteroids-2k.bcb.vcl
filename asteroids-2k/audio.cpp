/*!****************************************************************************

	@file	audio.h
	@file	audio.cpp

	@brief	Audio manager

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <assert.h>

#include <string>
#include <fstream>
#include <sstream>

#include <al/al.h>
#include <al/alc.h>

#include "audio.h"
#include "utils.h"


//-----------------------------------------------------------------------------
#define DELTAVOLUME	0.05


/*!****************************************************************************
* @brief	Constructor
******************************************************************************/
TSoundManager::TSoundManager()
{
	TALSystem *pALSystem = new TALSystem();
	assert(pALSystem);

    m_pALSystem = pALSystem;
											// Get an OpenAL device
	pALSystem->pAlcDevice = alcOpenDevice(NULL);

	if (!pALSystem->pAlcDevice == NULL)
	{
											// Create a context
		pALSystem->pAlcContext = alcCreateContext(pALSystem->pAlcDevice,NULL);
											// Set active context
		alcMakeContextCurrent(pALSystem->pAlcContext);
											// Clear Error Code
		alGetError();
	}
	else
	{
		throw;
	}
}

/*!****************************************************************************
* @brief	Destructor
******************************************************************************/
TSoundManager::~TSoundManager()
{
	assert(m_pALSystem);
	assert(m_pALSystem->pAlcContext);

	alcDestroyContext(m_pALSystem->pAlcContext);
	alcCloseDevice(m_pALSystem->pAlcDevice);
}

/*!****************************************************************************
* @brief	Set master volume
* @param	Volume Value for volume, between 0..1
******************************************************************************/
void TSoundManager::SetMasterVolume(double Volume)
{
	if( Volume > 1.0 ) Volume = 1.0;
	if( Volume < 0 ) Volume = 0;

	alListenerf(AL_GAIN, Volume);
}

/*!****************************************************************************
* @brief	Get master volume
* @return	Value for the master volume, between 0..1
******************************************************************************/
double TSoundManager::GetMasterVolume()
{
	ALfloat Volume = 0;

	alGetListenerf(AL_GAIN, &Volume);

	return double(Volume);
}

/*!****************************************************************************
* @brief	Increases the master volume by a predefined step
******************************************************************************/
void TSoundManager::IncreaseMasterVolume()
{
	double Volume = GetMasterVolume();

	Volume += DELTAVOLUME;

	if(Volume > 1.0) Volume = 1.0;

	SetMasterVolume(Volume);
}

/*!****************************************************************************
* @brief	Decreases the master volume by a predefined step
******************************************************************************/
void TSoundManager::DecreaseMasterVolume()
{
	double Volume = GetMasterVolume();

	Volume -= DELTAVOLUME;

	if( Volume < 0 ) Volume = 0;

	SetMasterVolume(Volume);
}

/*!****************************************************************************
* @brief	Load a sound from file
* @param	strFileName Path to an audio file in PCM WAV format
* @return	Returns true for success, false otherwise
******************************************************************************/
bool TSoundManager::LoadTheSound(std::string strFileName)
{
	bool bResult = false;

	int nChannels=0, nSampleRate=0, nBps=0, nSize = 0;
	char *pData = LoadWAV(strFileName.c_str(), nChannels, nSampleRate, nBps, nSize);

/*
printf("\n\nFile Name: %s", strFileName.c_str());
printf("\nchannels: %d", nChannels);
printf("\nSampleRate: %d", nSampleRate);
printf("\nBps: %d", nBps);
printf("\nSamples per channel: %d", nSize);
*/

	if( pData )
	{
		ALuint nBufferId, nFormat;
		alGenBuffers(1, &nBufferId);

		if( nChannels == 1 )
		{
			if( nBps == 8 )
			{
				nFormat = AL_FORMAT_MONO8;
			}
			else
			{
				nFormat = AL_FORMAT_MONO16;
			}
		}
		else
		{
			if( nBps == 8 )
			{
				nFormat = AL_FORMAT_STEREO8;
			}
			else
			{
				nFormat = AL_FORMAT_STEREO16;
			}
		}

		alBufferData(nBufferId, nFormat, pData, nSize, nSampleRate);

		ALuint nSourceId;
		alGenSources(1, &nSourceId);

		alSourcei(nSourceId, AL_BUFFER, nBufferId);

		std::string strName = utils::GetFileName(strFileName, true);

        TSoundTrack SoundTrack(strName, nBufferId, nSourceId);

		m_SoundTracks.insert(make_pair(strName, SoundTrack));

		bResult = true;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Load sounds from a list of specified files
* @param	strSounds A list of sound filenames to be open
* @return	Returns true for success, false otherwise
******************************************************************************/
bool TSoundManager::LoadTheSounds(std::vector<std::string> strSounds)
{
	assert(strSounds.size());

	bool bResult = true;

	std::string strDataPath = utils::GetDataPath();

	for(int i=0; i<strSounds.size(); ++i)
	{
		std::string strWavFile = strDataPath + strSounds[i] + ".wav";

		if( !LoadTheSound(strWavFile) )
		{
			bResult = false;
			break;
		}
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Delete sound buffers and related resources
******************************************************************************/
void TSoundManager::FreeTheSounds()
{
	for( TMapSoundTracks::iterator Iter = m_SoundTracks.begin(); Iter != m_SoundTracks.end(); ++Iter)
	{
		alDeleteSources(1, &Iter->second.nSourceId);
	}
}

/*!****************************************************************************
* @brief	Play and audio track
* @param	strSound The sound track corresponding to string name
* @param	bLoop Flag for looping: true for playing the sound repeatedly
******************************************************************************/
void TSoundManager::PlayTheSound(std::string strSound, bool bLoop)
{
	if( m_SoundTracks.size() )
	{
        TMapSoundTracks::iterator it = m_SoundTracks.find(strSound);

		if( it != m_SoundTracks.end() )
		{
			TSoundTrack ST = it->second;

			alSourcei(ST.nSourceId, AL_LOOPING, bLoop);
			alSourcePlay(ST.nSourceId);
		}
	}
}

/*!****************************************************************************
* @brief	Stops to playing a sound source
* @param	strSound Sound source to be stopped
******************************************************************************/
void TSoundManager::StopTheSound(std::string strSound)
{
	if( m_SoundTracks.size() )
	{
        TMapSoundTracks::iterator it = m_SoundTracks.find(strSound);

        if( it != m_SoundTracks.end() )
		{
			alSourceStop(it->second.nSourceId);
		}
	}
}

/*!****************************************************************************
* @brief	Stops to playing all sound sources
******************************************************************************/
void TSoundManager::StopAllSounds()
{
    for( TMapSoundTracks::iterator it = m_SoundTracks.begin(); it != m_SoundTracks.end(); ++it)
	{
		alSourceStop(it->second.nSourceId);
	}
}

/*!****************************************************************************
* @brief	Load raw data from an audio file in WAV PCM format
* @param	strFileName The path to the file name to be open
* @param[in,out] nChannels The number of channels: 1 for mono, 2 for stereo
* @param[in,out] nSampleRate The Sample rate for audio data
* @param[in,out] nBps The bits per sample
* @param[in,out] nSize The size of raw audio buffer
* @return	The pointer to the raw audio buffer data
******************************************************************************/
char* TSoundManager::LoadWAV(std::string strFileName,
	int& nChannels, int& nSampleRate, int& nBps, int& nSize)
{
    char buffer[4];

    std::ifstream in(strFileName.c_str(), std::ios::binary);

    in.read(buffer, 4);

    if (strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cout << "this is not a valid WAV file" << std::endl;
        return NULL;
    }

    in.read(buffer, 4);
    in.read(buffer, 4);      // WAVE
    in.read(buffer, 4);      // fmt
    in.read(buffer, 4);      // 16
    in.read(buffer, 2);      // 1

    in.read(buffer, 2);
    nChannels = utils::ConvertToInt(buffer, 2);

    in.read(buffer, 4);
    nSampleRate = utils::ConvertToInt(buffer, 4);

    in.read(buffer, 4);
    in.read(buffer, 2);

    in.read(buffer, 2);
    nBps = utils::ConvertToInt(buffer, 2);

    in.read(buffer, 4);      // data
    in.read(buffer, 4);
    nSize = utils::ConvertToInt(buffer, 4);

    char* pData = new char[nSize];
	assert(pData);
    in.read(pData, nSize);

    return pData;
}


