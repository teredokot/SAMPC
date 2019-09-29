unit About;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, MMSystem, StdCtrls;

type
  TfmAbout = class(TForm)
    procedure FormShow(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  TBGRArray = Array[0..MaxInt div SizeOf(TRGBQuad)-1] of TRGBQuad;
  PBGRArray = ^TBGRArray;

  TStar = record
    x, y, len, speed: Integer;
  end;

  TCreditLine = record
    Line: String;
    Color: COLORREF;
    Bold: Boolean;
  end;

const
  COLOR_TITLE = $000080FF;
  COLOR_NAME  = $00FFFFFF;//$00FFAA00;
  COLOR_URL   = $00FFAA00;//$008000FF;

var
  CreditLines: Array[0..102] of TCreditLine =
  (
    //---------- Header Start
    //(Line: 'Grand Theft Auto'; Color: $00FF8000; Bold: true),
    (Line: 'San Andreas'; Color: $000080FF; Bold: true),
    (Line: 'Multiplayer'; Color: $008000FF; Bold: true),
    //---------- Header End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- Coder Start
    (Line: 'Coding'; Color: COLOR_TITLE; Bold: true),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'kyeman'; Color: COLOR_NAME; Bold: false),
    (Line: 'spookie'; Color: COLOR_NAME; Bold: false),
    //---------- Coder End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- RE Start
    (Line: 'Reverse Engineering'; Color: COLOR_TITLE; Bold: true),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'kyeman'; Color: COLOR_NAME; Bold: false),
    (Line: 'spookie'; Color: COLOR_NAME; Bold: false),
    //---------- RE End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- Scripting Start
    (Line: 'Scripting'; Color: COLOR_TITLE; Bold: true),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'spookie'; Color: COLOR_NAME; Bold: false),
    (Line: 'kyeman'; Color: COLOR_NAME; Bold: false),
    (Line: 'jax'; Color: COLOR_NAME; Bold: false),
    (Line: 'Mike'; Color: COLOR_NAME; Bold: false),
    (Line: 'Cam'; Color: COLOR_NAME; Bold: false),
    //---------- Research End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- Research Start
    (Line: 'Research'; Color: COLOR_TITLE; Bold: true),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'Luke'; Color: COLOR_NAME; Bold: false),
    (Line: 'Falcon'; Color: COLOR_NAME; Bold: false),
    //---------- Research End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- Tester Start
    (Line: 'Testing'; Color: COLOR_TITLE; Bold: true),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'adamcs, bakasan, Born Acorn'; Color: COLOR_NAME; Bold: false),
    (Line: 'Dalpura, Damian, Delfi'; Color: COLOR_NAME; Bold: false),
    (Line: 'dexx, DrAke$, Drift'; Color: COLOR_NAME; Bold: false),
    (Line: 'ECLiPSE, f3llah1n, him selfe'; Color: COLOR_NAME; Bold: false),
    (Line: 'illspirit, littlewhitey, MrJax'; Color: COLOR_NAME; Bold: false),
    (Line: 'njr1489, Posty, PsYcHoGoD'; Color: COLOR_NAME; Bold: false),
    (Line: 'Shizz, Simon, sockx'; Color: COLOR_NAME; Bold: false),
    (Line: 'squiddy, Static, steve-m'; Color: COLOR_NAME; Bold: false),
    (Line: 'The Azer, Trix, Wacko'; Color: COLOR_NAME; Bold: false),
    (Line: 'XcR, Y_Less, [ULK]Crack'; Color: COLOR_NAME; Bold: false),
    //---------- Tester End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- Support Start
    (Line: 'Support'; Color: COLOR_TITLE; Bold: true),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'GTANet'; Color: COLOR_NAME; Bold: false),
    (Line: 'www.gtanet.com'; Color: COLOR_URL; Bold: false),
    (Line: ''; Color: 666; Bold: false), // Spacer
    //(Line: 'Rockstar North'; Color: COLOR_NAME; Bold: false),
    //(Line: 'www.rockstarnorth.com'; Color: COLOR_URL; Bold: false),
    //(Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'GTA Host'; Color: COLOR_NAME; Bold: false),
    (Line: 'www.gta-host.com'; Color: COLOR_URL; Bold: false),
    //---------- Support End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- Thanks Start
    (Line: 'Thanks'; Color: COLOR_TITLE; Bold: true),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'Rockstar North'; Color: COLOR_NAME; Bold: false),
    (Line: 'Kryptos, CyQ, Dan'; Color: COLOR_NAME; Bold: false),
    (Line: 'Tank, Jevon'; Color: COLOR_NAME; Bold: false),
    //---------- Thanks End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer

    //---------- URL Start
    (Line: 'www.sa-mp.com'; Color: COLOR_URL; Bold: true),
    //---------- URL End

    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'You can now access the secret levels.'; Color: COLOR_TITLE; Bold: false),
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: ''; Color: 666; Bold: false), // Spacer
    (Line: 'Just kidding :)'; Color: COLOR_URL; Bold: false)
  );

  fmAbout: TfmAbout;
  CritSect: _RTL_CRITICAL_SECTION;
  GameOver: Boolean;

  DIBWidth: Integer;
  DIBHeight: Integer;

  // Timing
  Ticks: Cardinal;
  TimeScale: Integer = 1;

  // Drawing
  RenderThread: THandle;
  bmi: BITMAPINFO;
  hDC1, hDC2: HDC;
  Buf: PBGRArray;
  hBmp: HBITMAP;
  hNormFont, hBoldFont: HFONT;
  xRect: TRect;

  // Stars
  Stars: Array[1..30] of TStar;

