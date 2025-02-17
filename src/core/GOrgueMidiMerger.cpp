/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOrgueMidiMerger.h"

#include "GOrgueMidiEvent.h"
#include <string.h>

GOrgueMidiMerger::GOrgueMidiMerger()
{
	Clear();
}

void GOrgueMidiMerger::Clear()
{
	memset(m_BankMsb, 0, sizeof(m_BankMsb));
	memset(m_BankLsb, 0, sizeof(m_BankLsb));
	memset(m_RpnMsb, 0, sizeof(m_RpnMsb));
	memset(m_RpnLsb, 0, sizeof(m_RpnLsb));
	memset(m_NrpnMsb, 0, sizeof(m_NrpnMsb));
	memset(m_NrpnLsb, 0, sizeof(m_NrpnLsb));
	m_Rpn = false;
}

bool GOrgueMidiMerger::Process(GOrgueMidiEvent& e)
{
	if (e.GetMidiType() == MIDI_CTRL_CHANGE && e.GetKey() == MIDI_CTRL_BANK_SELECT_MSB)
	{
		m_BankMsb[e.GetChannel() - 1] = e.GetValue();
		return false;
	}
	if (e.GetMidiType() == MIDI_CTRL_CHANGE && e.GetKey() == MIDI_CTRL_BANK_SELECT_LSB)
	{
		m_BankLsb[e.GetChannel() - 1] = e.GetValue();
		return false;
	}
	if (e.GetMidiType() == MIDI_CTRL_CHANGE && e.GetKey() == MIDI_CTRL_RPN_LSB)
	{
		m_RpnLsb[e.GetChannel() - 1] = e.GetValue();
		m_Rpn = true;
		return false;
	}
	if (e.GetMidiType() == MIDI_CTRL_CHANGE && e.GetKey() == MIDI_CTRL_RPN_MSB)
	{
		m_RpnMsb[e.GetChannel() - 1] = e.GetValue();
		m_Rpn = true;
		return false;
	}
	if (e.GetMidiType() == MIDI_CTRL_CHANGE && e.GetKey() == MIDI_CTRL_NRPN_LSB)
	{
		m_NrpnLsb[e.GetChannel() - 1] = e.GetValue();
		m_Rpn = false;
		return false;
	}
	if (e.GetMidiType() == MIDI_CTRL_CHANGE && e.GetKey() == MIDI_CTRL_NRPN_MSB)
	{
		m_NrpnMsb[e.GetChannel() - 1] = e.GetValue();
		m_Rpn = false;
		return false;
	}
	if (e.GetMidiType() == MIDI_PGM_CHANGE)
		e.SetKey(((e.GetKey() - 1) | (m_BankLsb[e.GetChannel() - 1] << 7) | (m_BankMsb[e.GetChannel() - 1] << 14)) + 1);

	if (e.GetMidiType() == MIDI_CTRL_CHANGE && e.GetKey() == MIDI_CTRL_DATA_ENTRY)
	{
		if (m_Rpn)
		{
			e.SetMidiType(MIDI_RPN);
			e.SetKey((m_RpnLsb[e.GetChannel() - 1] << 0) | (m_RpnMsb[e.GetChannel() - 1] << 7));
		}
		else
		{
			e.SetMidiType(MIDI_NRPN);
			e.SetKey((m_NrpnLsb[e.GetChannel() - 1] << 0) | (m_NrpnMsb[e.GetChannel() - 1] << 7));
		}
	}

	return true;
}
