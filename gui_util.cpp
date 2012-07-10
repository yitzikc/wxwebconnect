
#include <wx/wx.h>
#include "webframe.h"
#include "webcontrol.h"
#include "gui_util.h"

using namespace wxwc_util;

void wxwc_util::populate_menu(wxMenu& menu, const menu_item_info* items, int item_count)
{
	for (int i = 0; i < item_count; i++)
	{
		if (wxID_SEPARATOR != items[i].gui_id)
		{
			wxString label;
			if (NULL != items[i].label)
				label = wxGetTranslation(items[i].label);

			wxMenuItem *mi = new wxMenuItem(&menu, items[i].gui_id, label);
			mi -> SetBitmap(wxArtProvider::GetBitmap(items[i].art_id, wxART_MENU));
			menu.Append(mi);
		}
		else
		{
			menu.AppendSeparator();
		}
	}
}

wxWebFrame* wxwc_util::open_uri_in_frame(const wxString& uri)
{
	wxWebFrame* frm = new wxWebFrame(
		NULL, wxID_NONE, uri, wxDefaultPosition, wxSize(800, 600));

	frm -> SetShouldPreventAppExit(false);
	frm -> GetWebControl() -> OpenURI(uri);
	frm -> Show();
	return frm;
}
