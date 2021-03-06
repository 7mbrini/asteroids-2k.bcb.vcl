/*!****************************************************************************

	@file	game.h
	@file	game.cpp

	@brief	Game logic implementation

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <vcl.h>
#pragma hdrsop

#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>

#include <locale>
#include <string>

#include <algorithm>
									// openal audio engine
#include <al/al.h>
#include <al/alc.h>

#include "audio.h"
#include "video.h"
#include "game.h"
#include "utils.h"
#include "vectors.h"
#include "commdefs.h"

#include "TFormMain.h"
#include "TDlgBestScores.h"

//-----------------------------------------------------------------------------

#define MAXLIVES		3
#define STARTLEVEL		1
#define STARTSCORE		0
#define BONUSCOUNTER	1
#define BONUSPOINTS		1000

#define SHOTDELAY		150
#define MAXASTEROIDS	5

#define ASTEROIDVEL			10
#define ASTEROIDVELRATIO	2
#define ASTEROIDBIGSIZE		30
#define ASTEROIDMIDSIZE 	20
#define ASTEROIDSMALLSIZE	10

#define BIGASTEROIDSCORE	5
#define MIDASTEROIDSCORE    10
#define SMALLASTEROIDSCORE	20

#define BIGALIENSHIPSCORE	100
#define SMALLALIENSHIPSCORE	500

#define SAFETYDISTANCE		(2.0*ASTEROIDBIGSIZE)

#define SPLASHDELAY			5000

#define ALIENSHOTDELAY		20
#define HUMANSHOTDELAY		100

#define MISSILESPEED		100.0

#define ALIENSHIPTICK		500

#define ALIENBIGINACCURACY		(M_PI/16.0)
#define ALIENSMALLINACCURACY	(M_PI/64.0)


//#define _DEVEL


/*!****************************************************************************
* @brief	Constructor
* @param	pVM Pointer to the VideoManager
* @param	pSM Pointer to the SoundManager
******************************************************************************/
TGame::TGame(TVideoManager* pVM, TSoundManager* pSM)
{
	assert(pVM);
	assert(pSM);

	m_pVideo = pVM;
	m_pAudio = pSM;

	m_bRun = true;
	m_bPause = false;
	m_nLives = MAXLIVES;
	m_nScore = STARTSCORE;
	m_nLevel = STARTLEVEL;
	m_nBonusCount = BONUSCOUNTER;

    Setup();
}

/*!****************************************************************************
* @brief	Settings up the game engine
******************************************************************************/
void TGame::Setup()
{
	if( !BuildTheFonts() )
	{
		::MessageBox(0,
			TEXT("Error: Cannot find FONTS data file!\n\nPress any key to exit ..."),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONSTOP | MB_TASKMODAL);

        throw;
	}

	if( !BuildTheShips() )
	{
		::MessageBox(0,
			TEXT("Error: Cannot build the ships!\n\nPress any key to exit ..."),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONSTOP | MB_TASKMODAL);

        throw;
	}

	if( !LoadTheSounds() )
	{
		::MessageBox(0,
			TEXT("Error: Cannot load the sounds!"),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONERROR | MB_TASKMODAL);

        throw;
	}

	if( !LoadTheHelp() )
	{
		::MessageBox(0,
			TEXT("Error: Cannot open the help file!"),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONERROR | MB_TASKMODAL);

		throw;
	}

	std::string strScoresFile = std::string(utils::GetDataPath()) + std::string(SCORESFILE);
	if( !LoadTheBestScores((char*) strScoresFile.c_str()) )
	{
		::MessageBox(0,
			TEXT("Error: Cannot open SCORES data file!\n\nPress any key to exit ..."),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONSTOP | MB_TASKMODAL);

        throw;
	}

#ifdef _DEVEL
	BuildTheAsteroids(1);
#else
	BuildTheAsteroids(m_nLevel * MAXASTEROIDS );
#endif
}

/*!****************************************************************************
* @brief	Destructor
******************************************************************************/
TGame::~TGame()
{
    Clear(m_pShips);
    Clear(m_pMissiles);
    Clear(m_pAsteroids);

	m_pAudio->FreeTheSounds();
}

/*!****************************************************************************
* @brief	Loads the sounds
* @return	Returns true for success, false otherwise
******************************************************************************/
bool TGame::LoadTheSounds()
{
	assert(m_pAudio);

	std::string strArray[] = {
		"bonus",
		"shield",
		"ship_fire",
		"bang_large",
		"bang_medium",
		"bang_small",
		"saucer_big",
		"saucer_small",
		"ship_thrust",
		"ship_explosion",
		"starwars-trails"
	};

    std::vector<std::string> strSounds;

    for(int i=0; i<sizeof(strArray)/sizeof(strArray[0]); i++)
    {
		strSounds.push_back(strArray[i]);
    }

	return GetSM()->LoadTheSounds(strSounds);
}

