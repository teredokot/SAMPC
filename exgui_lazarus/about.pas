unit about;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs;

type

  { TfmAbout }

  TfmAbout = class(TForm)
    procedure FormClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private

  public

  end;

var
  fmAbout: TfmAbout;

implementation

{$R *.lfm}

{ TfmAbout }

procedure TfmAbout.FormClick(Sender: TObject);
begin

end;

procedure TfmAbout.FormClose(Sender: TObject; var CloseAction: TCloseAction);
begin

end;

procedure TfmAbout.FormCreate(Sender: TObject);
begin

end;

procedure TfmAbout.FormDestroy(Sender: TObject);
begin

end;

procedure TfmAbout.FormShow(Sender: TObject);
begin

end;

end.

