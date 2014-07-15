/*
 * GrandOrgue - free pipe organ simulator
 *
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2014 GrandOrgue contributors (see AUTHORS)
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

#include "GOGUIDisplayMetrics.h"

#include "GOGUIEnclosure.h"
#include "GOGUIManual.h"
#include "GOrgueManual.h"
#include "GrandOrgueFile.h"
#include <wx/font.h>
#include <wx/intl.h>

GOGUIDisplayMetrics::GOGUIDisplayMetrics(GrandOrgueFile* organfile, wxString group) :
	m_group(group),
	m_organfile(organfile),
	m_ManualRenderInfo(),
	m_Enclosures(),
	m_Manuals(),
	m_DispScreenSizeHoriz(0),
	m_DispScreenSizeVert(0),
	m_DispDrawstopBackgroundImageNum(0),
	m_DispConsoleBackgroundImageNum(0),
	m_DispKeyHorizBackgroundImageNum(0),
	m_DispKeyVertBackgroundImageNum(0),
	m_DispDrawstopInsetBackgroundImageNum(0),
	m_DispControlLabelFont(),
	m_DispShortcutKeyLabelFont(),
	m_DispShortcutKeyLabelColour(),
	m_DispGroupLabelFont(),
	m_DispDrawstopCols(0),
	m_DispDrawstopRows(0),
	m_DispDrawstopColsOffset(false),
	m_DispDrawstopOuterColOffsetUp(false),
	m_DispPairDrawstopCols(false),
	m_DispExtraDrawstopRows(0),
	m_DispExtraDrawstopCols(0),
	m_DispButtonCols(0),
	m_DispExtraButtonRows(0),
	m_DispExtraPedalButtonRow(false),
	m_DispExtraPedalButtonRowOffset(false),
	m_DispExtraPedalButtonRowOffsetRight(false),
	m_DispButtonsAboveManuals(false),
	m_DispTrimAboveManuals(false),
	m_DispTrimBelowManuals(false),
	m_DispTrimAboveExtraRows(false),
	m_DispExtraDrawstopRowsAboveExtraButtonRows(false),
	m_DrawStopWidth(78),
	m_DrawStopHeight(69),
	m_ButtonWidth(44),
	m_ButtonHeight(40),
	m_EnclosureWidth(52),
	m_EnclosureHeight(63),
	m_PedalHeight(40),
	m_PedalKeyWidth(7),
	m_ManualHeight(32),
	m_ManualKeyWidth(12),
	m_HackY(0),
	m_EnclosureY(0),
	m_CenterY(0),
	m_CenterWidth(0)
{
}

GOGUIDisplayMetrics::~GOGUIDisplayMetrics()
{
}

void GOGUIDisplayMetrics::Init()
{
	m_ControlLabelFont.SetName(m_DispControlLabelFont);
	m_GroupLabelFont.SetName(m_DispGroupLabelFont);
}

unsigned GOGUIDisplayMetrics::GetDrawstopWidth()
{
	return m_DrawStopWidth;
}

unsigned GOGUIDisplayMetrics::GetDrawstopHeight()
{
	return m_DrawStopHeight;
}

unsigned GOGUIDisplayMetrics::GetButtonWidth()
{
	return m_ButtonWidth;
}

unsigned GOGUIDisplayMetrics::GetButtonHeight()
{
	return m_ButtonHeight;
}

/*
 * BASIC EXPORTS STRAIGHT FROM ODF
 */

unsigned GOGUIDisplayMetrics::NumberOfExtraDrawstopRowsToDisplay()
{
	return m_DispExtraDrawstopRows;
}

unsigned GOGUIDisplayMetrics::NumberOfExtraDrawstopColsToDisplay()
{
	return m_DispExtraDrawstopCols;
}

unsigned GOGUIDisplayMetrics::NumberOfDrawstopColsToDisplay()
{
	return m_DispDrawstopCols;
}

unsigned GOGUIDisplayMetrics::NumberOfButtonCols()
{
	return m_DispButtonCols;
}

unsigned GOGUIDisplayMetrics::NumberOfExtraButtonRows()
{
	return m_DispExtraButtonRows;
}

unsigned GOGUIDisplayMetrics::GetDrawstopBackgroundImageNum()
{
	return m_DispDrawstopBackgroundImageNum;
}

unsigned GOGUIDisplayMetrics::GetConsoleBackgroundImageNum()
{
	return m_DispConsoleBackgroundImageNum;
}

