//---------------------------------------------------------------------------
#ifndef TFormMainH
#define TFormMainH

//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>


class TGame;
class TSoundManager;
class TVideoManager;

//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
	TTimer *Timer;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:	// User declarations

public:		// User declarations
	__fastcall TFormMain(TComponent* Owner);

protected:
	TGame* m_pGame;
	TSoundManager *m_pAudio;
    TVideoManager *m_pVideo;

	void Setup();
    void Cleanup();
    void MainLoop();
    void KeyboardHandler();
    void ForceToFPS(unsigned nFPS);
};

//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
