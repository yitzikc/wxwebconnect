#ifndef _WEBPANEL_H_
#define _WEBPANEL_H_

#include <wx/wx.h>
#include "webcontrol.h"

class wxWebPanelBase : public wxPanel
{
	

	wxGridSizer*   m_topSizer;
	wxFrame *	  m_containing_frame;

	bool m_set_statusbar_enabled;
	bool m_set_titlebar_enabled;

	 // dom content
    bool m_dom_contentloaded;

	// href location
    wxString m_uri_href;

	void DoInit();

protected:
	// IDs for controls
	enum { wxID_WEB = 9002 };

	// IDs for menu items.
	enum
    {
        ID_CopyLink = wxID_HIGHEST+1,
		ID_OpenHref,
		ID_OpenHrefInDefault
	};

	wxWebControl* m_browser;

	void DestroyGui();
	void ArrangeGui();

	// Populate a menu supplied by the user in response to a wxWebEvent
	virtual void PopulatePopupMenu(wxMenu& menuPopup, const wxWebEvent& evt);
public:

	enum ContainingFrameAccessFlags {
		ACCESS_TITLEBAR = 1 << 0,
		ACCESS_STATUSBAR = 1 << 1,
		ACCESS_ALL = ACCESS_TITLEBAR | ACCESS_STATUSBAR };

	wxWebPanelBase();
	wxWebPanelBase(	wxWindow *parent,
				wxWindowID id=wxID_ANY,
				const wxPoint &pos=wxDefaultPosition,
				const wxSize &size=wxDefaultSize,
				long  style=wxTAB_TRAVERSAL,
				const wxString &name=wxPanelNameStr);

	~wxWebPanelBase();
	
	void SetContainingFrame(wxFrame *frame) { m_containing_frame = frame; }
	void SetContainingFrameAccess(long access);
	bool SetStatusBarIsEnabled() const  { return (NULL != m_containing_frame) && m_set_statusbar_enabled; }
	bool SetTitleBarIsEnabled()  const  { return (NULL != m_containing_frame) && m_set_titlebar_enabled; }

protected:
	
    virtual void OnStatusText(wxWebEvent& evt);
    virtual void OnStatusChange(wxWebEvent& evt);
    virtual void OnStateChange(wxWebEvent& evt);
    virtual void OnLocationChange(wxWebEvent& evt);
    virtual void OnTitleChange(wxWebEvent& evt);
    virtual void OnShowContextMenu(wxWebEvent& evt);
    virtual void OnInitDownload(wxWebEvent& evt);
    virtual void OnShouldHandleContent(wxWebEvent& evt);
    virtual void OnDOMContentLoaded(wxWebEvent& evt);
	virtual void OnUpdateUI(wxUpdateUIEvent& evt);

	// Context menu actions, also available to the class' users.

public:
	void OnOpenHref(wxCommandEvent& evt);
	void OnOpenHrefInDefaultBrowser(wxCommandEvent& evt);
	void OnGoBack(wxCommandEvent& evt);
    void OnGoForward(wxCommandEvent& evt);
    void OnStop(wxCommandEvent& evt);
    void OnReload(wxCommandEvent& evt);
	void OnCut(wxCommandEvent& evt);
	void OnCopy(wxCommandEvent& evt);
    void OnCopyLink(wxCommandEvent& evt);
    void OnPaste(wxCommandEvent& evt);
    void OnSelectAll(wxCommandEvent& evt);
	void OnZoomIn(wxCommandEvent& evt);
    void OnZoomOut(wxCommandEvent& evt);
    void OnZoomReset(wxCommandEvent& evt);

	DECLARE_EVENT_TABLE();

};

template <typename WebControlT = wxWebControl>
class wxCustomBrowserWebPanel : public wxWebPanelBase
{
	
public:
	void SetWebControl(WebControlT* browser)
	{
		DestroyGui();
		m_browser = browser;
		ArrangeGui();
	}

	WebControlT* GetWebControl() const { return static_cast<WebControlT *>(m_browser); }

	wxCustomBrowserWebPanel() : wxWebPanelBase() { SetWebControl(new WebControlT); }

	wxCustomBrowserWebPanel(wxWindow *parent,
				wxWindowID id=wxID_ANY,
				const wxPoint &pos=wxDefaultPosition,
				const wxSize &size=wxDefaultSize,
				long  style=wxTAB_TRAVERSAL,
				const wxString &name=wxPanelNameStr) :
	wxWebPanelBase(parent, id, pos, size, style, name)
	{ SetWebControl(new WebControlT); }

	~wxCustomBrowserWebPanel() {}
	
};

typedef wxCustomBrowserWebPanel<wxWebControl> wxWebPanel;

#endif  // #ifndef _WEBPANEL_H_

