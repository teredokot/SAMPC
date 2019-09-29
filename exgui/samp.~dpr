program samp;

uses
  Windows,
  Forms,
  Main in 'Main.pas' {fmMain},
  Rcon in 'Rcon.pas' {fmRcon},
  About in 'About.pas' {fmAbout},
  RconConfig in 'RconConfig.pas' {fmRconConfig},
  Settings in 'Settings.pas' {fmSettings},
  ServerProperties in 'ServerProperties.pas' {fmServerProperties},
  ImportFavorites in 'ImportFavorites.pas' {fmImportFavorites},
  ExportFavorites in 'ExportFavorites.pas' {fmExportFavorites};

{$R *.res}

begin
  CreateMutex(nil, true, 'kyeman and spookie woz ''ere, innit.');
  if GetLastError = ERROR_ALREADY_EXISTS then begin
    MessageBox(0, 'SA:MP is already running.'#10#10'You can only run one instance at a time.', 'SA:MP Error', MB_ICONERROR);
    ExitProcess(0);
  end;

  Application.Initialize;
  Application.Title := 'SA:MP 0.2X';
  Application.CreateForm(TfmMain, fmMain);
  Application.CreateForm(TfmAbout, fmAbout);
  Application.Run;
end.
