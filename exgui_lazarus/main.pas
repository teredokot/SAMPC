unit Main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ComCtrls, Menus,
  ExtCtrls, StdCtrls, ValEdit, Grids, TAGraph, Types;

type

  { TfmMain }

  TfmMain = class(TForm)
    cbFilterEmpty: TCheckBox;
    cbFilterPassworded: TCheckBox;
    cbFilterFull: TCheckBox;
    chSIPingChart: TChart;
    edSIPing: TLabeledEdit;
    edSIMap: TLabeledEdit;
    edSIMode: TLabeledEdit;
    edSIPlayers: TLabeledEdit;
    gbFilter: TGroupBox;
    gbInfo: TGroupBox;
    ilMain: TImageList;
    imLogo: TImage;
    edFilterMode: TLabeledEdit;
    edFilterMap: TLabeledEdit;
    edSIAddress: TLabeledEdit;
    lebName: TLabeledEdit;
    lvServers: TListView;
    lvPlayers: TListView;
    lvRules: TListView;
    N5: TMenuItem;
    miFile: TMenuItem;
    miQuickConnect: TMenuItem;
    miAddServer: TMenuItem;
    miDeleteServer: TMenuItem;
    miRefreshServer: TMenuItem;
    miMasterServerUpdate: TMenuItem;
    miCopyServerInfo: TMenuItem;
    miServerProperties: TMenuItem;
    miTools: TMenuItem;
    miSettings: TMenuItem;
    miHelp: TMenuItem;
    miAbout: TMenuItem;
    N4: TMenuItem;
    N3: TMenuItem;
    N2: TMenuItem;
    miImportFavoritesList: TMenuItem;
    miExportFavoritesList: TMenuItem;
    miExit: TMenuItem;
    miView: TMenuItem;
    miFilterServerInfo: TMenuItem;
    miStatusBar: TMenuItem;
    miServers: TMenuItem;
    miConnect: TMenuItem;
    N1: TMenuItem;
    mmMain: TMainMenu;
    Panel1: TPanel;
    pURL: TPanel;
    pnLine: TPanel;
    pnBreakable: TPanel;
    //pnLine: TPanel;
    sbMain: TStatusBar;
    Splitter1: TSplitter;
    Splitter2: TSplitter;
    tsServerLists: TTabControl;
    tbMain: TToolBar;
    tbConnect: TToolButton;
    tbCopyServerInfo: TToolButton;
    tbServerProperties: TToolButton;
    tbSpacer4: TToolButton;
    tbSettings: TToolButton;
    tbQuickConnect: TToolButton;
    tbSpacer1: TToolButton;
    tbAddServer: TToolButton;
    tbDeleteServer: TToolButton;
    tbRefreshServer: TToolButton;
    tbSpacer2: TToolButton;
    tbMasterServerUpdate: TToolButton;
    tbSpacer3: TToolButton;
    tbSpacer5: TToolButton;
    tbHelp: TToolButton;
    tbAbout: TToolButton;
    procedure AboutClick(Sender: TObject);
    procedure AddServerClick(Sender: TObject);
    procedure ConnectClick(Sender: TObject);
    procedure CopyServerInfoClick(Sender: TObject);
    procedure DeleteServerClick(Sender: TObject);
    procedure edFilterModeClick(Sender: TObject);
    procedure ExitClick(Sender: TObject);
    procedure ExportFavoritesClick(Sender: TObject);
    procedure FilterChange(Sender: TObject);
    procedure GroupBox1Click(Sender: TObject);
    procedure imLogoClick(Sender: TObject);
    procedure ImportFavoritesClick(Sender: TObject);
    procedure lbServersContextPopup(Sender: TObject; MousePos: TPoint;
      var Handled: Boolean);
    procedure MasterServerUpdateClick(Sender: TObject);
    procedure miViewClick(Sender: TObject);
    procedure Panel1Click(Sender: TObject);
    procedure pnBreakableResize(Sender: TObject);
    procedure QuickConnectClick(Sender: TObject);
    procedure RefreshServerClick(Sender: TObject);
    procedure ServerPropertiesClick(Sender: TObject);
    procedure SettingsClick(Sender: TObject);
    procedure Splitter1CanOffset(Sender: TObject; var NewOffset: Integer;
      var Accept: Boolean);
    procedure tbMainClick(Sender: TObject);
    procedure tbMainResize(Sender: TObject);
    procedure ToggleFilterServerInfo(Sender: TObject);
    procedure ToggleStatusBar(Sender: TObject);
    procedure tsServerListsChange(Sender: TObject);
  private

  public

  end;

var
  fmMain: TfmMain;

implementation

{$R *.lfm}

{ TfmMain }

procedure TfmMain.ImportFavoritesClick(Sender: TObject);
begin

end;

procedure TfmMain.lbServersContextPopup(Sender: TObject; MousePos: TPoint;
  var Handled: Boolean);
begin

end;

procedure TfmMain.MasterServerUpdateClick(Sender: TObject);
begin

end;

procedure TfmMain.miViewClick(Sender: TObject);
begin

end;

procedure TfmMain.Panel1Click(Sender: TObject);
begin

end;

procedure TfmMain.pnBreakableResize(Sender: TObject);
begin

end;

procedure TfmMain.QuickConnectClick(Sender: TObject);
begin

end;

procedure TfmMain.RefreshServerClick(Sender: TObject);
begin

end;

procedure TfmMain.ServerPropertiesClick(Sender: TObject);
begin

end;

procedure TfmMain.SettingsClick(Sender: TObject);
begin

end;

procedure TfmMain.Splitter1CanOffset(Sender: TObject; var NewOffset: Integer;
  var Accept: Boolean);
begin

end;

procedure TfmMain.tbMainClick(Sender: TObject);
begin

end;

procedure TfmMain.tbMainResize(Sender: TObject);
begin

end;

procedure TfmMain.ToggleFilterServerInfo(Sender: TObject);
begin

end;

procedure TfmMain.ToggleStatusBar(Sender: TObject);
begin

end;

procedure TfmMain.tsServerListsChange(Sender: TObject);
begin

end;

procedure TfmMain.ExportFavoritesClick(Sender: TObject);
begin

end;

procedure TfmMain.FilterChange(Sender: TObject);
begin

end;

procedure TfmMain.GroupBox1Click(Sender: TObject);
begin

end;

procedure TfmMain.imLogoClick(Sender: TObject);
begin

end;

procedure TfmMain.ExitClick(Sender: TObject);
begin

end;

procedure TfmMain.ConnectClick(Sender: TObject);
begin

end;

procedure TfmMain.CopyServerInfoClick(Sender: TObject);
begin

end;

procedure TfmMain.DeleteServerClick(Sender: TObject);
begin

end;

procedure TfmMain.edFilterModeClick(Sender: TObject);
begin

end;

procedure TfmMain.AddServerClick(Sender: TObject);
begin

end;

procedure TfmMain.AboutClick(Sender: TObject);
begin

end;

end.

