//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TDlgBestScores.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDlgBestScores *DlgBestScores;


//---------------------------------------------------------------------------
__fastcall TDlgBestScores::TDlgBestScores(TComponent* Owner) : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TDlgBestScores::FormCreate(TObject *Sender)
{
	this->Caption = "Asteroids-2k Best Scores";
    this->FormStyle = fsStayOnTop;
    this->BorderStyle = bsSingle;
}

//---------------------------------------------------------------------------
void __fastcall TDlgBestScores::ButtonOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}

//---------------------------------------------------------------------------
void __fastcall TDlgBestScores::ButtonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------
AnsiString __fastcall TDlgBestScores::GetName()
{
	return this->EditName->Text;
}

//---------------------------------------------------------------------------
