/*
 1. my first, smallest wxwidgets program. 2012.05.02
 2. Main frame, general structure. 5.10
 3. make config file work, save basic params (size, position, etc.) and load on next start up
 4. clean up, make it as clean as possible

 */

#include "wx/wx.h"
#include <wx/dcbuffer.h>

#include "browser.h"
#include "wxres.h"


//这是主窗口
BrowserFrame::BrowserFrame( wxWindow* parent, int id, const wxString& title,
							const wxPoint& pos, const wxSize& size, long style ) :
wxFrame( parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE )
{
	// create visual components
	// main menu bar
	
	mainMenu = new MainMenuBar();

	v_split = new wxSplitterWindow	(	this, ID_VSPLIT, wxDefaultPosition, 
										wxDefaultSize, wxSP_NOBORDER | wxSP_LIVE_UPDATE );

	h_split_l = new wxSplitterWindow(	v_split, ID_HSPLIT, wxDefaultPosition, 
										wxDefaultSize, wxSP_NOBORDER | wxSP_LIVE_UPDATE );

	sensorlist = new wxReportView		(	h_split_l, ID_sensorlist, wxDefaultPosition, wxDefaultSize,
										wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxSUNKEN_BORDER |	wxLC_SINGLE_SEL );

	sensorinfo = new wxReportView		(	h_split_l, -1, wxDefaultPosition, wxDefaultSize,
										wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxSUNKEN_BORDER |	wxLC_SINGLE_SEL );

	h_split_r = new wxSplitterWindow	(	v_split, ID_HSPLIT, wxDefaultPosition, wxDefaultSize,
										wxSP_NOBORDER | wxSP_LIVE_UPDATE );

	meshview = new wxMapWindow			(	h_split_r, ID_meshview, wxDefaultPosition, wxDefaultSize,
										wxBORDER_DOUBLE/*wxBORDER_NONE/*wxBORDER_STATIC/*wxSUNKEN_BORDER*/|wxTAB_TRAVERSAL );

	logs = new wxNotebook( h_split_r, -1, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT/*wxNB_BOTTOM/*wxNB_TOP*/ );
	/*
	app_log = new wxLogView(
		logs, -1, wxT( "" ), wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2 | wxTE_READONLY |
		wxALWAYS_SHOW_SB );

	accesslist = new wxListBox(logs, ID_accesslist, wxDefaultPosition, wxDefaultSize,
		wxArrayString(), wxLB_SINGLE);

	debug_log = new wxLogView(
		logs, -1, wxT( "" ), wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2 | wxTE_READONLY |
		wxALWAYS_SHOW_SB );
	stderr_log = new wxLogView(
		logs, -1, wxT( "" ), wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2 | wxTE_READONLY |
		wxALWAYS_SHOW_SB );
	stdout_log = new wxLogView(
		logs, -1, wxT( "" ), wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2 | wxTE_READONLY |
		wxALWAYS_SHOW_SB );
	debug_log->Show( false );
	stderr_log->Show( false );
	stdout_log->Show( false );
	*/
	status = new wxStatusBar(this, -1 );

	// set visible properties
	SetIcon( wxIcon( wxICON( MESHCOM_ICON ) ) );
	SetMinSize( wxSize( 500, 400 ) );
	SetMenuBar( mainMenu );
	SetStatusBar( status );

	v_split->SetMinimumPaneSize( 20 );
	h_split_l->SetMinimumPaneSize( 20 );
	h_split_r->SetMinimumPaneSize( 20 );
	h_split_l->SetSashGravity( 0.5 );
	h_split_r->SetSashGravity( 0.5 );

	sensorlist->SetMinSize( wxSize( 150, 200 ) );
	sensorinfo->SetMinSize( wxSize( 150, 0 ) );
	meshview->SetMinSize( wxSize( 250, 0 ) );
	logs->SetMinSize( wxSize( 250, 0 ) );

	// layout
	v_split->SplitVertically( h_split_l, h_split_r );
	h_split_l->SplitHorizontally( sensorlist, sensorinfo );
	h_split_r->SplitHorizontally( meshview, logs );

	SetStatusText( _T("Welcome to Sensor Browser demo!") );

	// restore frame position and size
	wxConfigBase *pConfig = wxConfigBase::Get();

	int x = pConfig->Read(wxT("/MainFrame/x"), 100L),
		y = pConfig->Read(wxT("/MainFrame/y"), 100L),
		w = pConfig->Read(wxT("/MainFrame/w"), 600L),
		h = pConfig->Read(wxT("/MainFrame/h"), 400L);
	Move(x, y);
	SetClientSize(w, h);

	// restore splitter windows size
	v_split->SetSashPosition(pConfig->Read(wxT("/SplitWin/vpos"), v_split->GetSashPosition()));
	h_split_l->SetSashPosition(pConfig->Read(wxT("/SplitWin/hlpos"), h_split_l->GetSashPosition()));
	h_split_r->SetSashPosition(pConfig->Read(wxT("/SplitWin/hrpos"), h_split_r->GetSashPosition()));

	setup_sensorlist(); //仅仅填入一些demo用的假数据
	setup_sensorinfo(); //仅仅填入一些demo用的假数据

}

BrowserFrame::~BrowserFrame()
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	if ( pConfig == NULL )
		return;

	// save the control's values to the config

	// save the main frame position
	int x, y, w, h;
	GetClientSize(&w, &h);
	GetPosition(&x, &y);
	pConfig->Write(wxT("/MainFrame/x"), (long) x);
	pConfig->Write(wxT("/MainFrame/y"), (long) y);
	pConfig->Write(wxT("/MainFrame/w"), (long) w);
	pConfig->Write(wxT("/MainFrame/h"), (long) h);
	// save splitter window position
	int	vp = v_split->GetSashPosition();
	int hlp = h_split_l->GetSashPosition();
	int hrp = h_split_r->GetSashPosition();
	pConfig->Write(wxT("/SplitWin/vpos"), (long)vp);
	pConfig->Write(wxT("/SplitWin/hlpos"), (long)hlp);
	pConfig->Write(wxT("/SplitWin/hrpos"), (long)hrp);

};


