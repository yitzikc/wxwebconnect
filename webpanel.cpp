
#include <wx/wx.h>
#include "webcontrol.h"
#include "webpanel.h"
#include "gui_util.h"

void wxWebPanelBase::DoInit()
{
	m_browser  = NULL;
	m_topSizer = NULL;
	m_set_statusbar_enabled = false;
	m_set_titlebar_enabled  = false;
	m_dom_contentloaded = false;
}

void wxWebPanelBase::DestroyGui()
{
	if (NULL != m_browser)
		delete m_browser;

	if (NULL != m_topSizer)
		delete m_topSizer;
}

wxWebPanelBase::wxWebPanelBase() : wxPanel()
{
	DoInit();
}

wxWebPanelBase::wxWebPanelBase(
			wxWindow *parent,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name)  :
			wxPanel(parent, id, pos, size, style, name) 
{
	DoInit();
}

wxWebPanelBase::~wxWebPanelBase()
{
	
}

void wxWebPanelBase::ArrangeGui()
{
	m_topSizer = new wxGridSizer( 1 );
	m_browser -> Create(this, wxID_WEB);
	m_topSizer -> Add(m_browser, 0, wxEXPAND);
	SetSizer(m_topSizer);
	m_topSizer -> Fit(this);
}

void wxWebPanelBase::SetContainingFrameAccess(long access)
{
	m_set_statusbar_enabled = (0 != (access & ACCESS_STATUSBAR));
	m_set_titlebar_enabled  = (0 != (access & ACCESS_TITLEBAR));
}

BEGIN_EVENT_TABLE(wxWebPanelBase, wxPanel)

	// web events
    EVT_WEB_STATUSTEXT(wxID_WEB, wxWebPanelBase::OnStatusText)
    EVT_WEB_STATUSCHANGE(wxID_WEB, wxWebPanelBase::OnStatusChange)
    EVT_WEB_STATECHANGE(wxID_WEB, wxWebPanelBase::OnStateChange)
    EVT_WEB_LOCATIONCHANGE(wxID_WEB, wxWebPanelBase::OnLocationChange)
    EVT_WEB_TITLECHANGE(wxID_WEB, wxWebPanelBase::OnTitleChange)
    EVT_WEB_SHOWCONTEXTMENU(wxID_WEB, wxWebPanelBase::OnShowContextMenu)
    EVT_WEB_INITDOWNLOAD(wxID_WEB, wxWebPanelBase::OnInitDownload)
    EVT_WEB_SHOULDHANDLECONTENT(wxID_WEB, wxWebPanelBase::OnShouldHandleContent)
    EVT_WEB_DOMCONTENTLOADED(wxID_WEB, wxWebPanelBase::OnDOMContentLoaded)

	// Context menu events:
	EVT_MENU(ID_OpenHref, wxWebPanelBase::OnOpenHref)
	EVT_MENU(ID_OpenHrefInDefault, wxWebPanelBase::OnOpenHrefInDefaultBrowser)
	EVT_MENU(wxID_BACKWARD, wxWebPanelBase::OnGoBack)
    EVT_MENU(wxID_FORWARD, wxWebPanelBase::OnGoForward)
    EVT_MENU(wxID_STOP, wxWebPanelBase::OnStop)
    EVT_MENU(wxID_REFRESH, wxWebPanelBase::OnReload)
	EVT_MENU(wxID_CUT, wxWebPanelBase::OnCut)
    EVT_MENU(wxID_COPY, wxWebPanelBase::OnCopy)
    EVT_MENU(ID_CopyLink, wxWebPanelBase::OnCopyLink)
    EVT_MENU(wxID_PASTE, wxWebPanelBase::OnPaste)
    EVT_MENU(wxID_SELECTALL, wxWebPanelBase::OnSelectAll)

	 // update events
    EVT_UPDATE_UI(wxID_UNDO, wxWebPanelBase::OnUpdateUI)
    EVT_UPDATE_UI(wxID_REDO, wxWebPanelBase::OnUpdateUI)
    EVT_UPDATE_UI(wxID_CUT, wxWebPanelBase::OnUpdateUI)
    EVT_UPDATE_UI(wxID_COPY, wxWebPanelBase::OnUpdateUI)
    EVT_UPDATE_UI(ID_CopyLink, wxWebPanelBase::OnUpdateUI)
	
END_EVENT_TABLE()


void wxWebPanelBase::OnStatusText(wxWebEvent& evt)
{
    // wxEVT_WEB_STATUSTEXT is received when somebody hovers
    // the mouse over a link and the status text should
    // be updated
    if (SetStatusBarIsEnabled())
	{
		wxString status_text = evt.GetString();
		if (status_text.Length() == 0)
			status_text = _("Ready");

		wxStatusBar* status_bar = m_containing_frame -> GetStatusBar();
		if (NULL != status_bar)
			status_bar->SetStatusText(status_text);
	}
}

