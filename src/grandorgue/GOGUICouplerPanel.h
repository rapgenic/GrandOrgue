/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GGUICOUPLERPANEL_H
#define GGUICOUPLERPANEL_H

#include "GOGUIPanelCreator.h"

class GOGUIPanel;
class GrandOrgueFile;

class GOGUICouplerPanel : public GOGUIPanelCreator
{
private:
	GrandOrgueFile* m_organfile;

	GOGUIPanel* CreateCouplerPanel(GOrgueConfigReader& cfg, unsigned manual_nr);

public:
	GOGUICouplerPanel(GrandOrgueFile* organfile);
	~GOGUICouplerPanel();

	void CreatePanels(GOrgueConfigReader& cfg);
};

#endif
