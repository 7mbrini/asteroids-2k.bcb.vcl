object DlgBestScores: TDlgBestScores
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Best Scores'
  ClientHeight = 180
  ClientWidth = 240
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 38
    Top = 29
    Width = 160
    Height = 13
    Alignment = taCenter
    AutoSize = False
    Caption = 'Best Score !'
  end
  object Label2: TLabel
    Left = 38
    Top = 66
    Width = 160
    Height = 13
    Alignment = taCenter
    AutoSize = False
    Caption = 'Please, enter your name:'
  end
  object ButtonOk: TButton
    Left = 38
    Top = 138
    Width = 75
    Height = 25
    Caption = 'Ok'
    TabOrder = 0
    OnClick = ButtonOkClick
  end
  object ButtonCancel: TButton
    Left = 123
    Top = 138
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = ButtonCancelClick
  end
  object EditName: TEdit
    Left = 38
    Top = 88
    Width = 160
    Height = 21
    Alignment = taCenter
    CharCase = ecUpperCase
    TabOrder = 2
  end
end