void wxWebPanelBase::OnStatusChange(wxWebEvent& evt)
{
    // wxEVT_WEB_STATUSCHANGE is received when the status text
    // changes when a web page is loading
    if (SetStatusBarIsEnabled())
	{
		wxStatusBar* status_bar = m_containing_frame -> GetStatusBar();
		if (NULL != status_bar)
			status_bar->SetStatusText(evt.GetString());
	}
    // note: the status bar text is reset when
    // all the content is finished loading, in
    // OnDOMContentLoaded()
}

void wxWebPanelBase::OnStateChange(wxWebEvent& evt)
{
    // clear the status bar hear since OnStatusChange() doesn't 
    // contain an empty string and we don't want "stuck" text in 
    // the statusbar
    int state = evt.GetState();  
	if (SetStatusBarIsEnabled())
	{
		wxStatusBar* status_bar = m_containing_frame -> GetStatusBar();
		if (NULL != status_bar)
		{
			wxString status_text = _("Ready");
   

			if ((state & wxWEB_STATE_STOP) && (state & wxWEB_STATE_IS_REQUEST))
				status_bar->SetStatusText(status_text);

			if ((state & wxWEB_STATE_STOP) && (state & wxWEB_STATE_IS_REQUEST))
				status_bar->SetStatusText(status_text);
		}
	}
}

void wxWebPanelBase::OnLocationChange(wxWebEvent& evt)
{
    // set the url bar
#ifdef HAVE_URL_BAR
    SetUrlBarValue(evt.GetString());
#endif

    // set the DOM content loaded flag to false
    // until the DOM is safely loaded
    m_dom_contentloaded = false;
}

void wxWebPanelBase::OnTitleChange(wxWebEvent& evt)
{
	if (SetTitleBarIsEnabled())
		m_containing_frame -> SetTitle(evt.GetString());
}

void wxWebPanelBase::PopulatePopupMenu(wxMenu& menuPopup, const wxWebEvent& evt)
{
	using namespace wxwc_util;

	wxString href = evt.GetHref();
    if (!href.IsEmpty())
    {
		static const menu_item_info link_items[] = {
			{ ID_OpenHref, wxTRANSLATE("&Open"), wxART_FOLDER_OPEN },
			{ ID_CopyLink, wxTRANSLATE("Copy &Link"), wxART_COPY },
			{ ID_OpenHrefInDefault, wxTRANSLATE("Open Link in &Default browser") }
		};
        populate_menu(menuPopup, link_items, sizeof(link_items) / sizeof(link_items[0]));
        m_uri_href = href;
    }
    else
    {
        static const menu_item_info navigation_items[] = {
			{ wxID_BACKWARD, NULL, wxART_GO_BACK },
			{ wxID_FORWARD, NULL, wxART_GO_FORWARD },
			{ wxID_REFRESH, wxTRANSLATE("&Reload"), wxART_REDO },
			{ wxID_STOP, wxTRANSLATE("&Stop loading"), wxART_CROSS_MARK },
		};
		populate_menu(menuPopup, navigation_items, sizeof(navigation_items) / sizeof(navigation_items[0]));
        
    }

	static const menu_item_info generic_items[] = {
		{ wxID_SEPARATOR, NULL, NULL },
		{ wxID_CUT, NULL, wxART_CUT },
		{ wxID_COPY, NULL, wxART_COPY },
		
		{ wxID_PASTE, NULL, wxART_PASTE },
		{ wxID_SELECTALL, wxTRANSLATE("Select &All"), NULL },
		{ wxID_SEPARATOR, NULL, NULL },
		{ wxID_ZOOM_IN, wxTRANSLATE("Larger &Text"), wxART_LIST_VIEW },
		{ wxID_ZOOM_OUT, wxTRANSLATE("&Smaller Text"), wxART_REPORT_VIEW },
		{ wxID_ZOOM_100, wxTRANSLATE("&Normal Text"), NULL },
	};

	populate_menu(menuPopup, generic_items, sizeof(generic_items) / sizeof(generic_items[0]));
}

void wxWebPanelBase::OnShowContextMenu(wxWebEvent& evt)
{

    wxMenu menuPopup;
	PopulatePopupMenu(menuPopup, evt);
   
    wxPoint pt_mouse = ::wxGetMousePosition();
    pt_mouse = m_browser->ScreenToClient(pt_mouse);
    PopupMenu(&menuPopup, pt_mouse);
}

void wxWebPanelBase::OnOpenHref(wxCommandEvent& evt)
{
    m_browser->OpenURI(m_uri_href);
}

void wxWebPanelBase::OnOpenHrefInDefaultBrowser(wxCommandEvent& evt)
{
	wxLaunchDefaultBrowser(m_uri_href);
}

void wxWebPanelBase::OnGoBack(wxCommandEvent& evt)
{
    m_browser->GoBack();
}

void wxWebPanelBase::OnGoForward(wxCommandEvent& evt)
{
    m_browser->GoForward();
}

