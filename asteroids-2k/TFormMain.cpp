//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#include "TFormMain.h"

#include "video.h"
#include "audio.h"
#include "game.h"
#include "commdefs.h"

#include "TDlgBestScores.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;

//---------------------------------------------------------------------------
//#define _DEVEL


/*!****************************************************************************
* @brief	Constructor
* @param	Owner Pointer to the owner object
******************************************************************************/
__fastcall TFormMain::TFormMain(TComponent* Owner) : TForm(Owner)
{
	m_pGame = NULL;
	m_pAudio = NULL;
    m_pVideo = NULL;
}

/*!****************************************************************************
* @brief	FormCreate
* @param	Sender Pointer to the event source object
******************************************************************************/
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
    Setup();
}

/*!****************************************************************************
* @brief	Paint Event Handler
* @param	Sender Pointer to the event source object
******************************************************************************/
void __fastcall TFormMain::FormPaint(TObject *Sender)
{
	if( m_pGame )
    {

        HDC hDC = (HDC) this->Canvas->Handle;
        assert(hDC);

		HDC hVMDC = m_pGame->GetVM()->GetDC();
        RECT Rect = m_pGame->GetClientArea();

        ::BitBlt(hDC, 0, 0, Rect.right, Rect.bottom, hVMDC, 0, 0, SRCCOPY);
	}
}

/*!****************************************************************************
* @brief	Timer Event Handler
* @param	Sender Pointer to the event source object
******************************************************************************/
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
	MainLoop();
}

/*!****************************************************************************
* @brief	Form Close Event Handler
* @param	Sender Pointer to the event source object
* @param	Action Reference to action type specifier
******************************************************************************/
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	Cleanup();
}

/*!****************************************************************************
* @brief	Setting-up the application
******************************************************************************/
void TFormMain::Setup()
{
	this->Caption = APPNAME;

	this->Width = FRAMEW;
    this->Height = FRAMEH;
    this->BorderStyle = bsSingle;
    this->BorderIcons >> biMaximize;
	this->Position = poScreenCenter;

	DlgBestScores = new TDlgBestScores(NULL);
    assert(DlgBestScores);
    DlgBestScores->Visible = false;

	srand(::GetCurrentTime());

								// setting-up the video manager
	try
    {
        m_pVideo = new TVideoManager((HWND)this->Handle, Rect(0,0,FRAMEW,FRAMEH));
        assert(m_pVideo);
	}
    catch(...)
    {
		::MessageBox(0, L"Error intializing the Video Manager", L"Error", MB_OK | MB_ICONERROR);
		exit(-1);
    }

								// setting-up the sound manager
    try
    {
    	m_pAudio = new TSoundManager();
		assert(m_pAudio);

		m_pAudio->SetMasterVolume(0.25);
	}
    catch(...)
    {
		::MessageBox(0, L"Error intializing OpenAL!", L"Error", MB_OK | MB_ICONERROR);
		exit(-1);
	}

								// create the game
	try
    {
        m_pGame = new TGame(m_pVideo, m_pAudio);
        assert(m_pGame);
	}
    catch(...)
    {
		::MessageBox(0,
			TEXT("Error intializing the game! "),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONERROR | MB_TASKMODAL);

		exit(-1);
    }

#ifdef _DEVEL
	m_pGame->Restart();
#else
	m_pGame->GameOver();
#endif
}

/*!****************************************************************************
* @brief	Cleaning-up the application
******************************************************************************/
void TFormMain::Cleanup()
{
	assert(m_pGame);
	assert(m_pAudio);
    assert(m_pVideo);

	delete m_pGame;
    delete m_pAudio;
    delete m_pVideo;
}

/*!****************************************************************************
* @brief	Keyboard handler
* @note		Uses GetAsynkKeyState() function instead of WM_KEYDOWN event
*			handler so that can handle multiple keys pressed simultaneously
******************************************************************************/
void TFormMain::KeyboardHandler()
{
	static SHORT nKeys[256];
	int VK_N = 0X4E, VK_P = 0X50, VK_Q = 0X51, VK_S = 0X53;

	nKeys[VK_N] = GetAsyncKeyState(0X4E);
	nKeys[VK_P] = GetAsyncKeyState(0x50);
	nKeys[VK_Q] = GetAsyncKeyState(0X51);
	nKeys[VK_S] = GetAsyncKeyState(0x53);

	nKeys[VK_LEFT] = GetAsyncKeyState(VK_LEFT);
	nKeys[VK_RIGHT] = GetAsyncKeyState(VK_RIGHT);
	nKeys[VK_UP] = GetAsyncKeyState(VK_UP);
	nKeys[VK_DOWN] = GetAsyncKeyState(VK_DOWN);
	nKeys[VK_SPACE] = GetAsyncKeyState(VK_SPACE);
	nKeys[VK_ESCAPE] = GetAsyncKeyState(VK_ESCAPE);
	nKeys[VK_ADD] = GetAsyncKeyState(VK_ADD);
	nKeys[VK_SUBTRACT] = GetAsyncKeyState(VK_SUBTRACT);

	if( nKeys[VK_N] && !m_pGame->IsPausing() ) m_pGame->Restart();

	if( nKeys[VK_P] ) m_pGame->PauseTheGame();
	if( nKeys[VK_ADD] ) m_pGame->GetSM()->IncreaseMasterVolume();
	if( nKeys[VK_SUBTRACT] ) m_pGame->GetSM()->DecreaseMasterVolume();
	if( nKeys[VK_ESCAPE] | nKeys[VK_Q] ) { m_pGame->EndTheGame(); PostQuitMessage(0); }

	if( m_pGame->GetShip(scHuman)->IsAlive() )
	{
		if( nKeys[VK_S] ) m_pGame->GetShip(scHuman)->ActivateTheShield();;
		if( nKeys[VK_SPACE] ) m_pGame->ShotTheMissile(m_pGame->GetShip(scHuman));
		if( nKeys[VK_LEFT] ) m_pGame->GetShip(scHuman)->RotateLeft(SHIP_ROTSTEP);
		if( nKeys[VK_RIGHT] ) m_pGame->GetShip(scHuman)->RotateRight(SHIP_ROTSTEP);
		if( nKeys[VK_UP] ) m_pGame->GetShip(scHuman)->Impulse(SHIP_IMPULSE);
	}
}

/*!****************************************************************************
* @brief	Force the execution speed to a specified framerate
* @param	nFPS The value of Frames per Second to be forced to
******************************************************************************/
void TFormMain::ForceToFPS(unsigned nFPS)
{
	assert(nFPS > 0);

	static unsigned nCurTime = ::GetTickCount();

	while( (::GetTickCount() - nCurTime) < 1.0/FPS * 1000.0) {;}
	nCurTime = ::GetTickCount();
}

/*!****************************************************************************
* @brief	Main application loop
******************************************************************************/
void TFormMain::MainLoop()
{
	assert(m_pGame);

	KeyboardHandler();

	if( m_pGame->IsRunning() && !m_pGame->IsPausing() )
	{
											// clear the screen to black
		m_pGame->GetVM()->ClearScreen(RGB(0,0,0));

		m_pGame->Run();
											// Force to repaint. The last paramater
											// [BOOL bErase] must be set to FALSE
											// to avoid annoying flickering effects
		InvalidateRect((HWND) this->Handle, &m_pGame->GetClientArea(), FALSE);

											// Force to a specific FPS so that
											// the frame-rate is CPU independent
		ForceToFPS(FPS);
	}
}

//---------------------------------------------------------------------------


