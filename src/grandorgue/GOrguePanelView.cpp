/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOrguePanelView.h"

#include "GOGUIPanel.h"
#include "GOGUIPanelWidget.h"
#include "Images.h"
#include <wx/display.h>
#include <wx/frame.h>
#include <wx/icon.h>
#include <wx/image.h>

BEGIN_EVENT_TABLE(GOrguePanelView, wxScrolledWindow)
	EVT_SIZE(GOrguePanelView::OnSize)
END_EVENT_TABLE()

GOrguePanelView* GOrguePanelView::createWindow(GOrgueDocumentBase* doc, GOGUIPanel* panel, wxWindow* parent)
{
	wxFrame* frame = new wxFrame(NULL, -1, panel->GetName(), wxDefaultPosition, wxDefaultSize, 
				     wxMINIMIZE_BOX | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | 
				     wxCLIP_CHILDREN | wxFULL_REPAINT_ON_RESIZE | wxMAXIMIZE_BOX);
				    // By adding wxMAXIMIZE_BOX, maximize (on Windows, Linux) will work, but only
				    // fill the screen if it is smaller than the maximum allowed frame size.
				    // We probably should change this to add black borders to it. Also, for these
				    // platforms a full-screen mode would be beneficial (on MacOS is already works).
	wxIcon icon;
	icon.CopyFromBitmap(GetImage_GOIcon());
	frame->SetIcon(icon);
	parent = frame;
	return new GOrguePanelView(doc, panel, parent);
}


GOrguePanelView::GOrguePanelView(GOrgueDocumentBase* doc, GOGUIPanel* panel, wxWindow* parent) :
	wxScrolledWindow(parent),
	GOrgueView(doc, parent),
	m_panelwidget(NULL),
	m_panel(panel)
{
	wxWindow* frame = parent;
	
	GOGUIPanelWidget* panelwidget = new GOGUIPanelWidget(panel, this);

	// Set maximum size to the size the window would have with scaling 100% 
	// However, still within the default maximum limits, whatever those may be
	// Set an initial client size to what it would be with scaling 100%
	frame->SetMaxSize(wxSize(wxDefaultCoord, wxDefaultCoord)); 
	frame->SetClientSize(panelwidget->GetSize());
	//frame->SetMaxSize(frame->GetSize());

	// Set a minimum size for the window, just some small value
	wxSize minsize(100,100);
	frame->SetMinClientSize(minsize);	

	// Set a black background color (especially useful if user switches to full screen)
	frame->SetBackgroundColour(*wxBLACK);

	// Get the position and size of the window as saved by the user previously,
	// or otherwise its default values
	wxRect size = panel->GetWindowSize();

	// If both width and height are set, set position and size of the window
	// E.g. in case of corrupted preferences, this might not be the case 
	if (size.GetWidth() && size.GetHeight())
		frame->SetSize(size);
	// However, even if this worked, we cannot be sure that the window is now 
	// fully within the client area of an existing display.
	// For example, the user may have disconnected the display, or lowered its resolution.
	// So, we need to get the client area of the desired display, or
	// if it does not exist anymore, the client area of the default display
	int nr = wxDisplay::GetFromWindow(frame);
	wxDisplay display(nr != wxNOT_FOUND ? nr : 0);
	wxRect max = display.GetClientArea();
	// If our current window is within this area, all is fine
	wxRect current = frame->GetRect();
	if (!max.Contains(current)) {
		// Otherwise, check and correct width and height,
		// and place the frame at the center of the Client Area of the display
		if (current.GetWidth() > max.GetWidth())
			current.SetWidth(max.GetWidth());
		if (current.GetHeight() > max.GetHeight())
			current.SetHeight(max.GetHeight());
		frame->SetSize(current.CenterIn(max, wxBOTH));
	}
	
	m_panelwidget = panelwidget;
	
	// At this point, the new window may fill the whole display and still not be large
	// enough to show all contents. So, before showing anything, lets see what scaling
	// is needed to fit the content completely to the window.
	wxSize scaledsize = m_panelwidget->UpdateSize(frame->GetClientSize());
	
	// If we resized the window with respect to a default or user setting,
	// it should now be resized again, to eliminate (by default) any whitespace.
	// On the other hand, if the user wanted to have whitespace, we must keep
	// the size set by the user.
	if (size != frame->GetRect()) {
		// However, there is a chance that the scaled size is actually larger
		// than the actual size (in case the display area is so small that with
		// the minimum scaling we still need scrollbars). In that case we should
		// not do any resizing. So, resize only if this eliminates whitespace.
		if ((scaledsize.GetWidth()<=frame->GetClientSize().GetWidth()) && (scaledsize.GetHeight()<=frame->GetClientSize().GetHeight()))
			frame->SetClientSize(scaledsize);
	}

	frame->Show();
	frame->Update();
	
	// Ensure that scrollbars will appear when they are needed
	// Current design aims for avoiding this as much as possible
	this->SetScrollRate(5, 5);
	this->SetVirtualSize(scaledsize);

	// Set position of panelwidget to initial value
	m_panelwidget->SetPosition(wxPoint(0,0));
	// In a direction where the actual window size is larger than the 
	// size of the panel (=scaledsize), the panel needs to be centered
	wxSize actualsize = GetParent()->GetClientSize();
	if (actualsize.GetWidth() > scaledsize.GetWidth())
		m_panelwidget->CentreOnParent(wxHORIZONTAL);
	if (actualsize.GetHeight() > scaledsize.GetHeight())
		m_panelwidget->CentreOnParent(wxVERTICAL);

	m_panel->SetView(this);

}