void wxWebPanelBase::OnStop(wxCommandEvent& evt)
{
    m_browser->Stop();
}

void wxWebPanelBase::OnReload(wxCommandEvent& evt)
{
    m_browser->Reload();
}

void wxWebPanelBase::OnCut(wxCommandEvent& evt)
{
    m_browser->CutSelection();
}

void wxWebPanelBase::OnCopy(wxCommandEvent& evt)
{
    m_browser->CopySelection();
}

void wxWebPanelBase::OnCopyLink(wxCommandEvent& evt)
{
    m_browser->CopyLinkLocation();
}

void wxWebPanelBase::OnPaste(wxCommandEvent& evt)
{
    m_browser->Paste();
}

void wxWebPanelBase::OnSelectAll(wxCommandEvent& evt)
{
    m_browser->SelectAll();
}

void wxWebPanelBase::OnZoomIn(wxCommandEvent& evt)
{
    float zoom;
    m_browser->GetTextZoom(&zoom);
    m_browser->SetTextZoom(zoom + 0.10);
}

void wxWebPanelBase::OnZoomOut(wxCommandEvent& evt)
{
    float zoom;
    m_browser->GetTextZoom(&zoom);
    m_browser->SetTextZoom(zoom - 0.10);
}

void wxWebPanelBase::OnZoomReset(wxCommandEvent& evt)
{
    m_browser->SetTextZoom(1);
}

void wxWebPanelBase::OnInitDownload(wxWebEvent& evt)
{
    // TODO: add download status indicator and option
    // to open content in the browser

    // note: this handler gets called when content is
    // available to download; the content can be handled
    // as follows:
    //   evt.SetDownloadAction(wxWEB_DOWNLOAD_OPEN);   // open content
    //   evt.SetDownloadAction(wxWEB_DOWNLOAD_SAVEAS); // save content
    //   evt.SetDownloadAction(wxWEB_DOWNLOAD_CANCEL); // cancel the operation

    // here, we'll allow the user to download it or cancel
    // the operation

    // get the filename
    wxString filename = evt.GetFilename();
    wxMessageDialog dlg(this,
                        wxString::Format(wxT("Would you like to download %s?"), (const wxChar*)filename.c_str()),
                        wxT("Download File"),
                        wxYES_NO);

    int result = dlg.ShowModal();

    switch (result)
    {
        case wxID_YES:
            {
                wxString filter;
                filter += _("All Files");
                filter += wxT(" (*.*)|*.*");

                wxFileDialog dlg(this,
                         _("Save As"),
                         wxT(""),
                         filename,
                         filter,
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

                if (dlg.ShowModal() != wxID_OK)
                {
                    evt.SetDownloadAction(wxWEB_DOWNLOAD_CANCEL);
                    return;
                }

                wxWebProgressBase* listener = new wxWebProgressBase;
                evt.SetDownloadAction(wxWEB_DOWNLOAD_SAVEAS);
                evt.SetDownloadTarget(dlg.GetPath());
                evt.SetDownloadListener(listener);
            }
            break;

        case wxID_NO:
            evt.SetDownloadAction(wxWEB_DOWNLOAD_CANCEL);
            break;
    }
}

void wxWebPanelBase::OnShouldHandleContent(wxWebEvent& evt)
{
    // note: this handler gets called when a content
    // type needs to be handled and allows us to determine
    // whether or not we want to handle the content type
    // or let the browser handle it

    // for example, by default, we'll let the browser handle
    // all content types, except XML

    // get the content type
    wxString input_content_type = evt.GetContentType();
    if (input_content_type == wxT("application/xml"))
    {
        // we have an XML content; don't let the browser
        // handle this, since we'll do it ourselves
        evt.SetShouldHandle(false);
        
        // TODO: custom XML handling
        return;
    }

    evt.Skip();
}

void wxWebPanelBase::OnDOMContentLoaded(wxWebEvent& evt)
{
    m_dom_contentloaded = true;
}

void wxWebPanelBase::OnUpdateUI(wxUpdateUIEvent& evt)
{
    int id = evt.GetId();
    switch (id)
    {
        case wxID_UNDO:
            evt.Enable(false);  // TODO: undo needs to be implemented
            break;
            
        case wxID_REDO:
            evt.Enable(false);  // TODO: undo needs to be implemented
            break;

        case wxID_CUT:
            evt.Enable(m_browser->CanCutSelection());
            break;
            
        case wxID_COPY:
            evt.Enable(m_browser->CanCopySelection());
            break;

        case ID_CopyLink:
            evt.Enable(m_browser->CanCopyLinkLocation());
            break;
            
        case wxID_PASTE:
            evt.Enable(m_browser->CanPaste());
            break;
/* Text searching is not yet implemented.
        case ID_FindAgain:
            evt.Enable(m_find_text.Length() > 0 ? true : false);
            break;
			*/
    }
}