unsigned GOGUIDisplayMetrics::GetDrawstopInsetBackgroundImageNum()
{
	return m_DispDrawstopInsetBackgroundImageNum;
}

unsigned GOGUIDisplayMetrics::GetKeyVertBackgroundImageNum()
{
	return m_DispKeyVertBackgroundImageNum;
}

unsigned GOGUIDisplayMetrics::GetKeyHorizBackgroundImageNum()
{
	return m_DispKeyHorizBackgroundImageNum;
}

bool GOGUIDisplayMetrics::HasPairDrawstopCols()
{
	return m_DispPairDrawstopCols;
}

bool GOGUIDisplayMetrics::HasTrimAboveExtraRows()
{
	return m_DispTrimAboveExtraRows;
}

bool GOGUIDisplayMetrics::HasExtraPedalButtonRow()
{
	return m_DispExtraPedalButtonRow;
}

GOrgueFont GOGUIDisplayMetrics::GetControlLabelFont()
{
	return m_ControlLabelFont;
}

GOrgueFont GOGUIDisplayMetrics::GetGroupLabelFont()
{
	return m_GroupLabelFont;
}

unsigned GOGUIDisplayMetrics::GetScreenWidth()
{
	return m_DispScreenSizeHoriz;
}

unsigned GOGUIDisplayMetrics::GetScreenHeight()
{
	return m_DispScreenSizeVert;
}

/*
 * COMPUTED VALUES FROM ODF PARAMETERS
 */

unsigned GOGUIDisplayMetrics::GetJambLeftRightHeight()
{
	return (m_DispDrawstopRows + 1) * m_DrawStopHeight;
}

int GOGUIDisplayMetrics::GetJambLeftRightY()
{
	return ((int)(m_DispScreenSizeVert - GetJambLeftRightHeight() - (m_DispDrawstopColsOffset ? (m_DrawStopHeight/2) : 0))) / 2;
}

int GOGUIDisplayMetrics::GetJambLeftRightWidth()
{
	int jamblrw = m_DispDrawstopCols * m_DrawStopWidth / 2;
	if (m_DispPairDrawstopCols)
		jamblrw += ((m_DispDrawstopCols >> 2) * (m_DrawStopWidth / 4)) - 8;
	return jamblrw;
}

unsigned GOGUIDisplayMetrics::GetJambTopHeight()
{
	return m_DispExtraDrawstopRows * m_DrawStopHeight;
}

unsigned GOGUIDisplayMetrics::GetJambTopWidth()
{
	return m_DispExtraDrawstopCols * m_DrawStopWidth;
}

int GOGUIDisplayMetrics::GetJambTopX()
{
	return (m_DispScreenSizeHoriz - GetJambTopWidth()) >> 1;
}

unsigned GOGUIDisplayMetrics::GetPistonTopHeight()
{
	return m_DispExtraButtonRows * m_ButtonHeight;
}

unsigned GOGUIDisplayMetrics::GetPistonWidth()
{
	return m_DispButtonCols * m_ButtonWidth;
}

int GOGUIDisplayMetrics::GetPistonX()
{
	return (m_DispScreenSizeHoriz - GetPistonWidth()) >> 1;
}

/*
 * COMPUTED VALUES FROM ODF PARAMETERS & SCREEN
 */

int GOGUIDisplayMetrics::GetCenterWidth()
{
	return m_CenterWidth;
}

int GOGUIDisplayMetrics::GetCenterX()
{
	return (m_DispScreenSizeHoriz - m_CenterWidth) >> 1;
}

int GOGUIDisplayMetrics::GetCenterY()
{
	return m_CenterY;
}

unsigned GOGUIDisplayMetrics::GetEnclosureWidth()
{
	return m_EnclosureWidth * m_Enclosures.size();
}

int GOGUIDisplayMetrics::GetEnclosureY()
{
	return m_EnclosureY;
}

int GOGUIDisplayMetrics::GetEnclosureX(const GOGUIEnclosure* enclosure)
{

	assert(enclosure);

	int enclosure_x = (GetScreenWidth() - GetEnclosureWidth() + 6) >> 1;
	for (unsigned int i = 0; i < m_Enclosures.size(); i++)
	{
		if (enclosure == m_Enclosures[i])
			return enclosure_x;
		enclosure_x += m_EnclosureWidth;
	}

	throw (wxString)_("enclosure not found");

}

