/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "OrganSelectDialog.h"

#include "GOrgueArchiveFile.h"
#include "GOrgueOrgan.h"
#include "GOrgueOrganList.h"
#include <wx/msgdlg.h>
#include <wx/sizer.h>

BEGIN_EVENT_TABLE(OrganSelectDialog, wxDialog)
	EVT_BUTTON(wxID_OK, OrganSelectDialog::OnOK)
	EVT_LIST_ITEM_ACTIVATED(ID_ORGANS, OrganSelectDialog::OnDoubleClick)
END_EVENT_TABLE()

OrganSelectDialog::OrganSelectDialog(wxWindow* parent, wxString title, const GOrgueOrganList& organList) :
	wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 480), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxDIALOG_NO_PARENT),
	m_OrganList(organList)
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->AddSpacer(5);

	m_Organs = new wxListView(this, ID_ORGANS, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);
	m_Organs->InsertColumn(0, _("Church"));
	m_Organs->InsertColumn(1, _("Builder"));
	m_Organs->InsertColumn(2, _("Recording"));
	m_Organs->InsertColumn(3, _("Organ package"));
	m_Organs->InsertColumn(4, _("ODF Path"));
	topSizer->Add(m_Organs, 1, wxEXPAND | wxALL, 5);

	for(unsigned i = 0, j = 0; j < m_OrganList.GetOrganList().size(); j++)
	{
		const GOrgueOrgan* o = m_OrganList.GetOrganList()[j];
		if (!o->IsUsable(m_OrganList))
			continue;
		m_Organs->InsertItem(i, o->GetChurchName());
		m_Organs->SetItemPtrData(i, (wxUIntPtr)o);
		m_Organs->SetItem(i, 1, o->GetOrganBuilder());
		m_Organs->SetItem(i, 2, o->GetRecordingDetail());
		m_Organs->SetItem(i, 4, o->GetODFPath());
		if (o->GetArchiveID() != wxEmptyString)
		{
			const GOrgueArchiveFile* a = m_OrganList.GetArchiveByID(o->GetArchiveID());
			m_Organs->SetItem(i, 3, a ? a->GetName() : o->GetArchiveID());
		}
		i++;
	}

	m_Organs->SetColumnWidth(0, 150);
	m_Organs->SetColumnWidth(1, 150);
	m_Organs->SetColumnWidth(2, 250);
	m_Organs->SetColumnWidth(3, wxLIST_AUTOSIZE);
	m_Organs->SetColumnWidth(4, wxLIST_AUTOSIZE);


	topSizer->AddSpacer(5);
	topSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_RIGHT | wxALL, 5);

	SetSizer(topSizer);
	Layout();
}

void OrganSelectDialog::OnOK(wxCommandEvent& event)
{
	if (m_Organs->GetItemData(m_Organs->GetFirstSelected()) == 0)
	{
		wxMessageBox(_("Please select an organ"), _("Error"), wxOK | wxICON_ERROR, this);
		return;
	}
	EndModal(wxID_OK);
}

const GOrgueOrgan* OrganSelectDialog::GetSelection()
{
	return (const GOrgueOrgan*)m_Organs->GetItemData(m_Organs->GetFirstSelected());
}

void OrganSelectDialog::OnDoubleClick(wxListEvent& event)
{
	wxCommandEvent e;
	OnOK(event);
}