void BrowserFrame::setup_sensorlist()
{
	sensorlist->InsertColumn( 0, wxT( "SENSOR ID" ) );
	sensorlist->InsertColumn( 1, wxT( "STATUS" ) );

	node_font = GetFont();
	node_font.Larger();
	//node_font.SetPixelSize( wxSize(7,12) );

	sensorlist->SetPair(0,_T("Sensor 1"),_T("Available"));
	//sensorlist->SetItemFont(0, node_font.Larger());
	sensorlist->SetPair(1,_T("Sensor 2"),_T("Available"));
	//sensorlist->SetItemFont(1, node_font.Larger());
	sensorlist->SetPair(2,_T("Sensor 3"),_T("Available"));
	//sensorlist->SetItemFont(2, node_font.Larger());
	sensorlist->SetPair(3,_T("Sensor 4"),_T("Available"));
	//sensorlist->SetItemFont(3, node_font.Larger());

}

void BrowserFrame::setup_sensorinfo()
{
	sensorinfo->InsertColumn( 0, wxT( "PROPERTY" ) );
	sensorinfo->InsertColumn( 1, wxT( "VALUE" ) );
	//bold_node_font = GetFont();
	//bold_node_font.SetWeight( wxFONTWEIGHT_BOLD );
}


void BrowserFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void BrowserFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox( _T("wxWidgets Hello world sample\n\n")
		_T("Version 0.1\n") _T("by Zhifeng @2012"),
		_T("About Hello World"),
		wxOK | wxCANCEL| wxICON_INFORMATION, this);

}



//------------------------------------------------------------------------------
// event processing
BEGIN_EVENT_TABLE(BrowserFrame, wxFrame)
    EVT_MENU(wxID_EXIT, BrowserFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, BrowserFrame::OnAbout)

END_EVENT_TABLE()


//------------------------------------------------------------------------------


MainMenuBar::MainMenuBar()
{
	wxMenu* m = new wxMenu();
	m->Append(
				wxID_OPEN, wxGetStockLabel( wxID_OPEN, true ),
				wxT( "Load a previously saved mesh snapshot" ) );

	m->Append(
		wxID_SAVEAS, wxGetStockLabel( wxID_SAVEAS, true ),
		wxT( "Save the current mesh" ) );

	m->AppendSeparator();

	m->Append(
		ID_LIVE, wxT( "Go &Live\tCtrl+L" ),
		wxT( "View the actual mesh" ) );

	m->AppendCheckItem(
		ID_AUTOPOLL, wxT( "Freeze\tCtrl+F" ),
		wxT( "Stop recollecting the information from the driver" ) );

	m->AppendSeparator();

	m->Append(
		wxID_EXIT, wxGetStockLabel( wxID_EXIT, true ),
		wxT( "Quit" ) );

	Append( m, wxT( "&File" ) );


	m = new wxMenu();
	m->AppendCheckItem(
		ID_LABELS, wxT( "Show &Node Names\tAlt+N" ),
		wxT( "Show node labels on the mesh diagram" ) );

	m->AppendCheckItem(
		ID_DEBUG_PANES, wxT( "Show &Debug Panes\tAlt+D" ),
		wxT( "Show debug logs" ) );

	Append( m, wxT( "&View" ) );

	m = new wxMenu();
	m->Append(
		ID_CREATE_KEYS, wxT( "&Create Keys\tCtrl+K" ),
		wxT( "Generate public/private administrator key pair file" ) );
	m->Append(
		ID_IMPORT_KEYS, wxT( "&Import Keys\tCtrl+I" ),
		wxT( "Import public/private administrator key pair file" ) );
	Append( m, wxT( "&Keys" ) );

	m = new wxMenu();
	m->Append(
		wxID_ABOUT, wxT( "&About...\tF1" ), wxT( "&About..." ) );
	Append( m, wxT( "&Help" ) );
}



//=======================================================================


class MyApp: public wxApp
{
	// implement base class virtual funcs
	virtual bool OnInit();
	virtual int OnExit();
};

bool MyApp::OnInit()
{
	if ( !wxApp::OnInit() )
		return false;

	// application and vendor name are used by wxConfig to construct the name
	// of the config file/registry key and must be set before the first call
	// to Get() if you want to override the default values (the application
	// name is the name of the executable and the vendor name is the same)
	SetVendorName(wxT("Mil-Spec Engineering"));
	
	wxConfigBase *pConfig = wxConfigBase::Get();

	// uncomment this to force writing back of the defaults for all values
	//pConfig->SetRecordDefaults();

	// where you can also specify the file names explicitly if you wish.
	// Of course, calling Set() is optional and you only must do it if
	// you want to later retrieve this pointer with Get().

    BrowserFrame *frame = new BrowserFrame( NULL, -1, _T("Hello World"), wxDefaultPosition, wxSize(640,480) );
	
    frame->Show(true);
	frame->Update();
    SetTopWindow(frame);
    return true;
}

int MyApp::OnExit()
{
	// clean up: Set() returns the active config object as Get() does, but unlike
	// Get() it doesn't try to create one if there is none (definitely not what
	// we want here!)
	delete wxConfigBase::Set((wxConfigBase *) NULL);

	return 0;
}

IMPLEMENT_APP(MyApp)


