//---------------------------------------------------------------------------
#ifndef TDlgBestScoresH
#define TDlgBestScoresH

//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

//---------------------------------------------------------------------------
class TDlgBestScores : public TForm
{
__published:	// IDE-managed Components
	TButton *ButtonOk;
	TButton *ButtonCancel;
	TLabel *Label1;
	TEdit *EditName;
	TLabel *Label2;
	void __fastcall ButtonOkClick(TObject *Sender);
	void __fastcall ButtonCancelClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TDlgBestScores(TComponent* Owner);
public:
	AnsiString __fastcall GetName();
};

//---------------------------------------------------------------------------
extern PACKAGE TDlgBestScores *DlgBestScores;
//---------------------------------------------------------------------------
#endif
