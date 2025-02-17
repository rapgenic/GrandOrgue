/*
 * GrandOrgue - a free pipe organ simulator
 *
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef GRANDORGUEFRAME_H
#define GRANDORGUEFRAME_H

#include "GOrgueEvent.h"
#include "GOrgueMidiCallback.h"
#include "GOrgueMidiListener.h"
#include "threading/GOMutex.h"
#include <vector>
#include <wx/dcmemory.h>
#include <wx/frame.h>

class GOrgueApp;
class GOrgueDocument;
class GOrgueMidiEvent;
class GOrgueOrgan;
class GOrgueSettings;
class GOrgueSound;
class wxChoice;
class wxGaugeAudio;
class wxHtmlHelpController;
class wxSpinCtrl;

class GOrgueFrame: public wxFrame, protected GOrgueMidiCallback
{
private:
  GOrgueApp &m_App;
  GOMutex m_mutex;
  wxMenu* m_file_menu;
  wxMenu* m_audio_menu;
  wxMenu* m_panel_menu;
  wxMenu* m_favorites_menu;
  wxMenu* m_recent_menu;
  wxMenu* m_temperament_menu;
  GOrgueDocument* m_doc;
  wxHtmlHelpController* m_Help;
  wxGaugeAudio *m_SamplerUsage;
  wxControl* m_VolumeControl;
  std::vector<wxGaugeAudio*> m_VolumeGauge;
  wxSpinCtrl* m_Transpose;
  wxChoice* m_ReleaseLength;
  wxSpinCtrl* m_Polyphony;
  wxSpinCtrl* m_SetterPosition;
  wxSpinCtrl* m_Volume;
  GOrgueSound& m_Sound;
  GOrgueSettings& m_Settings;
  GOrgueMidiListener m_listener;
  wxString m_Title;
  wxString m_Label;
  bool m_MidiMonitor;
  bool m_isMeterReady;
  
  // to avoid event processing when the settings dialog is open
  bool m_InSettings;
  wxEventType m_AfterSettingsEventType;
  int m_AfterSettingsEventId;
  GOrgueOrgan* p_AfterSettingsEventOrgan;
  
  void InitHelp();
  void UpdatePanelMenu();
  void UpdateFavoritesMenu();
  void UpdateRecentMenu();
  void UpdateTemperamentMenu();
  bool AdjustVolumeControlWithSettings();
  void UpdateSize();
  void UpdateVolumeControlWithSettings();

  GOrgueDocument* GetDocument();

  void OnMeters(wxCommandEvent& event);

  void OnLoadFile(wxCommandEvent& event);
  void OnLoad(wxCommandEvent& event);
  void OnLoadFavorite(wxCommandEvent& event);
  void OnLoadRecent(wxCommandEvent& event);
  void OnInstall(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnClose(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnImportSettings(wxCommandEvent& event);
  void OnImportCombinations(wxCommandEvent& event);
  void OnExport(wxCommandEvent& event);
  void OnCache(wxCommandEvent& event);
  void OnCacheDelete(wxCommandEvent& event);
  void OnReload(wxCommandEvent& event);
  void OnRevert(wxCommandEvent& event);
  void OnProperties(wxCommandEvent& event);

  void OnEditOrgan(wxCommandEvent& event);
  void OnMidiList(wxCommandEvent& event);

  void OnAudioPanic(wxCommandEvent& event);
  void OnAudioMemset(wxCommandEvent& event);
  void OnAudioState(wxCommandEvent& event);

  void SetEventAfterSettings(
    wxEventType eventType, int eventId, GOrgueOrgan* pOrganFile=NULL
  );

  void OnSettings(wxCommandEvent& event);
  void OnMidiLoad(wxCommandEvent& event);
  void OnMidiMonitor(wxCommandEvent& event);

  void OnPreset(wxCommandEvent& event);
  void OnTemperament(wxCommandEvent& event);
  void OnHelp(wxCommandEvent& event);
  void OnHelpAbout(wxCommandEvent& event);
  void OnShowHelp(wxCommandEvent& event);

  void OnSettingsVolume(wxCommandEvent& event);
  void OnSettingsPolyphony(wxCommandEvent& event);
  void OnSettingsMemory(wxCommandEvent& event);
  void OnSettingsMemoryEnter(wxCommandEvent& event);
  void OnSettingsTranspose(wxCommandEvent& event);
  void OnSettingsReleaseLength(wxCommandEvent& event);

  void OnKeyCommand(wxKeyEvent& event);
  void OnChangeTranspose(wxCommandEvent& event);
  void OnChangeSetter(wxCommandEvent& event);
  void OnChangeVolume(wxCommandEvent& event);
  void OnPanel(wxCommandEvent& event);

  void OnSize(wxSizeEvent& event);

  void OnMenuOpen(wxMenuEvent& event);
  void OnCloseWindow(wxCloseEvent& event);

  void OnMidiEvent(const GOrgueMidiEvent& event);

  void OnUpdateLoaded(wxUpdateUIEvent& event);
  void OnSetTitle(wxCommandEvent& event);
  void OnMsgBox(wxMsgBoxEvent& event);
  void OnRenameFile(wxRenameFileEvent& event);

  bool DoClose();
  void Open(const GOrgueOrgan& organ);

  bool InstallOrganPackage(wxString name);
  void LoadLastOrgan();
  void LoadFirstOrgan();
  void SendLoadOrgan(const GOrgueOrgan& organ);

public:
  GOrgueFrame(GOrgueApp &app, wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long type, GOrgueSound& sound);
  virtual ~GOrgueFrame(void);

  void Init(wxString filename);
  bool Close();

  void DoSplash(bool timeout = true);

  void SendLoadFile(wxString filename);

  void ApplyRectFromSettings(wxRect rect);

  DECLARE_EVENT_TABLE()
};

#endif