implementation

{$R *.dfm}

function Col(r,g,b: Byte): TRGBQuad;
begin
  Result.rgbBlue:= b;
  Result.rgbGreen:= g;
  Result.rgbRed:= r;
  Result.rgbReserved:= 0;
end;

procedure DrawStars;
var
  i, j, buf_pos: Integer;
begin
  for i:= 1 to 30 do begin
    Dec(Stars[i].x, Stars[i].speed * TimeScale);
    if Stars[i].x <= -Stars[i].len then begin
      Stars[i].x:= Random(DIBWidth) + DIBWidth;
      Stars[i].y:= Random(DIBHeight);
      Stars[i].speed:= Random(4)+2;
      Stars[i].len:= Stars[i].speed * 3;
    end;
    if Stars[i].x < DIBWidth then begin
      buf_pos:= (Stars[i].y*DIBWidth) + Stars[i].x;
      if Stars[i].x > 0 then
        Buf[buf_pos]:= Col(255, 255, 255);
      for j:= 1 to Stars[i].len do
        if (Stars[i].x + j < DIBWidth) and (Stars[i].x + j > 0) then begin
          Buf[buf_pos+j].rgbBlue:= (255 div Stars[i].len) * (Stars[i].len-j);
          Buf[buf_pos+j].rgbGreen:= (180 div Stars[i].len) * (Stars[i].len-j);
          Buf[buf_pos+j].rgbRed:= (100 div Stars[i].len) * (Stars[i].len-j);
        end;
    end;
  end;
end;

var
  CreditsRollY: Integer = 260;
  TempCRY: Integer = 0;
procedure Flip;
var
  i: Integer;