/*!****************************************************************************
* @brief	Loads the help strings information
* @return	Returns true for success, false otherwise
******************************************************************************/
bool TGame::LoadTheHelp()
{
	bool bResult = false;

	std::string strHelpFile = utils::GetDataPath() + std::string(HELPFILE);

	FILE* fp = fopen(strHelpFile.c_str(), "r");

	if( fp )
	{
		m_strHelp.clear();

		while( !feof(fp) )
		{
			char Buffer[1024];

			fgets(Buffer, sizeof(Buffer) - 1, fp);

			m_strHelp.push_back(Buffer);
		}

		fclose(fp);

		bResult = true;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Load the best scores from file
* @param	pFileName Pointer to the filename string
* @return	Returns true for success, false otherwise
******************************************************************************/
bool TGame::LoadTheBestScores(char* pFileName)
{
	assert(pFileName);

	bool bResult = false;

	FILE *fp = fopen(pFileName, "r");

	if( fp )
	{
		m_BestScores.clear();

		while( !feof(fp) )
		{
			char Buffer[1024];

			if( fgets(Buffer, sizeof(Buffer)-1, fp) )
			{
				TRecordScore Score;
				memset(&Score, 0, sizeof(Score));

				std::string strTemp(Buffer);
				int nPos = strTemp.find(",");

				Score.strName = strTemp.substr(0, nPos);

				std::string strScore = strTemp.substr(nPos+1, strTemp.length());
				Score.nScore = atoi((char*) strScore.c_str());

				m_BestScores.push_back(Score);
			}
		}

		fclose(fp);

		Sort(m_BestScores, false);

		bResult = true;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Builds the fonts
* @return	Returns true for success, false otherwise
******************************************************************************/
bool TGame::BuildTheFonts()
{
	assert(m_pVideo);

	std::wstring strFontName = L"Techno LCD";
	std::string strFontPath = utils::GetDataPath() + "\\technolcd.ttf";

	return m_pVideo->LoadFont(strFontPath, strFontName, FONTSIZE);
}

/*!****************************************************************************
* @brief	Set all parameters to their default values and restart the game
******************************************************************************/
void TGame::Restart()
{
	assert(m_pAudio);
	assert(m_pShips.size());

    Clear(m_pMissiles);
	Clear(m_pAsteroids);

	m_pAudio->StopAllSounds();

	unsigned nWidth, nHeight;
	GetClientArea(nWidth, nHeight);

	for(int i=0; i<m_pShips.size(); ++i)
	{
		m_pShips[i]->Reset();

		if( m_pShips[i]->GetClass() == scHuman )
		{
			m_pShips[i]->SetVel(TVector2(0,0));
			m_pShips[i]->SetRot(180.0);
			m_pShips[i]->SetPos(TVector2(nWidth/2.0, nHeight/2.0) );

			m_pShips[i]->SetAlive(true);
			m_pShips[i]->SetVisible(true);
		}
		else
		{
			m_pShips[i]->SetVel(TVector2(0,0) );
			m_pShips[i]->SetRot(0);
			m_pShips[i]->SetPos(TVector2( -100, -100 ) );

			m_pShips[i]->SetAlive(false);
			m_pShips[i]->SetVisible(false);
		}
	}

	m_nLives = MAXLIVES;
	m_nLevel = STARTLEVEL;
	m_nScore = STARTSCORE;
	m_nBonusCount = BONUSCOUNTER;
	m_bGameOver = false;

#ifdef _DEVEL
	BuildTheAsteroids(1);
#else
	BuildTheAsteroids(m_nLevel * MAXASTEROIDS);
#endif
}

/*!****************************************************************************
* @brief	Terminates the game
******************************************************************************/
void TGame::GameOver()
{
	m_bRun = true;
	m_bGameOver = true;

	for(int i=0; i<m_pShips.size(); ++i)
	{
		m_pShips[i]->SetVisible(false);
		m_pShips[i]->SetAlive(false);
	}

#ifndef _DEVEL
	m_pAudio->PlayTheSound("starwars-trails", true);
#endif
}

/*!****************************************************************************
* @brief	Advances the game to next level
******************************************************************************/
void TGame::NextLevel()
{
	Clear(m_pMissiles);
    Clear(m_pAsteroids);

	m_nLevel++;

#ifdef _DEVEL
	unsigned nCount = m_nLevel;
#else
	unsigned nCount = m_nLevel * MAXASTEROIDS;
#endif

	BuildTheAsteroids(nCount);
}

/*!****************************************************************************
* @brief	Deletes a bunch of weapons referenced by a vector of pointers
* @param	Weapons Referernce to a list of pointers to weapons
******************************************************************************/
void TGame::Clear(TVecPtrWeapons& Weapons)
{
	for(int i=0; i<Weapons.size(); i++)
	{
		if( Weapons[i] ) delete Weapons[i];
	}

	Weapons.clear();
}

/*!****************************************************************************
* @brief	Deletes a bunch of asteroids referenced by a vector of pointers
* @param	Asteroids Referernce to a list of pointers to asteroids
******************************************************************************/
void TGame::Clear(TVecPtrAsteroids& Asteroids)
{
	for(int i=0; i<Asteroids.size(); i++)
    {
    	if( Asteroids[i] ) delete Asteroids[i];
    }

    Asteroids.clear();
}

/*!****************************************************************************
* @brief	Deletes a bunch of ships referenced by a vector of pointers
* @param	Ships Referernce to a list of pointers to ships
******************************************************************************/
void TGame::Clear(TVecPtrShips& Ships)
{
	for(int i=0; i<Ships.size(); i++)
    {
    	if( Ships[i] ) delete Ships[i];
    }

    Ships.clear();
}

/*!****************************************************************************
* @brief	Builds the ships
* @return	Returns true for success, false otherwise
******************************************************************************/
bool TGame::BuildTheShips()
{
	bool bResult = true;
											// build the human ship
	{
		TShip *pShip = new TShip(
			m_pVideo,
			m_pAudio,
			scHuman,
			TVector2 ( SHIP_SIZE, SHIP_SIZE ),
			TVector2 ( FRAMEW/2, FRAMEH/2 ),
			TVector2 ( 0, 0 ) );

		assert(pShip);

		pShip->SetRot(180.0);
		pShip->SetAlive(true);
		pShip->SetVisible(true);
		pShip->SetColor(RGB(255,255,255));

		m_pShips.push_back(pShip);
	}
											// build the small alien ship
	{
		TShip *pShip = new TShip(
			m_pVideo,
			m_pAudio,
			scAlienSmall,
			TVector2 ( SHIP_SIZE, SHIP_SIZE ),
			TVector2 ( -100, -100 ),
			TVector2 ( 0, 0 ) );

		assert(pShip);

		pShip->SetAlive(true);
		pShip->SetVisible(false);
		pShip->SetColor(RGB(255,255,255));

		m_pShips.push_back(pShip);
	}
											// build the big alien ship
	{
		TShip *pShip = new TShip(
			m_pVideo,
			m_pAudio,
			scAlienBig,
			TVector2 (1.5*SHIP_SIZE, 1.5*SHIP_SIZE),
			TVector2 ( -100, -100 ),
			TVector2 ( 0, 0 ) );

		assert(pShip);

		pShip->SetAlive(true);
		pShip->SetVisible(false);
		pShip->SetColor(RGB(255,255,255));

		m_pShips.push_back(pShip);
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Checks status for pausing
* @return	Returns true if game is in "pause" mode, false otherwise
******************************************************************************/
bool TGame::IsPausing()
{
	return m_bPause;
}

/*!****************************************************************************
* @brief	Checks status for running
* @return	Returns true if game is in "run" mode, false otherwise
******************************************************************************/
bool TGame::IsRunning()
{
	return m_bRun;
}

/*!****************************************************************************
* @brief	Shots the missile
* @param	pShip Pointer to the ship object
******************************************************************************/
void TGame::ShotTheMissile(TShip* pShip)
{
	assert(pShip);
											// delay, in milliseconds,
											// between sequential shots
	unsigned nTickDelay = HUMANSHOTDELAY;

	static unsigned nCurTick = nTickDelay;

	if( (::GetTickCount() - nCurTick ) >= nTickDelay)
	{
		nCurTick = ::GetTickCount();

		m_pAudio->PlayTheSound("ship_fire");

		TMissile *pMissile = new TMissile(m_pVideo);
		assert(pMissile);

		pMissile->SetShip(pShip);

		m_pMissiles.push_back(pMissile);
													// nel caso dell'astronave "umana" spara
													// il missile lungo la direzione della prua
		if( pShip->GetClass() == scHuman )
		{
			double Rot = pShip->GetRot();
			double Mod = MISSILESPEED;

			TVector2 Vel( Mod*cos(DEG2RAD(Rot-90.0)), Mod*sin(DEG2RAD(Rot+90.0)) );

			TVector2 Pos = pShip->GetPos();
			TVector2 ShipVel = pShip->GetVel();

			pMissile->Arm(Pos, Add( Vel, ShipVel) );
		}
	}
}

/*!****************************************************************************
* @brief	Inhibits game status for running
******************************************************************************/
void TGame::EndTheGame()
{
	m_bRun = false;
}

/*!****************************************************************************
* @brief	Pausing the game in toggle mode
******************************************************************************/
void TGame::PauseTheGame()
{
	m_bPause = !m_bPause;
}

/*!****************************************************************************
* @brief	Pausing the game
* @param	bPause True for pausing the game, false otherwise
******************************************************************************/
void TGame::PauseTheGame(bool bPause)
{
	m_bPause = bPause;
}

/*!****************************************************************************
* @brief	Shows game information
******************************************************************************/
void TGame::ShowInfo()
{
	assert(m_pVideo);

	unsigned nW, nH;
	GetClientArea(nW, nH);

	char Buffer[256];
	sprintf(Buffer, "Ships: %d", m_nLives < 0 ? 0 : m_nLives);
	m_pVideo->DrawText(Buffer, 96, 16);

	sprintf(Buffer, "Level: %d", m_nLevel);
	m_pVideo->DrawText(Buffer, nW/2.0, 16);

	sprintf(Buffer, "Score: %d", m_nScore);
	m_pVideo->DrawText(Buffer, nW - 96, 16);
}

/*!****************************************************************************
* @brief	Builds the asteroids
* @param	nCount Number of asteroids to be created
******************************************************************************/
void TGame::BuildTheAsteroids(unsigned nCount)
{
	assert(m_pAudio);
	assert(m_pVideo);
												// rebuild the asteroid's list
	RECT ClientArea = m_pVideo->GetClientArea();

	for(int i=0; i<nCount; i++)
	{
		TVector2 Pos( maths::AbsRand(ClientArea.right), maths::AbsRand(ClientArea.bottom) );
		TVector2 Vel ( maths::Rand(ASTEROIDVEL) + ASTEROIDVEL/5.0, maths::Rand(ASTEROIDVEL) + ASTEROIDVEL/5.0 );

		TAsteroid *pAsteroid = new TAsteroid(
        	m_pVideo, m_pAudio, acBig, Pos, Vel,
            ASTEROIDBIGSIZE + maths::AbsRand(ASTEROIDBIGSIZE/10.0) );
		assert(pAsteroid);

		m_pAsteroids.push_back(pAsteroid);
	}
}

/*!****************************************************************************
* @brief	Checks if given position are out of the scenario boundaries
* @param	Pos Position to be tested to for coordinates
* @return	Returns true if Pos is inside limits, false otherwise
******************************************************************************/
bool TGame::IsInsideGameArea(TVector2 Pos)
{
	unsigned nWidth, nHeight;
	GetClientArea(nWidth, nHeight);

	return bool( (Pos.X>=0) && (Pos.X <= nWidth) && (Pos.Y >=0 ) && (Pos.Y <= nHeight) );
}

/*!****************************************************************************
* @brief	Force all actors inside of the scenario boundaries
******************************************************************************/
void TGame::ForceInsideLimits()
{
	assert(m_pVideo);
											// force ships inside the scenery limits
	unsigned nWidth, nHeight;
	GetClientArea(nWidth, nHeight);

	for(int i=0; i<m_pShips.size(); ++i)
	{
		TVector2 Pos = m_pShips[i]->GetPos();

		if( !IsInsideGameArea(Pos) )
		{
			if( m_pShips[i]->GetClass() == scHuman )
			{

				if( Pos.X < 0 ) Pos.X = nWidth;
				if( Pos.X > nWidth ) Pos.X = 0;
				if( Pos.Y < 0 ) Pos.Y = nHeight;
				if( Pos.Y > nHeight ) Pos.Y = 0;

				m_pShips[i]->SetPos(Pos);
			}
			else
			{
				if( Pos.X >=0 && Pos.X <= nWidth )
				{
					if( Pos.Y < 0 ) Pos.Y = nHeight;
					if( Pos.Y > nHeight ) Pos.Y = 0;

					m_pShips[i]->SetPos(Pos);
				}
				else
				{
					m_pShips[i]->SetVisible(false);
					m_pShips[i]->SetPos(TVector2 ( -100, -100 ) );
				}
			}
		}
	}
											// force asteroids inside the scenery limits
	for(int i=0; i<m_pAsteroids.size(); i++)
	{
		assert(m_pAsteroids[i]);

        if( m_pAsteroids[i]->IsAlive() )
		{
			TVector2 Pos = m_pAsteroids[i]->GetPos();

			if( Pos.X < 0 ) Pos.X = nWidth;
			if( Pos.X > nWidth ) Pos.X = 0;
			if( Pos.Y < 0 ) Pos.Y = nHeight;
			if( Pos.Y > nHeight ) Pos.Y = 0;

			m_pAsteroids[i]->SetPos(Pos);
		}
	}
}

/*!****************************************************************************
* @brief	Handles the transition to next game level
******************************************************************************/
void TGame::LevelHandler()
{
	bool bLevelCompleted = true;

	for( int i=0; i<m_pAsteroids.size(); i++)
	{
		assert(m_pAsteroids[i]);

        if( m_pAsteroids[i]->IsAlive() )
        {
            bLevelCompleted = false;
            break;
        }
	}
											// se non ci sono piu` asteroidi ...
	if( bLevelCompleted )
	{
		NextLevel();
	}
}

/*!****************************************************************************
* @brief	Handles the "bonus" event
******************************************************************************/
void TGame::BonusHandler()
{
	if( m_nScore >= (BONUSPOINTS * m_nBonusCount) )
	{
		m_nLives++;
		m_nBonusCount++;

		m_pAudio->PlayTheSound("bonus");
	}
}

/*!****************************************************************************
* @brief	Checks for collision between two ships
******************************************************************************/
bool TGame::Collide(TShip* pShip1, TShip* pShip2)
{
	assert(pShip1);
    assert(pShip2);

    return bool(
    	Distance(pShip1->GetPos(), pShip2->GetPos())
    		<= ( pShip1->GetSize().Length() + pShip2->GetSize().Length()) / 2.0);

}

/*!****************************************************************************
* @brief	Collision detection between ships and asteroids
* @param	pAsteroid Pointer to the asteroid object
* @param	pShip Pointer to the ship object
* @return	Returns true if objects collides, false otherwise
******************************************************************************/
bool TGame::Collide(TAsteroid* pAsteroid, TShip* pShip)
{
	return bool( Distance(pShip->GetPos(), pAsteroid->GetPos())
    	<= ( pAsteroid->GetSize() + pShip->GetSize().X / 2.0));
}

/*!****************************************************************************
* @brief	Previene di piazzare "a tradimento" l'astronave
*			ossia nel bel mezzo di una pioggia di meteoriti!
* @param	Pos Position to want to check
* @return	Returns true if area around specified position is
			free from meteorites, false otherwise
******************************************************************************/
bool TGame::IsSafetyPos(TVector2 Pos)
{
	bool bResult = true;

	for(int i=0; i<m_pAsteroids.size(); ++i)
	{
		TAsteroid* pRoid = m_pAsteroids[i];
		assert(pRoid);

		if( pRoid->IsAlive() )
		{
			if( Distance( pRoid->GetPos(), Pos) <= SAFETYDISTANCE)
			{
				bResult = false;
				break;
			}
		}
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Handles the "game-over" status
******************************************************************************/
void TGame::GameOverHandler()
{
	assert(m_pVideo);

	static int nCounter = 0;
	unsigned nTickDelay = SPLASHDELAY;

	static unsigned nCurTick = nTickDelay;

	TVector2 ScreenCenter = m_pVideo->GetScreenCenter();

											// heading for best scores
    std::vector<std::string> strBestScores;
	{
        int nItems = std::min( (int) BESTSCORES, (int) m_BestScores.size() );

        strBestScores.push_back("Best Scores:");
        strBestScores.push_back(" ");
        strBestScores.push_back(" ");

        for(int i=0; i<nItems; i++)
        {
            char Buffer[256];

            sprintf(Buffer, "%s   %d",
                (char*)m_BestScores[i].strName.c_str(),
                m_BestScores[i].nScore
            );

            strBestScores.push_back(Buffer);
        }
	}

	if( (::GetTickCount() - nCurTick ) >= nTickDelay)
	{
		nCurTick = ::GetTickCount();

		nCounter++;
		if( nCounter > 2 ) nCounter = 0;
	}

	switch( nCounter )
	{
		case 0:
			m_pVideo->DrawText((char*)"Game Over", ScreenCenter.X, ScreenCenter.Y);
		break;

		case 1:
			m_pVideo->DrawText(m_strHelp, ScreenCenter.X, 128, FONTSIZE);
		break;

		case 2:
			m_pVideo->DrawText(strBestScores, ScreenCenter.X, 128, FONTSIZE);
		break;
	}
}

/*!****************************************************************************
* @brief	Checks for "game-over" status
* @return	If the game is in "game-over" status return true, false otherwise
******************************************************************************/
bool TGame::IsGameOver()
{
	return m_bGameOver;
}

/*!****************************************************************************
* @brief	Handles the human ships
******************************************************************************/
void TGame::HumanShipsHandler()
{
    TVector2 ScreenCenter = m_pVideo->GetScreenCenter();

    if ( IsSafetyPos(ScreenCenter)
        && !m_pShips[scHuman]->IsAlive()
        && !m_pShips[scHuman]->IsExploding() )
    {
        m_pShips[scHuman]->Reset();
        m_pShips[scHuman]->SetPos(ScreenCenter );
        m_pShips[scHuman]->SetRot(180.0);
        m_pShips[scHuman]->SetVel(TVector2(0,0) );
        m_pShips[scHuman]->SetAlive(true);
        m_pShips[scHuman]->SetVisible(true);
    }
}

/*!****************************************************************************
* @brief	Handles the aliens ships
******************************************************************************/
void TGame::AlienShipsHandler()
{
	static int nAlienShipTick = ALIENSHIPTICK + maths::Rand(ALIENSHIPTICK/2);

	nAlienShipTick--;

	TShip* pShip = maths::RandSign() >= 0 ? m_pShips[scAlienBig] : m_pShips[scAlienSmall];
	assert(pShip);

	if( nAlienShipTick == 0 )
	{
		nAlienShipTick = ALIENSHIPTICK + maths::Rand(ALIENSHIPTICK/2);

		if( !pShip->IsVisible() )
		{
			TVector2 ScreenCenter = m_pVideo->GetScreenCenter();

			pShip->SetPos(TVector2( 0, ScreenCenter.Y + maths::Rand(double(ScreenCenter.Y - 50)) ) );

			pShip->SetVel(TVector2( 25 + maths::AbsRand(25), 0 ) );
			pShip->SetAlive(true);
			pShip->SetVisible(true);
		}
	}
}

/*!****************************************************************************
* @brief	Returns the handle to the main game window
* @return	Window handle
******************************************************************************/
HWND TGame::GetMainWnd()
{
	assert(m_pVideo);

	return m_pVideo->GetHWnd();
}

/*!****************************************************************************
* @brief	Checks for best score
* @return	Returns true if the game score is in the best scores
******************************************************************************/
bool TGame::IsBestScore()
{
	bool bResult = false;

	int nCount = std::min( (int) BESTSCORES, (int) m_BestScores.size());

											// linear search
	for(int i=0; i<nCount; i++)
	{
		if( m_nScore > m_BestScores[i].nScore )
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Increments the total game score
* @param	nScore	The value for score increment
******************************************************************************/
void TGame::AddScore(int nScore)
{
	m_nScore += nScore;
}

/*!****************************************************************************
* @brief	Increments the total game score based on the asteroid size
* @param	pAsteroid	Pointer to the asteroid object
******************************************************************************/
void TGame::AddScore(TAsteroid* pAsteroid)
{
    if( pAsteroid->GetClass() == acBig )
    {
        AddScore((int)BIGASTEROIDSCORE);
    }
    else if( pAsteroid->GetClass() == acMedium )
    {
        AddScore((int) MIDASTEROIDSCORE);
    }
    else
    {
        AddScore((int)SMALLASTEROIDSCORE);
    }
}

/*!****************************************************************************
* @brief	Splits an asteroid in two smaller ones
* @param	pRoid	Pointer to an asteoid object
* @param	Splits	Reference to a vector of asteroid pointers
******************************************************************************/
void TGame::Split(TAsteroid* pRoid, TVecPtrAsteroids& Splits)
{
	assert(pRoid);

	enAsteroidClass nClass = pRoid->GetClass();
    enAsteroidClass nNewClass = acSmall;
    double nSize = ASTEROIDSMALLSIZE, NewSize = ASTEROIDSMALLSIZE;

    if( nClass == acBig )
    {
    	nNewClass = acMedium;
		nSize = ASTEROIDMIDSIZE;
        NewSize = ASTEROIDMIDSIZE/4.0;
    }
    else if( nClass == acMedium )
    {
    	nNewClass = acSmall;
		nSize = ASTEROIDSMALLSIZE;
        NewSize = ASTEROIDSMALLSIZE/2.0;
    }

	for(int i=0; i<2; i++)
    {
        TVector2 Pos, Vel;
        Pos = pRoid->GetPos();
        Vel = pRoid->GetVel();

        TVector2 RndVel1( maths::Rand(Vel.X)/ASTEROIDVELRATIO,
            maths::Rand(Vel.Y)/ASTEROIDVELRATIO );

        TVector2 Vel1 = Add(Vel, RndVel1);

        TAsteroid *pAsteroid = new TAsteroid(m_pVideo, m_pAudio, nNewClass,
            Pos, Add(Vel, Vel1), nSize + maths::AbsRand(NewSize));
        assert(pAsteroid);

        Splits.push_back(pAsteroid);
	}
}

/*!****************************************************************************
* @brief	Run the game
******************************************************************************/
void TGame::RunTheGame()
{
	this->m_bRun = true;
    this->m_bPause = false;
}

/*!****************************************************************************
* @brief	Stop the game
******************************************************************************/
void TGame::StopTheGame()
{
	this->m_bRun = false;
    this->m_bPause = true;
}


/*!****************************************************************************
* @brief	Shows a dialog-box to gets best score data
******************************************************************************/
void TGame::RegisterBestScore()
{
	assert(DlgBestScores);

    if( DlgBestScores->ShowModal() == mrOk )
	{
		m_strBestScoresName = DlgBestScores->GetName();

		SaveBestScores();
	}
}

/*!****************************************************************************
* @brief	Save best scores to file
******************************************************************************/
void TGame::SaveBestScores()
{
	TRecordScore BestScore;
	BestScore.nScore = m_nScore;
    BestScore.strName = m_strBestScoresName.c_str();

	m_BestScores.push_back(BestScore);

											// false -> sort from bigger to lower
	Sort(m_BestScores, false);

											// save (append) the score to file
	std::string strScoresFile = std::string(utils::GetDataPath()) + std::string(SCORESFILE);

											// "a" -> open in "append" mode
	FILE *fp = fopen(strScoresFile.c_str(), "a");

	if( fp )
	{
		fprintf(fp, "%s,%d\n", BestScore.strName.c_str(), BestScore.nScore);

		fclose(fp);
	}
}

/*!****************************************************************************
* @brief	Reorder best scores
* @param	Scores	Reference to a vector of best scores data structures
* @param	bAscending Sets the reordering mode, true for ascending order
******************************************************************************/
void TGame::Sort(TVecRecordScores& Scores, bool bAscending)
{
	for(int i=0; i<Scores.size(); i++)
	{
		for(int j=i+1; j<Scores.size(); j++)
		{
			if( bAscending )
			{
				if( Scores[i].nScore> Scores[j].nScore)
				{
					std::swap(Scores[i], Scores[j]);
				}
			}
			else
			{
				if( Scores[i].nScore< Scores[j].nScore)
				{
					std::swap(Scores[i], Scores[j]);
				}
			}
		}
	}
}

/*!****************************************************************************
* @brief	Gets the size of the client area
* @param[in,out] nW Width of clienr area
* @param[in,out] nH Height of clienr area
******************************************************************************/
void TGame::GetClientArea(unsigned& nW, unsigned& nH)
{
	assert(m_pVideo);

	RECT ClientArea = m_pVideo->GetClientArea();

	nW = ClientArea.right;
	nH = ClientArea.bottom;
}

/*!****************************************************************************
* @brief	Gets the size of the client area
* @return	The size (RECT) of the client area
******************************************************************************/
RECT TGame::GetClientArea()
{
	assert(m_pVideo);

    RECT ClientArea = m_pVideo->GetClientArea();

	RECT Rect;
    Rect.left = Rect.top = 0;
    Rect.right = ClientArea.right;
    Rect.bottom =ClientArea.bottom;

	return Rect;
}

/*!****************************************************************************
* @brief	Run the game
******************************************************************************/
void TGame::Run()
{
	assert(m_pAudio);
	assert(m_pVideo);
	assert(m_pShips[scHuman]);
	assert(m_pShips[scAlienBig]);
	assert(m_pShips[scAlienSmall]);

											// update the ships
	for(int i=0; i<m_pShips.size(); ++i)
	{
		m_pShips[i]->Update(DT);
	}
											// if alien ships are active (visibles)
											// then make shoots against human ships
	{
		static int nTickCount = 0;
		nTickCount++;

		if( nTickCount >= ALIENSHOTDELAY)
		{
			nTickCount = 0;

			if( m_pShips[scAlienBig]->IsVisible() && m_pShips[scAlienBig]->IsAlive() )
			{
				TMissile *pMissile = new TMissile(m_pVideo);
				assert(pMissile);

				pMissile->SetShip(m_pShips[scAlienBig]);

				m_pMissiles.push_back(pMissile);

				{
					TVector2 AlienPos = m_pShips[scAlienBig]->GetPos();
					TVector2 HumanPos = m_pShips[scHuman]->GetPos();

					double Mod = MISSILESPEED;

					double DX = HumanPos.X - AlienPos.X;
					double DY = HumanPos.Y - AlienPos.Y;

					//double Rot = atan2(DY, DX);	// NO! troppo preciso!
					double Rot = atan2(DY, DX) + ALIENBIGINACCURACY + maths::Rand(ALIENBIGINACCURACY);

					TVector2 Vel( Mod*cos(Rot), Mod*sin(Rot) );

					TVector2 ShipVel = m_pShips[scAlienBig]->GetVel();

					pMissile->Arm(AlienPos, Vel);
				}
			}

			if( m_pShips[scAlienSmall]->IsVisible()&& m_pShips[scAlienSmall]->IsAlive() )
			{
				TMissile *pMissile = new TMissile(m_pVideo);
				assert(pMissile);

				pMissile->SetShip(m_pShips[scAlienSmall]);

				m_pMissiles.push_back(pMissile);

				{
					TVector2 AlienPos = m_pShips[scAlienSmall]->GetPos();
					TVector2 HumanPos = m_pShips[scHuman]->GetPos();

					double Mod = MISSILESPEED;

					double DX = HumanPos.X - AlienPos.X;
					double DY = HumanPos.Y - AlienPos.Y;

					//double Rot = atan2(DY, DX);	// NO! troppo preciso!
					double Rot = atan2(DY, DX) + ALIENSMALLINACCURACY + maths::Rand(ALIENSMALLINACCURACY);

					TVector2 Vel( Mod*cos(Rot), Mod*sin(Rot) );

					TVector2 ShipVel = m_pShips[scAlienSmall]->GetVel();

					pMissile->Arm(AlienPos, Vel);
				}
			}
		}
	}
											// update the missiles
	for(int i=0; i<m_pMissiles.size(); i++)
	{
		TMissile* pMissile = static_cast<TMissile*>(m_pMissiles[i]);

		if ( pMissile && pMissile->IsArmed() )
		{
			pMissile->Update(DT);
		}
	}
											// update the asteroids
	unsigned nWidth, nHeight;
	GetClientArea(nWidth, nHeight);

	for(int i=0; i<m_pAsteroids.size(); ++i)
	{
		TAsteroid *pAsteroid = m_pAsteroids[i];
		assert(pAsteroid);

        if( pAsteroid->IsAlive() || pAsteroid->IsExploding() )
		{
			pAsteroid->Update(DT);
		}
	}
											// forces actors inside of scenery limits
	ForceInsideLimits();

	if( !IsGameOver() )
	{
		HumanShipsHandler();
		AlienShipsHandler();
		CollisionHandler();
		BonusHandler();
		LevelHandler();
	}
	else
	{
#ifndef _DEVEL
		GameOverHandler();
#endif
	}
											// show info (help, ships, score, etc...)
	ShowInfo();
}

/*!****************************************************************************
* @brief	Handles collisions between all objects of the scenario
******************************************************************************/
void TGame::CollisionHandler()
{
											// check for collisions between ...

											// ... human ship and alien ships

	{
		TShip *pHuman = m_pShips[scHuman],
        	*pAlienBig = m_pShips[scAlienBig],
            *pAlienSmall = m_pShips[scAlienSmall];

		if( pAlienBig->IsAlive() && pAlienBig->IsVisible() )
        {
			if( Collide(pHuman, pAlienBig) )
            {
				pHuman->Explode();
                pAlienBig->Explode();

                m_nLives--;
            }
        }
		else if( pAlienSmall->IsAlive() && pAlienSmall->IsVisible() )
        {
			if( Collide(pHuman, pAlienSmall) )
            {
				pHuman->Explode();
                pAlienSmall->Explode();

                m_nLives--;
            }
        }
    }

											// ... ships and asteroids
	for(int i=0; i<m_pAsteroids.size(); ++i)
	{
		for(int j=0; j<m_pShips.size(); ++j)
		{
			if( m_pAsteroids[i]->IsAlive() && m_pShips[j]->IsAlive() )
			{
				if( Collide(m_pAsteroids[i], m_pShips[j]) )
				{
					m_pShips[j]->Explode();
                    m_pAsteroids[i]->Explode();

					if( m_pShips[j]->GetClass() == scHuman )
					{
						m_nLives--;
					}

					break;
				}
			}
		}
	}
											// ... missiles and ships

	for(int i=0; i<m_pMissiles.size(); ++i)
	{
		TMissile *pMissile = static_cast<TMissile*>(m_pMissiles[i]);

		if( pMissile && pMissile->IsArmed() )
		{
			for(int j=0; j<m_pShips.size(); ++j)
			{
				TShip* pShip = m_pShips[j];

				if( pShip->IsAlive()
					&& pMissile
											// avoids that the missile destroy
											// the ship itself that has shooted it
					&& pMissile->GetShip() != pShip
				)
				{
					if( pShip->IsColliding( pMissile->GetPos()) && !pShip->IsShieldActive() )
					{
						pShip->Explode();

						delete pMissile;
						pMissile = NULL;
						m_pMissiles[i] = NULL;

						if( pShip->GetClass() == scHuman )
						{
							m_nLives--;

							if( m_nLives == 0 )
							{
								GameOver();
							}
						}
						else if( pShip->GetClass() == scAlienBig )
						{
                            AddScore(BIGALIENSHIPSCORE);
						}
						else if( pShip->GetClass() == scAlienSmall )
						{
                            AddScore(SMALLALIENSHIPSCORE);
						}
					}
				}
			}
		}
	}
											// ... missiles and asteroids
	for(int i=0; i<m_pMissiles.size(); i++)
	{
		TMissile* pMissile = static_cast<TMissile*>(m_pMissiles[i]);

		if( pMissile && pMissile->IsArmed() )
		{
			for(int j=0; j<m_pAsteroids.size(); ++j)
			{
				TAsteroid *pRoid = m_pAsteroids[j];
                assert(pRoid);

				if( pRoid->IsAlive() && pMissile && pRoid->Collide( pMissile->GetPos() ) )
				{
                    pRoid->Explode();
                    AddScore(pRoid);

					if( pRoid->GetClass() != acSmall )
                    {
						TVecPtrAsteroids Splits;
												// split an asteroid (big,medium)
                                                // in two smaller ones asteorids
                        Split(pRoid, Splits);

                        for(int i=0; i<Splits.size(); i++)
                        {
							Splits[i]->SetAlive(true);
                        	m_pAsteroids.push_back(Splits[i]);
                    	}
                    }

					delete pMissile;
					m_pMissiles[i] = pMissile = NULL;
				}
			}
		}
	}
											// deletes the missiles that have
                                            // gone out of range (screen area)
	for(int i=0; i<m_pMissiles.size(); i++)
	{
		TMissile *pMissile = static_cast<TMissile*>(m_pMissiles[i]);

		if( pMissile && !IsInsideGameArea( pMissile->GetPos() ) )
		{
			delete m_pMissiles[i];
			m_pMissiles[i] = NULL;
		}
	}
											// checks for game-over
    if( m_nLives == 0 )
    {
        GameOver();

        if( IsBestScore() )
        {
            StopTheGame();

            RegisterBestScore();

            RunTheGame();
        }
    }
}


