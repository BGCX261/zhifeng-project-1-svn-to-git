/*****************
2012.05.06
*****************/
#ifndef _BROWSER_H_
#define _BROWSER_H_

#include <wx/filename.h>
#include <wx/notebook.h>
#include <wx/textfile.h>
#include <wx/event.h>
#include <wx/splitter.h>
#include "wx/config.h"

#include "reportview.h"
#include "mapwindow.h"

//Global variables here
//...


// @idea: wxWidgets has a XML-based resource system, one day we might want to
//        use it; currently however I can see no immediate reason
class MainMenuBar: public wxMenuBar {
public:
	MainMenuBar();
};


class BrowserFrame: public wxFrame
{
public:

	BrowserFrame(	wxWindow* parent, int id, const wxString& title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = wxDEFAULT_FRAME_STYLE );

	~BrowserFrame();

private:
	//visual components
	MainMenuBar				*mainMenu;
	wxSplitterWindow        *v_split;
	wxSplitterWindow        *h_split_l;
	wxSplitterWindow        *h_split_r;
	wxReportView            *sensorlist;
	wxFont                   node_font;
	wxReportView            *sensorinfo;
	wxWindow                *meshview;
	wxListBox               *accesslist;
	wxNotebook              *logs;
	/*
	wxLogView               *app_log;
	wxLogView               *debug_log;
	wxLogView               *stderr_log;
	wxLogView               *stdout_log;
	*/
	wxStatusBar             *status;


	void setup_sensorlist();
	void update_sensorlist(){};

	void setup_sensorinfo();
	void update_sensorinfo(){};

	// events handling
	void					OnQuit(wxCommandEvent& event);
	void					OnAbout(wxCommandEvent& event);
	void					OnPaint(wxPaintEvent& event);

	DECLARE_EVENT_TABLE()
};



#endif // _BROWSER_H_


