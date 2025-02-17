/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "SettingsMidiDevices.h"

#include "GOrgueMidi.h"
#include "GOrgueSettings.h"
#include "GOrgueSound.h"
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/choice.h>
#include <wx/choicdlg.h> 
#include <wx/numdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

BEGIN_EVENT_TABLE(SettingsMidiDevices, wxPanel)
	EVT_LISTBOX(ID_INDEVICES, SettingsMidiDevices::OnInDevicesClick)
	EVT_LISTBOX_DCLICK(ID_INDEVICES, SettingsMidiDevices::OnInOutDeviceClick)
	EVT_BUTTON(ID_INCHANNELSHIFT, SettingsMidiDevices::OnInChannelShiftClick)
	EVT_BUTTON(ID_INOUTDEVICE, SettingsMidiDevices::OnInOutDeviceClick)
END_EVENT_TABLE()

SettingsMidiDevices::SettingsMidiDevices(GOrgueSound& sound, wxWindow* parent) :
	wxPanel(parent, wxID_ANY),
	m_Sound(sound)
{
	m_Sound.GetMidi().UpdateDevices();

	wxArrayString choices;
	std::vector<wxString> list = m_Sound.GetMidi().GetInDevices();
	std::vector<bool> state;
	for (unsigned i = 0; i < list.size(); i++)
	{
		choices.push_back(list[i]);
		m_InDeviceData.push_back(m_Sound.GetSettings().GetMidiInDeviceChannelShift(list[i]));
		m_InOutDeviceData.push_back(m_Sound.GetSettings().GetMidiInOutDevice(list[i]));
		state.push_back(m_Sound.GetSettings().GetMidiInState(list[i]));
	}

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* item3 = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI &input devices"));
	m_InDevices = new wxCheckListBox(this, ID_INDEVICES, wxDefaultPosition, wxSize(100, 200), choices);
	for (unsigned i = 0; i < state.size(); i++)
	{
		if (state[i])
			m_InDevices->Check(i);
	}
	item3->Add(m_InDevices, 1, wxEXPAND | wxALL, 5);

	wxBoxSizer* item4 = new wxBoxSizer(wxHORIZONTAL);
	m_InProperties = new wxButton(this, ID_INCHANNELSHIFT, _("A&dvanced..."));
	m_InProperties->Disable();
	item4->Add(m_InProperties, 0, wxALL, 5);
	m_InOutDevice = new wxButton(this, ID_INOUTDEVICE, _("&MIDI-Output-Device..."));
	m_InOutDevice->Disable();
	item4->Add(m_InOutDevice, 0, wxALL, 5);
	item3->Add(item4, 0, wxALIGN_RIGHT | wxALL, 5);
	topSizer->Add(item3, 1, wxEXPAND | wxALL, 5);

	choices.clear();
	std::vector<bool> out_state;
	list = m_Sound.GetMidi().GetOutDevices();
	for (unsigned i = 0; i < list.size(); i++)
	{
		choices.push_back(list[i]);
		out_state.push_back(m_Sound.GetSettings().GetMidiOutState(list[i]) == 1);
	}

	item3 = new wxStaticBoxSizer(wxVERTICAL, this, _("MIDI &output devices"));
	m_OutDevices = new wxCheckListBox(this, ID_OUTDEVICES, wxDefaultPosition, wxSize(100, 200), choices);
	for (unsigned i = 0; i < out_state.size(); i++)
		if (out_state[i])
			m_OutDevices->Check(i);

	item3->Add(m_OutDevices, 1, wxEXPAND | wxALL, 5);
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	item3->Add(box);
	box->Add(new wxStaticText(this, wxID_ANY, _("Send MIDI Recorder Output Stream to ")), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	m_RecorderDevice = new wxChoice(this, ID_RECORDERDEVICE, wxDefaultPosition, wxSize(400, wxDefaultCoord));
	box->Add(m_RecorderDevice, 1, wxEXPAND | wxALL, 5);
	m_RecorderDevice->Append(_("No device"));
	m_RecorderDevice->Select(0);
	list = m_Sound.GetMidi().GetOutDevices();
	for (unsigned i = 0; i < list.size(); i++)
	{
		m_RecorderDevice->Append(list[i]);
		if (m_Sound.GetSettings().MidiRecorderOutputDevice() == list[i])
			m_RecorderDevice->SetSelection(m_RecorderDevice->GetCount() - 1);
	}
	topSizer->Add(item3, 1, wxEXPAND | wxALL, 5);

	topSizer->AddSpacer(5);
	this->SetSizer(topSizer);
	topSizer->Fit(this);
}

void SettingsMidiDevices::OnInDevicesClick(wxCommandEvent& event)
{
	m_InProperties->Enable();
	m_InOutDevice->Enable();
}

void SettingsMidiDevices::OnInOutDeviceClick(wxCommandEvent& event)
{
	int index = m_InDevices->GetSelection();
	wxArrayString choices = m_RecorderDevice->GetStrings();
	choices[0] = _("No device");
	int selection = 0;
	for(unsigned i = 1; i < choices.GetCount(); i++)
		if (choices[i] == m_InOutDeviceData[index])
			selection = i;
	int result = wxGetSingleChoiceIndex(_("Select the corresponding MIDI output device for converting input events into output events"), _("MIDI output device"), choices, selection, this);
	if (result == -1)
		return;
	if (result)
		m_InOutDeviceData[index] = choices[result];
	else
		m_InOutDeviceData[index] = wxEmptyString;
}

void SettingsMidiDevices::OnInChannelShiftClick(wxCommandEvent& event)
{
	m_InProperties->Enable();
	m_InOutDevice->Enable();
	int index = m_InDevices->GetSelection();
	int result = ::wxGetNumberFromUser(_("A channel offset allows the use of two MIDI\ninterfaces with conflicting MIDI channels. For\nexample, applying a channel offset of 8 to\none of the MIDI interfaces would cause that\ninterface's channel 1 to appear as channel 9,\nchannel 2 to appear as channel 10, and so on."), _("Channel offset:"), 
					   m_InDevices->GetString(index), m_InDeviceData[index], 0, 15, this);

	if (result >= 0)
		m_InDeviceData[index] = result;
}

void SettingsMidiDevices::Save()
{
	for (unsigned i = 0; i < m_InDevices->GetCount(); i++)
	{
		m_Sound.GetSettings().SetMidiInState(m_InDevices->GetString(i), m_InDevices->IsChecked(i));
		m_Sound.GetSettings().SetMidiInDeviceChannelShift(m_InDevices->GetString(i), m_InDeviceData[i]);
		m_Sound.GetSettings().SetMidiInOutDevice(m_InDevices->GetString(i), m_InOutDeviceData[i]);
	}

	for (unsigned i = 0; i < m_OutDevices->GetCount(); i++)
	{
		m_Sound.GetSettings().SetMidiOutState(m_OutDevices->GetString(i), m_OutDevices->IsChecked(i));
	}
	if (m_RecorderDevice->GetSelection() == 0)
		m_Sound.GetSettings().MidiRecorderOutputDevice(wxEmptyString);
	else
		m_Sound.GetSettings().MidiRecorderOutputDevice(m_RecorderDevice->GetString(m_RecorderDevice->GetSelection()));
}
