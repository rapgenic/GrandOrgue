/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOrgueMidiPlayerContent.h"

#include "GOrgueMidiEvent.h"
#include "GOrgueMidiFileReader.h"
#include "GOrgueMidiMap.h"
#include "GOrgueMidiMerger.h"

GOrgueMidiPlayerContent::GOrgueMidiPlayerContent() :
	m_Events(),
	m_Pos(0)
{
	Clear();
}

GOrgueMidiPlayerContent::~GOrgueMidiPlayerContent()
{
}

void GOrgueMidiPlayerContent::Clear()
{
	m_Events.clear();
	Reset();
}

void GOrgueMidiPlayerContent::Reset()
{
	m_Pos = 0;
}

bool GOrgueMidiPlayerContent::IsLoaded()
{
	return m_Events.size() > 0;
}

void GOrgueMidiPlayerContent::ReadFileContent(GOrgueMidiFileReader& reader, std::vector<GOrgueMidiEvent>& events)
{
	unsigned pos = 0;
	GOrgueMidiEvent e;
	while(reader.ReadEvent(e))
	{
		while (pos > 0 && events[pos - 1].GetTime() > e.GetTime())
		{
			pos--;
		}
		while(pos < events.size() && events[pos].GetTime() < e.GetTime())
		{
			pos++;
		}
		if (pos <= events.size())
			events.insert(events.begin() + pos, e);
		else
			events.push_back(e);
		pos++;
	}
}

void GOrgueMidiPlayerContent::SetupManual(GOrgueMidiMap& map, unsigned channel, wxString ID)
{
	GOrgueMidiEvent e;
	e.SetTime(0);

	e.SetMidiType(MIDI_SYSEX_GO_SETUP);
	e.SetKey(map.GetElementByString(ID));
	e.SetChannel(channel);
	e.SetValue(0);
	m_Events.push_back(e);

	e.SetMidiType(MIDI_CTRL_CHANGE);
	e.SetChannel(channel);
	e.SetKey(MIDI_CTRL_NOTES_OFF);
	e.SetValue(0);
	m_Events.push_back(e);
}

bool GOrgueMidiPlayerContent::Load(GOrgueMidiFileReader& reader, GOrgueMidiMap& map, unsigned manuals, bool pedal)
{
	Clear();
	std::vector<GOrgueMidiEvent> events;
	ReadFileContent(reader, events);

	GOrgueMidiMerger merger;
	merger.Clear();
	if (events.size() && events[0].GetMidiType() != MIDI_SYSEX_GO_CLEAR)
	{
		GOrgueMidiEvent e;
		e.SetTime(0);
		e.SetMidiType(MIDI_SYSEX_GO_CLEAR);
		e.SetChannel(0);
		m_Events.push_back(e);

		for(unsigned i = 1; i <= manuals; i++)
			SetupManual(map, i, wxString::Format(wxT("M%d"), i));
		if (pedal)
			SetupManual(map, manuals + 1, wxString::Format(wxT("M%d"), 0));
	}
	for(unsigned i = 0; i < events.size(); i++)
		if (merger.Process(events[i]))
			m_Events.push_back(events[i]);

	return true;
}

const GOrgueMidiEvent& GOrgueMidiPlayerContent::GetCurrentEvent()
{
	return m_Events[m_Pos];
}

bool GOrgueMidiPlayerContent::Next()
{
	m_Pos++;
	return m_Pos < m_Events.size();
}
