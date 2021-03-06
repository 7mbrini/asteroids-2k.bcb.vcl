/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _GAME_H_
#define _GAME_H_

#include <vcl.h>
#pragma hdrstop

#include <windows.h>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>

#include "audio.h"
#include "video.h"

#include "ships.h"
#include "weapons.h"
#include "asteroids.h"


struct TRecordScore
{
	unsigned nScore;
	std::string strName;
};

typedef std::vector<std::string> TVecStrings;
typedef std::vector<TRecordScore> TVecRecordScores;

class TGame
{
	public:
    	TGame(TVideoManager* pVM, TSoundManager* pSM);
        ~TGame();

	public:
        void Run();

        bool IsPausing();
        bool IsRunning();
        bool IsGameOver();

        void Restart();
        void GameOver();
        void RunTheGame();
        void StopTheGame();
        void PauseTheGame();
        void PauseTheGame(bool bPause);

        void EndTheGame();

        HWND GetMainWnd();
        RECT GetClientArea();
        void GetClientArea(unsigned& nW, unsigned& nH);

        void ShotTheMissile(TShip* pShip);

        TVideoManager* GetVM() { return m_pVideo; }
        TSoundManager* GetSM() { return m_pAudio; }

		TShip* GetShip(enShipClass nShipClass) { return m_pShips[nShipClass]; }

    protected:
        TSoundManager* m_pAudio;
        TVideoManager* m_pVideo;

        TVecPtrShips m_pShips;
        TVecPtrWeapons m_pMissiles;
        TVecPtrAsteroids m_pAsteroids;
        bool m_bRun, m_bPause, m_bGameOver;
        int m_nScore, m_nLevel,
        	m_nDifficulty, m_nLives, m_nBonusCount;

        AnsiString m_strBestScoresName;
        TVecRecordScores m_BestScores;
        unsigned m_nDlgRetVal;

        TVecStrings m_strHelp;

	protected:

		void Setup();

        void ShowInfo();
        void NextLevel();

		void Split(TAsteroid* pAsteroid, TVecPtrAsteroids& Splits);

        bool IsSafetyPos(TVector2 Pos);

        bool LoadTheHelp();
        bool LoadTheSounds();

        void AddScore(int nScore);
        void AddScore(TAsteroid* pAsteroid);

        bool IsBestScore();
        void RegisterBestScore();
        void SaveBestScores();
        bool LoadTheBestScores(char* pFileName);

        bool BuildTheFonts();
        void BuildTheAsteroids(unsigned nCount);

        bool BuildTheShips();

        void ForceInsideLimits();
        bool IsInsideGameArea(TVector2 Pos);

        void LevelHandler();
        void BonusHandler();
        void CollisionHandler();

		bool Collide(TShip* pShip1, TShip* pShip2);
        bool Collide(TMissile* pMissile, TShip* pShip);
        bool Collide(TAsteroid* pAsteroid, TShip* pShip);

        void GameOverHandler();
		void HumanShipsHandler();
        void AlienShipsHandler();

        void Sort(TVecRecordScores& Scores, bool bAscending=true);

		void Clear(TVecPtrShips& Ships);
        void Clear(TVecPtrWeapons& Missiles);
        void Clear(TVecPtrAsteroids& Asteroids);

};

#endif


