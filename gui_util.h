#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

#include "../webconnect/webframe.h"

namespace wxwc_util {

	struct menu_item_info {
		unsigned gui_id;		// Message routing ID of the menu item, or wxID_SEPARATOR to create a separator.
		const wxChar *label;	// Label text or NULL if a default value should be used.
		const wxChar *art_id;	// Art ID for use with wxArtProvider::GetBitmap, or NULL if not icon is desired.
	};

	void populate_menu(wxMenu& menu, const menu_item_info* items, int item_count);

	wxWebFrame* open_uri_in_frame(const wxString& uri);
	
};

#endif // ifdef _GUI_UTIL_H_
