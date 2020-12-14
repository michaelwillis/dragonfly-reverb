#define AppName "DragonflyReverbs"
#define AppVersion "3.2.2"
#define AppPublisher "PlugInGuru"
#define AppURL "https://github.com/pluginguru/dragonfly-reverb"
#define BuildRoot "D:\Documents\Github\dragonfly-reverb\juce-plugins"

[Setup]
AppId={{844AE577-E8B3-4C99-BBD2-C9BB3942A627}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}

WizardStyle=modern
OutputDir="."
OutputBaseFilename="DragonflySetup_{#AppVersion}"
DefaultDirName={autopf}\PlugInGuru\Unify
DefaultGroupName=PlugInGuru
;UninstallDisplayIcon={app}\Unify.exe
;InfoBeforeFile=".\text\install-before.txt"
LicenseFile="GPL3.txt"

Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
RestartIfNeededByRun=no
DisableDirPage=yes
AlwaysShowComponentsList=yes
AlwaysShowDirOnReadyPage=yes

[Types]
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "vst2_64"; Description: "64-bit VST2 Plugin (.dll)";
Name: "vst3_64"; Description: "64-bit VST3 Plugin (.vst3)";

; Unify app/plug-ins
[Files]
Source: "{#BuildRoot}\DragonflyER\Builds\VisualStudio2019\x64\Release\VST\Dragonfly ER.dll"; DestDir: {code:GetVST2Dir_64}; Components:vst2_64;
Source: "{#BuildRoot}\DragonflyHall\Builds\VisualStudio2019\x64\Release\VST\Dragonfly Hall.dll"; DestDir: {code:GetVST2Dir_64}; Components:vst2_64;
Source: "{#BuildRoot}\DragonflyPlate\Builds\VisualStudio2019\x64\Release\VST\Dragonfly Plate.dll"; DestDir: {code:GetVST2Dir_64}; Components:vst2_64;
Source: "{#BuildRoot}\DragonflyRoom\Builds\VisualStudio2019\x64\Release\VST\Dragonfly Room.dll"; DestDir: {code:GetVST2Dir_64}; Components:vst2_64;

Source: "{#BuildRoot}\DragonflyER\Builds\VisualStudio2019\x64\Release\VST3\Dragonfly ER.vst3"; DestDir: "{commoncf64}\VST3\"; Components:vst3_64;
Source: "{#BuildRoot}\DragonflyHall\Builds\VisualStudio2019\x64\Release\VST3\Dragonfly Hall.vst3"; DestDir: "{commoncf64}\VST3\"; Components:vst3_64;
Source: "{#BuildRoot}\DragonflyPlate\Builds\VisualStudio2019\x64\Release\VST3\Dragonfly Plate.vst3"; DestDir: "{commoncf64}\VST3\"; Components:vst3_64;
Source: "{#BuildRoot}\DragonflyRoom\Builds\VisualStudio2019\x64\Release\VST3\Dragonfly Room.vst3"; DestDir: "{commoncf64}\VST3\"; Components:vst3_64;


; Extract and install VC redistributable only if not already done
; See https://stackoverflow.com/questions/24574035/how-to-install-microsoft-vc-redistributables-silently-in-inno-setup
[Files]
; VC++ redistributable runtime. Extracted by VCRedistNeedsInstall(), if needed.
Source: "VC_redist.x64.exe"; DestDir: {tmp}; Flags: dontcopy

[Run]
Filename: "{tmp}\VC_redist.x64.exe"; StatusMsg: "Installing VC++ 2019 Redistributables..."; Parameters: "/quiet /norestart"; Check: VCRedistNeedsInstall ; Flags: waituntilterminated

[Code]
var
  VST2DirPage_64: TInputDirWizardPage;


function GetVST2Dir_64(Param: String): String;
begin
  Result := VST2DirPage_64.Values[0]
end;


function VCRedistNeedsInstall: Boolean;
var 
  Version: String;
begin
  if (RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Version', Version)) then
  begin
    // Is the installed version at least 14.24 ? 
    Log('VC Redist Version check : found ' + Version);
    Result := (CompareStr(Version, 'v14.24.28127.04')<0);
  end
  else 
  begin
    // Not even an old version installed
    Result := True;
  end;
  if (Result) then
  begin
    ExtractTemporaryFile('VC_redist.x64.exe');
  end;
end;

procedure InitializeWizard;
var
    UserVST2Dir: String;
begin
    WizardSelectComponents('vst2_64');
    WizardSelectComponents('vst3_64');

    VST2DirPage_64 := CreateInputDirPage(wpSelectComponents,
    '64-Bit VST2 Plugin Directory', '',
    'Select the folder in which setup should install the 64-bit VST2 Plugin, then click Next.',
    False, '');
    VST2DirPage_64.Add('');

    if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'Software\PlugInGuru\DragonflyReverbs', 'VST2Dir_64', UserVST2Dir) then
      VST2DirPage_64.Values[0] := UserVST2Dir
    else
      VST2DirPage_64.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{cf64}\VST2}\');
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := False;
  if PageID = VST2DirPage_64.ID then
    { if the component is not selected, skip the page }
    Result := not WizardIsComponentSelected('vst2_64');
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;

  if CurPageID = wpSelectComponents then
  begin
    if WizardSelectedComponents(False) = '' then
    begin
      MsgBox('No component selected', mbInformation, MB_OK);
      Result := False;
    end;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep=ssPostInstall then begin
     RegWriteStringValue(HKEY_LOCAL_MACHINE, 'Software\PlugInGuru\DragonflyReverbs', 'VST2Dir_64', GetVST2Dir_64(''));
  end;
end;
