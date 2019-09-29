object fmMasterUpdate: TfmMasterUpdate
  Left = 452
  Top = 328
  Width = 308
  Height = 130
  Caption = 'Master Server Update'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Verdana'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblPleaseWait: TLabel
    Left = 0
    Top = 24
    Width = 300
    Height = 13
    Alignment = taCenter
    AutoSize = False
    Caption = 'Please Wait'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Verdana'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblUpdating: TLabel
    Left = 0
    Top = 56
    Width = 300
    Height = 13
    Alignment = taCenter
    AutoSize = False
    Caption = 'Retrieving list from master server...'
  end
end
