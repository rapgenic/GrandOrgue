/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOrgueDocumentBase.h"

#include "GOrgueView.h"

GOrgueDocumentBase::GOrgueDocumentBase() :
	m_Windows()
{
}

GOrgueDocumentBase::~GOrgueDocumentBase()
{
}

bool GOrgueDocumentBase::WindowExists(WindowType type, void* data)
{
	for(unsigned i = 0; i < m_Windows.size(); i++)
		if (m_Windows[i].type == type && m_Windows[i].data == data)
			return true;
	return false;
}

bool GOrgueDocumentBase::showWindow(WindowType type, void* data)
{
	for(unsigned i = 0; i < m_Windows.size(); i++)
		if (m_Windows[i].type == type && m_Windows[i].data == data)
		{
			m_Windows[i].window->ShowView();
			return true;
		}
	return false;
}

void GOrgueDocumentBase::registerWindow(WindowType type, void* data, GOrgueView *window)
{
	WindowInfo info;
	info.type = type;
	info.data = data;
	info.window = window;
	m_Windows.push_back(info);
	window->ShowView();
}

void GOrgueDocumentBase::unregisterWindow(GOrgueView* window)
{
	for(unsigned i = 0; i < m_Windows.size(); i++)
		if (m_Windows[i].window == window)
		{
			m_Windows.erase(m_Windows.begin() + i);
			return;
		}
}

void GOrgueDocumentBase::SyncState()
{
	for(unsigned i = 0; i < m_Windows.size(); i++)
		if (m_Windows[i].data)
			m_Windows[i].window->SyncState();
}

void GOrgueDocumentBase::CloseWindows()
{
	while(m_Windows.size() > 0)
	{
		GOrgueView* wnd = m_Windows[0].window;
		m_Windows.erase(m_Windows.begin());
		wnd->RemoveView();
	}
}
