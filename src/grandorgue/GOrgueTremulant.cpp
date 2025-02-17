/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOrgueTremulant.h"

#include "GOSoundProviderSynthedTrem.h"
#include "GOrgueConfigReader.h"
#include "GrandOrgueFile.h"
#include <wx/intl.h>

#define DELETE_AND_NULL(x) do { if (x) { delete x; x = NULL; } } while (0)

const struct IniFileEnumEntry GOrgueTremulant::m_tremulant_types[]={
	{ wxT("Synth"), GOSynthTrem },
	{ wxT("Wave"), GOWavTrem },
};

GOrgueTremulant::GOrgueTremulant(GrandOrgueFile* organfile) :
	GOrgueDrawstop(organfile),
	m_TremulantType(GOSynthTrem),
	m_Period(0),
	m_StartRate(0),
	m_StopRate(0),
	m_AmpModDepth(0),
	m_TremProvider(NULL),
	m_PlaybackHandle(0),
	m_LastStop(0),
	m_SamplerGroupID(0)
{
}

GOrgueTremulant::~GOrgueTremulant()
{
	DELETE_AND_NULL(m_TremProvider);
}

void GOrgueTremulant::Initialize()
{
}

void GOrgueTremulant::LoadData()
{
	InitSoundProvider();
}

bool GOrgueTremulant::LoadCache(GOrgueCache& cache)
{
	InitSoundProvider();
	return true;
}

bool GOrgueTremulant::SaveCache(GOrgueCacheWriter& cache)
{
	return true;
}

void GOrgueTremulant::UpdateHash(GOrgueHash& ctx)
{
}

const wxString& GOrgueTremulant::GetLoadTitle()
{
	return m_Name;
}

void GOrgueTremulant::Load(GOrgueConfigReader& cfg, wxString group, int sampler_group_id)
{
	m_TremulantType = (GOrgueTremulantType)cfg.ReadEnum(ODFSetting, group, wxT("TremulantType"), m_tremulant_types, sizeof(m_tremulant_types) / sizeof(m_tremulant_types[0]), false, GOSynthTrem);
	if (m_TremulantType == GOSynthTrem)
	{
		m_TremProvider = new GOSoundProviderSynthedTrem(m_organfile->GetMemoryPool()),
		m_Period            = cfg.ReadLong(ODFSetting, group, wxT("Period"), 32, 441000);
		m_StartRate         = cfg.ReadInteger(ODFSetting, group, wxT("StartRate"), 1, 100);
		m_StopRate          = cfg.ReadInteger(ODFSetting, group, wxT("StopRate"), 1, 100);
		m_AmpModDepth       = cfg.ReadInteger(ODFSetting, group, wxT("AmpModDepth"), 1, 100);
		m_SamplerGroupID    = sampler_group_id;
		m_PlaybackHandle    = 0;
	}
	GOrgueDrawstop::Load(cfg, group);
	m_organfile->RegisterCacheObject(this);
}

void GOrgueTremulant::SetupCombinationState()
{
	m_StoreDivisional = m_organfile->DivisionalsStoreTremulants() && (m_organfile->CombinationsStoreNonDisplayedDrawstops() || IsDisplayed());
	m_StoreGeneral = m_organfile->CombinationsStoreNonDisplayedDrawstops() || IsDisplayed();
}

void GOrgueTremulant::InitSoundProvider()
{
	if (m_TremulantType == GOSynthTrem)
	{
		((GOSoundProviderSynthedTrem*)m_TremProvider)->Create(m_Period, m_StartRate, m_StopRate, m_AmpModDepth);
		assert(!m_TremProvider->IsOneshot());
	}
}

void GOrgueTremulant::ChangeState(bool on)
{
	if (m_TremulantType == GOSynthTrem)
	{
		if (on)
		{
			assert(m_SamplerGroupID < 0);
			m_PlaybackHandle = m_organfile->StartSample(m_TremProvider, m_SamplerGroupID, 0, 0x7f, 0, m_LastStop);
			on = (m_PlaybackHandle != NULL);
		}
		else
		{
			assert(m_PlaybackHandle);
			m_LastStop = m_organfile->StopSample(m_TremProvider, m_PlaybackHandle);
			m_PlaybackHandle = NULL;
		}
	}
	if (m_TremulantType == GOWavTrem)
	{
		m_organfile->UpdateTremulant(this);
	}
}

void GOrgueTremulant::AbortPlayback()
{
	m_PlaybackHandle = NULL;
	m_LastStop = 0;
	GOrgueButton::AbortPlayback();
}

void GOrgueTremulant::StartPlayback()
{
	GOrgueDrawstop::StartPlayback();

	if (IsActive() && m_TremulantType == GOSynthTrem)
	{
		assert(m_SamplerGroupID < 0);
		m_PlaybackHandle = m_organfile->StartSample(m_TremProvider, m_SamplerGroupID, 0, 0x7f, 0, 0);
	}
	if (m_TremulantType == GOWavTrem)
	{
		m_organfile->UpdateTremulant(this);
	}
}

GOrgueTremulantType GOrgueTremulant::GetTremulantType()
{
	return m_TremulantType;
}

wxString GOrgueTremulant::GetMidiType()
{
	return _("Tremulant");
}