int GOGUIDisplayMetrics::GetJambLeftX()
{
	int jamblx = (GetCenterX() - GetJambLeftRightWidth()) >> 1;
	if (m_DispPairDrawstopCols)
		jamblx += 5;
	return jamblx;
}

int GOGUIDisplayMetrics::GetJambRightX()
{
	int jambrx = GetJambLeftX() + GetCenterX() + m_CenterWidth;
	if (m_DispPairDrawstopCols)
		jambrx += 5;
	return jambrx;
}

int GOGUIDisplayMetrics::GetJambTopDrawstop()
{
	if (m_DispTrimAboveExtraRows)
		return m_CenterY + 8;
	return m_CenterY;
}

int GOGUIDisplayMetrics::GetJambTopPiston()
{
	if (m_DispTrimAboveExtraRows)
		return m_CenterY + 8;
	return m_CenterY;
}

int GOGUIDisplayMetrics::GetJambTopY()
{
	if (m_DispTrimAboveExtraRows)
		return m_CenterY + 8;
	return m_CenterY;
}

/*
 * BLIT POSITION FUNCTIONS
 */

void GOGUIDisplayMetrics::GetDrawstopBlitPosition(const int drawstopRow, const int drawstopCol, int& blitX, int& blitY)
{
	int i;
	if (drawstopRow > 99)
	{
		blitX = GetJambTopX() + (drawstopCol - 1) * m_DrawStopWidth + 6;
		if (m_DispExtraDrawstopRowsAboveExtraButtonRows)
		{
			blitY = GetJambTopDrawstop() + (drawstopRow - 100) * m_DrawStopHeight + 2;
		}
		else
		{
			blitY = GetJambTopDrawstop() + (drawstopRow - 100) * m_DrawStopHeight + (m_DispExtraButtonRows * m_ButtonHeight) + 2;
		}
	}
	else
	{
		i = m_DispDrawstopCols >> 1;
		if (drawstopCol <= i)
		{
			blitX = GetJambLeftX() + (drawstopCol - 1) * m_DrawStopWidth + 6;
			blitY = GetJambLeftRightY() + (drawstopRow - 1) * m_DrawStopHeight + 32;
		}
		else
		{
			blitX = GetJambRightX() + (drawstopCol - 1 - i) * m_DrawStopWidth + 6;
			blitY = GetJambLeftRightY() + (drawstopRow - 1) * m_DrawStopHeight + 32;
		}
		if (m_DispPairDrawstopCols)
			blitX += (((drawstopCol - 1) % i) >> 1) * (m_DrawStopWidth / 4);

		if (drawstopCol <= i)
			i = drawstopCol;
		else
			i = m_DispDrawstopCols - drawstopCol + 1;
		if (m_DispDrawstopColsOffset && (i & 1) ^ m_DispDrawstopOuterColOffsetUp)
			blitY += m_DrawStopHeight / 2;

	}
}

void GOGUIDisplayMetrics::GetPushbuttonBlitPosition(const int buttonRow, const int buttonCol, int& blitX, int& blitY)
{

	blitX = GetPistonX() + (buttonCol - 1) * m_ButtonWidth + 6;
	if (buttonRow > 99)
	{
		if (m_DispExtraDrawstopRowsAboveExtraButtonRows)
		{
			blitY = GetJambTopPiston() + (buttonRow - 100) * m_ButtonHeight + (m_DispExtraDrawstopRows * m_DrawStopHeight) + 5;
		}
		else
		{
			blitY = GetJambTopPiston() + (buttonRow - 100) * m_ButtonHeight + 5;
		}
	}
	else
	{
		int i = buttonRow;
		if (i == 99)
			i = 0;

		if (i >= (int)m_Manuals.size())
			blitY = m_HackY - (i + 1 - (int)m_Manuals.size()) * (m_ManualHeight + m_ButtonHeight) + m_ManualHeight + 5;
		else
			blitY = m_ManualRenderInfo[i].piston_y + 5;

		if (m_DispExtraPedalButtonRow && !buttonRow)
			blitY += m_ButtonHeight;
		if (m_DispExtraPedalButtonRowOffset && buttonRow == 99)
			blitX -= m_ButtonWidth / 2 + 2;
	}

}

/*
 * UPDATE METHOD - updates parameters dependent on ODF parameters and
 * screen stuff.
 */

/* TODO: this method could do with a cleanup */

