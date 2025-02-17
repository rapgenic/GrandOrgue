/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOrgueStop.h"

#include "GOrgueConfigReader.h"
#include "GOrgueRank.h"
#include "GrandOrgueFile.h"
#include <wx/intl.h>

GOrgueStop::GOrgueStop(GrandOrgueFile* organfile, unsigned first_midi_note_number) :
	GOrgueDrawstop(organfile),
	m_RankInfo(0),
	m_KeyVelocity(0),
	m_FirstMidiNoteNumber(first_midi_note_number),
	m_FirstAccessiblePipeLogicalKeyNumber(0),
	m_NumberOfAccessiblePipes(0)
{
}

unsigned GOrgueStop::IsAuto() const
{
	/* m_auto seems to state that if a stop only has 1 note, the note isn't
	 * actually controlled by a manual, but will be on if the stop is on and
	 * off if the stop is off... */
	return (m_RankInfo.size() == 1 && m_RankInfo[0].Rank->GetPipeCount() == 1);
}

void GOrgueStop::Load(GOrgueConfigReader& cfg, wxString group)
{
	unsigned number_of_ranks = cfg.ReadInteger(ODFSetting, group, wxT("NumberOfRanks"), 0, 999, false, 0);

	m_FirstAccessiblePipeLogicalKeyNumber  = cfg.ReadInteger(ODFSetting, group, wxT("FirstAccessiblePipeLogicalKeyNumber"), 1,  128);
	m_NumberOfAccessiblePipes              = cfg.ReadInteger(ODFSetting, group, wxT("NumberOfAccessiblePipes"), 1, 192);

	if (number_of_ranks)
        {
		for(unsigned i = 0; i < number_of_ranks; i++)
		{
			RankInfo info;
			unsigned no = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Rank%03d"), i + 1), 1, m_organfile->GetODFRankCount());
			info.Rank = m_organfile->GetRank(no - 1); 
			info.FirstPipeNumber = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Rank%03dFirstPipeNumber"), i + 1), 1, info.Rank->GetPipeCount(), false, 1);
			info.PipeCount = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Rank%03dPipeCount"), i + 1), 1, info.Rank->GetPipeCount() - info.FirstPipeNumber + 1, false, info.Rank->GetPipeCount() - info.FirstPipeNumber + 1);
			info.FirstAccessibleKeyNumber = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Rank%03dFirstAccessibleKeyNumber"), i + 1), 1, m_NumberOfAccessiblePipes, false, 1);
			info.StopID = info.Rank->RegisterStop(this);
			m_RankInfo.push_back(info);
		}
        }
	else
	{
		RankInfo info;
		info.Rank = new GOrgueRank(m_organfile);
		m_organfile->AddRank(info.Rank);
		info.FirstPipeNumber = cfg.ReadInteger(ODFSetting, group, wxT("FirstAccessiblePipeLogicalPipeNumber"), 1, 192);
		info.FirstAccessibleKeyNumber = 1;
		info.PipeCount = m_NumberOfAccessiblePipes;
		info.Rank->Load(cfg, group, m_FirstMidiNoteNumber - info.FirstPipeNumber + info.FirstAccessibleKeyNumber + m_FirstAccessiblePipeLogicalKeyNumber - 1);
		info.StopID = info.Rank->RegisterStop(this);
		m_RankInfo.push_back(info);
	}

        m_KeyVelocity.resize(m_NumberOfAccessiblePipes);
        std::fill(m_KeyVelocity.begin(), m_KeyVelocity.end(), 0);
	m_StoreDivisional = m_organfile->CombinationsStoreNonDisplayedDrawstops() || IsDisplayed();
	m_StoreGeneral = m_organfile->CombinationsStoreNonDisplayedDrawstops() || IsDisplayed();

        GOrgueDrawstop::Load(cfg, group);
}

void GOrgueStop::SetupCombinationState()
{
	m_StoreDivisional = m_organfile->CombinationsStoreNonDisplayedDrawstops() || IsDisplayed();
	m_StoreGeneral = m_organfile->CombinationsStoreNonDisplayedDrawstops() || IsDisplayed();
}

void GOrgueStop::SetRankKey(unsigned key, unsigned velocity)
{
	for(unsigned j = 0; j < m_RankInfo.size(); j++)
	{
		if (key + 1 < m_RankInfo[j].FirstAccessibleKeyNumber || key >= m_RankInfo[j].FirstAccessibleKeyNumber + m_RankInfo[j].PipeCount)
			continue;
		m_RankInfo[j].Rank->SetKey(key + m_RankInfo[j].FirstPipeNumber - m_RankInfo[j].FirstAccessibleKeyNumber, velocity, m_RankInfo[j].StopID);
	}
}

void GOrgueStop::SetKey(unsigned note, unsigned velocity)
{
	if (note < m_FirstAccessiblePipeLogicalKeyNumber || note >= m_FirstAccessiblePipeLogicalKeyNumber + m_NumberOfAccessiblePipes)
		return;
	if (IsAuto())
		return;
	note -= m_FirstAccessiblePipeLogicalKeyNumber;
	
	if (m_KeyVelocity[note] == velocity)
		return;
	m_KeyVelocity[note] = velocity;
	if (IsActive())
		SetRankKey(note, m_KeyVelocity[note]);
}

void GOrgueStop::ChangeState(bool on)
{
	if (IsAuto())
	{
		SetRankKey(0, on ? 0x7f : 0x00);
	}
	else
	{
		for(unsigned i = 0; i < m_NumberOfAccessiblePipes; i++)
			SetRankKey(i, on ? m_KeyVelocity[i] : 0);
	}

}

GOrgueStop::~GOrgueStop(void)
{
}

void GOrgueStop::AbortPlayback()
{
	if (IsAuto())
		Set(false);
	GOrgueButton::AbortPlayback();
}

void GOrgueStop::PreparePlayback()
{
	GOrgueDrawstop::PreparePlayback();

	m_KeyVelocity.resize(m_NumberOfAccessiblePipes);
	std::fill(m_KeyVelocity.begin(), m_KeyVelocity.end(), 0);
}

void GOrgueStop::StartPlayback()
{
	GOrgueDrawstop::StartPlayback();

	if (IsAuto() && IsActive())
		SetRankKey(0, 0x7f);
}

GOrgueRank* GOrgueStop::GetRank(unsigned index)
{
	return m_RankInfo[index].Rank;
}

wxString GOrgueStop::GetMidiType()
{
	return _("Stop");
}
