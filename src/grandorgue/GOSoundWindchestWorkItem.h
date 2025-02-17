/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GOSOUNDWINDCHESTWORKITEM_H
#define GOSOUNDWINDCHESTWORKITEM_H

#include "GOSoundWorkItem.h"
#include "threading/GOMutex.h"
#include "ptrvector.h"

class GOSoundEngine;
class GOSoundTremulantWorkItem;
class GOrgueWindchest;

class GOSoundWindchestWorkItem : public GOSoundWorkItem
{
private:
	GOSoundEngine& m_engine;
	GOMutex m_Mutex;
	float m_Volume;
	bool m_Done;
	GOrgueWindchest* m_Windchest;
	std::vector<GOSoundTremulantWorkItem*> m_Tremulants;

public:
	GOSoundWindchestWorkItem(GOSoundEngine& sound_engine, GOrgueWindchest* windchest);

	unsigned GetGroup();
	unsigned GetCost();
	bool GetRepeat();
	void Run(GOSoundThread *thread = nullptr);
	void Exec();

	void Clear();
	void Reset();
	void Init(ptr_vector<GOSoundTremulantWorkItem>& tremulants);

	float GetWindchestVolume();
	float GetVolume()
	{
		if (!m_Done)
			Run();
		return m_Volume;
	}
};

#endif
