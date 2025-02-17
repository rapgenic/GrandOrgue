/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GOSOUNDSAMPLERPOOL_H_
#define GOSOUNDSAMPLERPOOL_H_

#include "GOSoundSimpleSamplerList.h"
#include "threading/atomic.h"
#include "threading/GOMutex.h"
#include "ptrvector.h"

class GO_SAMPLER;

class GOSoundSamplerPool
{

private:
	GOMutex                 m_Lock;
	atomic_uint m_SamplerCount;
	unsigned                m_UsageLimit;
	GOSoundSimpleSamplerList m_AvailableSamplers;
	ptr_vector<GO_SAMPLER> m_Samplers;

public:
	GOSoundSamplerPool();
	GO_SAMPLER* GetSampler();
	void ReturnSampler(GO_SAMPLER* sampler);
	void ReturnAll();
	unsigned GetUsageLimit() const;
	void SetUsageLimit(unsigned count);
	unsigned UsedSamplerCount() const;
};

inline
unsigned GOSoundSamplerPool::GetUsageLimit() const
{
	return m_UsageLimit;
}

inline
unsigned GOSoundSamplerPool::UsedSamplerCount() const
{
	return m_SamplerCount;
}

#endif /* GOSOUNDSAMPLERPOOL_H_ */