void GOGUIDisplayMetrics::Update()
{
	if (!m_Manuals.size())
		m_Manuals.push_back(NULL);

	m_ManualRenderInfo.resize(m_Manuals.size());
	m_CenterY = m_DispScreenSizeVert - m_PedalHeight;
	m_CenterWidth = std::max(GetJambTopWidth(), GetPistonWidth());

	for (unsigned i = 0; i < m_Manuals.size(); i++)
	{

		if (!i && m_Manuals[i])
		{
			m_ManualRenderInfo[0].height = m_PedalHeight;
			m_ManualRenderInfo[0].keys_y = m_ManualRenderInfo[0].y = m_CenterY;
			m_CenterY -= m_PedalHeight;
			if (m_DispExtraPedalButtonRow)
				m_CenterY -= m_ButtonHeight;
			m_ManualRenderInfo[0].piston_y = m_CenterY;
			m_CenterWidth = std::max(m_CenterWidth, (int)GetEnclosureWidth());
			m_CenterY -= 12;
			m_CenterY -= m_EnclosureHeight;
			m_EnclosureY = m_CenterY;
			m_CenterY -= 12;
		}
		if (!i && !m_Manuals[i] && m_Enclosures.size())
		{
			m_CenterY -= 12;
			m_CenterY -= m_EnclosureHeight;
			m_EnclosureY = m_CenterY;
			m_CenterY -= 12;
		}

		if (!m_Manuals[i])
			continue;

		if (i)
		{
			if (!m_DispButtonsAboveManuals)
			{
				m_CenterY -= m_ButtonHeight;
				m_ManualRenderInfo[i].piston_y = m_CenterY;
			}
			m_ManualRenderInfo[i].height = m_ManualHeight;
			if (m_DispTrimBelowManuals && i == 1)
			{
				m_ManualRenderInfo[i].height += 8;
				m_CenterY -= 8;
			}
			m_CenterY -= m_ManualHeight;
			m_ManualRenderInfo[i].keys_y = m_CenterY;
			if (m_DispTrimAboveManuals && i + 1 == m_Manuals.size())
			{
				m_CenterY -= 8;
				m_ManualRenderInfo[i].height += 8;
			}
			if (m_DispButtonsAboveManuals)
			{
				m_CenterY -= m_ButtonHeight;
				m_ManualRenderInfo[i].piston_y = m_CenterY;
			}
			m_ManualRenderInfo[i].y = m_CenterY;
		}
		m_ManualRenderInfo[i].width = 1;
		if (i)
		{
			for (unsigned j = 0; j < m_Manuals[i]->GetKeyCount(); j++)
			{
				if (!m_Manuals[i]->IsSharp(j))
					m_ManualRenderInfo[i].width += m_ManualKeyWidth;
			}
		}
		else
		{
			for (unsigned j = 0; j < m_Manuals[i]->GetKeyCount(); j++)
			{
				m_ManualRenderInfo[i].width += m_PedalKeyWidth;
				if (j && !m_Manuals[i]->IsSharp(j - 1) && !m_Manuals[i]->IsSharp(j))
					m_ManualRenderInfo[i].width += m_PedalKeyWidth;
			}
		}
		m_ManualRenderInfo[i].x = (m_DispScreenSizeHoriz - m_ManualRenderInfo[i].width) >> 1;
		m_ManualRenderInfo[i].width += 16;
		if ((int)m_ManualRenderInfo[i].width > m_CenterWidth)
			m_CenterWidth = m_ManualRenderInfo[i].width;
	}

	m_HackY = m_CenterY;

	if (m_CenterWidth + GetJambLeftRightWidth() * 2 < (int)m_DispScreenSizeHoriz)
		m_CenterWidth += (m_DispScreenSizeHoriz - m_CenterWidth - GetJambLeftRightWidth() * 2) / 3;

	m_CenterY -= GetPistonTopHeight();
	m_CenterY -= GetJambTopHeight();
	if (m_DispTrimAboveExtraRows)
		m_CenterY -= 8;
}

const GOGUIDisplayMetrics::MANUAL_RENDER_INFO& GOGUIDisplayMetrics::GetManualRenderInfo(const unsigned manual_nb) const
{
	assert(manual_nb < m_ManualRenderInfo.size());
	return m_ManualRenderInfo[manual_nb];

}

void GOGUIDisplayMetrics::RegisterEnclosure(GOGUIEnclosure* enclosure)
{
	m_Enclosures.push_back(enclosure);
}

void GOGUIDisplayMetrics::RegisterManual(GOGUIManual* manual)
{
	m_Manuals.push_back(manual);
}