GOrguePanelView::~GOrguePanelView()
{
	if (m_panel)
		m_panel->SetView(NULL);
}

void GOrguePanelView::RemoveView()
{
	if (m_panel)
		m_panel->SetView(NULL);
	m_panel = NULL;
	GOrgueView::RemoveView();
}

void GOrguePanelView::AddEvent(GOGUIControl* control)
{
	wxCommandEvent event(wxEVT_GOCONTROL, 0);
	event.SetClientData(control);
	m_panelwidget->GetEventHandler()->AddPendingEvent(event);
}

void GOrguePanelView::SyncState()
{
	m_panel->SetWindowSize(GetParent()->GetRect());
	m_panel->SetInitialOpenWindow(true);
}

bool GOrguePanelView::Destroy()
{
	if (m_panel)
		m_panel->SetView(NULL);
	return wxScrolledWindow::Destroy();
}

void GOrguePanelView::Raise()
{
	wxScrolledWindow::Raise();
	m_panelwidget->CallAfter(&GOGUIPanelWidget::Focus);
}

void GOrguePanelView::OnSize(wxSizeEvent& event)
{
	if (m_panelwidget)
	{
		wxSize max = event.GetSize();
		max = m_panelwidget->UpdateSize(max);
		wxSize scaledsize = max;
		this->SetVirtualSize(max);

		int x, xu, y, yu;
		GetScrollPixelsPerUnit(&xu, &yu);
		
		GetViewStart(&x, &y);
		
		if (xu && x * xu + max.GetWidth() > event.GetSize().GetWidth())
			x = (event.GetSize().GetWidth() - max.GetWidth()) / xu;
		if (yu && y * yu + max.GetHeight() > event.GetSize().GetHeight())
			y = (event.GetSize().GetHeight() - max.GetHeight()) / yu;
		this->Scroll(x, y);

		// Reset the position of the panel, before centering it
		// (some wrong value from previous centering may remain otherwise)
		m_panelwidget->SetPosition(wxPoint(0,0));
		// In a direction where the actual window size is larger than the 
		// size of the panel (=scaledsize), the panel needs to be centered
		wxSize actualsize = GetParent()->GetClientSize();
		if (actualsize.GetWidth() > scaledsize.GetWidth())
			m_panelwidget->CentreOnParent(wxHORIZONTAL);
		if (actualsize.GetHeight() > scaledsize.GetHeight())
			m_panelwidget->CentreOnParent(wxVERTICAL);

	}
	event.Skip();
}
