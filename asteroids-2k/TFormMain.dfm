object FormMain: TFormMain
  Left = 0
  Top = 0
  Caption = 'Asteroids-2k'
  ClientHeight = 343
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnCreate = FormCreate
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object Timer: TTimer
    Interval = 10
    OnTimer = TimerTimer
    Left = 70
    Top = 60
  end
end
