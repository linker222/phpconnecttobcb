object fmMyIndy: TfmMyIndy
  Left = 0
  Top = 0
  Caption = 'Ask Server'
  ClientHeight = 691
  ClientWidth = 980
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = #24494#36575#27491#40657#39636
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 20
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 980
    Height = 49
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object Label1: TLabel
      Left = 722
      Top = 10
      Width = 64
      Height = 20
      Caption = #36899#32218#25976#65306
      Font.Charset = CHINESEBIG5_CHARSET
      Font.Color = clBlue
      Font.Height = -16
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object Label_ConnectCount: TLabel
      Left = 792
      Top = 10
      Width = 9
      Height = 20
      Caption = '0'
      Font.Charset = CHINESEBIG5_CHARSET
      Font.Color = clBlue
      Font.Height = -16
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object Label3: TLabel
      Left = 16
      Top = 10
      Width = 31
      Height = 20
      Caption = 'IP'#65306
      Transparent = True
    end
    object Label4: TLabel
      Left = 199
      Top = 10
      Width = 48
      Height = 20
      Caption = 'Port'#65306
      Transparent = True
    end
    object Edit_Port: TEdit
      Left = 253
      Top = 10
      Width = 60
      Height = 28
      TabOrder = 0
      Text = '8080'
    end
    object Button_Start: TButton
      Left = 319
      Top = 10
      Width = 138
      Height = 25
      Caption = #21855#21205
      TabOrder = 1
      OnClick = Button_StartClick
    end
    object Edit_IP: TEdit
      Left = 72
      Top = 10
      Width = 121
      Height = 28
      TabOrder = 2
      Text = '127.0.0.1'
    end
    object Button_CountConnect: TButton
      Left = 831
      Top = 10
      Width = 138
      Height = 25
      Caption = #32113#35336#36899#32218#25976
      TabOrder = 3
      OnClick = Button_CountConnectClick
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 49
    Width = 980
    Height = 642
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object Memo_Server: TMemo
      Left = 0
      Top = 0
      Width = 980
      Height = 642
      Align = alClient
      ReadOnly = True
      TabOrder = 0
    end
  end
  object IdTCPServer1: TIdTCPServer
    OnStatus = IdTCPServer1Status
    Bindings = <>
    DefaultPort = 80
    OnConnect = IdTCPServer1Connect
    OnDisconnect = IdTCPServer1Disconnect
    OnException = IdTCPServer1Exception
    OnExecute = IdTCPServer1Execute
    Left = 104
    Top = 168
  end
  object Timer1: TTimer
    Interval = 30000
    OnTimer = Timer1Timer
    Left = 376
    Top = 232
  end
end