begin
  SetDIBits(hDC2, hBmp, 0, DIBHeight, @Buf[0], bmi, DIB_RGB_COLORS);

  Inc(TempCRY, TimeScale);
  if TempCRY >= 5 then begin
    TempCRY:= 0;
    Dec(CreditsRollY, TimeScale);
    if CreditsRollY < -((High(CreditLines) * 12) + 50) then
      CreditsRollY:= 260;
  end;

  xRect.Top:= CreditsRollY;
  for i:= 0 to High(CreditLines) do begin
    if (CreditLines[i].Color <> 666) and (xRect.Top > -12) and (xRect.Top < 260) then begin
      SetTextColor(hDC2, CreditLines[i].Color);
      if CreditLines[i].Bold then
        SelectObject(hDC2, hBoldFont)
      else
        SelectObject(hDC2, hNormFont);
      DrawText(hDC2, PChar(CreditLines[i].Line), -1, xRect, DT_NOCLIP or DT_CENTER);
    end;
    Inc(xRect.Top, 12);
  end;

  BitBlt(hDC1, 2, 2, DIBWidth, DIBHeight, hDC2, 0, 0, SRCCOPY);
end;

procedure Render;
var
  t: Cardinal;
begin
  timeBeginPeriod(1);
  t:= timeGetTime;
  timeEndPeriod(1);
  TimeScale:= Round(100 / (1000 / (t - Ticks)));
  Ticks:= t;

  ZeroMemory(Buf, (DIBWidth*DIBHeight)*4);

  DrawStars;

  Flip;
end;

procedure RenderTimer;
begin
  while true do begin
    Sleep(10);
    EnterCriticalSection(CritSect);
    if GameOver then begin
      LeaveCriticalSection(CritSect);
      Exit;
    end;
    Render;
    LeaveCriticalSection(CritSect);
  end;
end;

procedure TfmAbout.FormShow(Sender: TObject);
var
  FontStruct: LogFont;
begin
  CreditsRollY:= 260;

  GetMem(Buf, (DIBWidth*DIBHeight)*4);
  hDC1:= GetDC(fmAbout.Handle);
  hDC2:= CreateCompatibleDC(hDC1);
  hBmp:= CreateCompatibleBitmap(hDC1, DIBWidth, DIBHeight);
  ZeroMemory(@FontStruct, SizeOf(FontStruct));
  FontStruct.lfWidth:= 0;
  FontStruct.lfHeight:= -10;
  FontStruct.lfQuality:= PROOF_QUALITY;
  FontStruct.lfFaceName:= 'Verdana';
  hNormFont:= CreateFontIndirect(FontStruct);
  FontStruct.lfWeight:= FW_BOLD;
  hBoldFont:= CreateFontIndirect(FontStruct);

  SelectObject(hDC2, hBmp);
  SetBkMode(hDC2, TRANSPARENT);

  Ticks:= timeGetTime;
  GameOver:= false;
  BeginThread(nil, 0, @RenderTimer, nil, 0, RenderThread);
end;

procedure TfmAbout.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  EnterCriticalSection(CritSect);

  GameOver:= true;

  DeleteObject(hNormFont);
  DeleteObject(hBoldFont);
  DeleteObject(hBmp);
  DeleteDC(hDC1);
  DeleteDC(hDC2);
  FreeMem(Buf);

  LeaveCriticalSection(CritSect);
end;

procedure TfmAbout.FormCreate(Sender: TObject);
begin
  InitializeCriticalSection(CritSect);

  DIBWidth:= ClientWidth - 4;
  DIBHeight:= ClientHeight - 4;

  xRect:= Rect(0, 0, ClientWidth, ClientHeight);

  with bmi.bmiHeader do begin
    biSize:=          SizeOf(bmi.bmiHeader);
    biWidth:=         DIBWidth;
    biHeight:=       -DIBHeight;
    biPlanes:=        1;
    biBitCount:=      32;
    biCompression:=   BI_RGB;
    biSizeImage:=     0;
    biXPelsPerMeter:= 0;
    biYPelsPerMeter:= 0;
    biClrUsed:=       0;
    biClrImportant:=  0;
  end;
end;

procedure TfmAbout.FormDestroy(Sender: TObject);
begin
  DeleteCriticalSection(CritSect);
end;

procedure TfmAbout.FormClick(Sender: TObject);
begin
  Close;
end;

end.
