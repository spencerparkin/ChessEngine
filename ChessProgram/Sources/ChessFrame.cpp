#include "ChessFrame.h"
#include "ChessCanvas.h"
#include "ChessApp.h"
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/aboutdlg.h>

ChessFrame::ChessFrame(wxWindow* parent, const wxPoint& pos, const wxSize& size) : wxFrame(parent, wxID_ANY, "Chess", pos, size)
{
	wxMenu* gameMenu = new wxMenu();
	gameMenu->Append(new wxMenuItem(gameMenu, ID_NewGame, "New Game", "Start a new game of Chess."));
	gameMenu->AppendSeparator();
	gameMenu->Append(new wxMenuItem(gameMenu, ID_Exit, "Exit", "Terminate this program."));

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(new wxMenuItem(helpMenu, ID_About, "About", "Show the about-box."));

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(gameMenu, "Game");
	menuBar->Append(helpMenu, "Help");
	this->SetMenuBar(menuBar);

	this->SetStatusBar(new wxStatusBar(this));

	this->Bind(wxEVT_MENU, &ChessFrame::OnNewGame, this, ID_NewGame);
	this->Bind(wxEVT_MENU, &ChessFrame::OnAbout, this, ID_About);
	this->Bind(wxEVT_MENU, &ChessFrame::OnExit, this, ID_Exit);

	this->canvas = new ChessCanvas(this);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(this->canvas, 1, wxGROW);
	this->SetSizer(sizer);
}

/*virtual*/ ChessFrame::~ChessFrame()
{
}

void ChessFrame::OnNewGame(wxCommandEvent& event)
{
	delete wxGetApp().game;
	wxGetApp().game = new ChessEngine::ChessGame();

	this->Refresh();
}

void ChessFrame::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo aboutDialogInfo;

	aboutDialogInfo.SetName("Chess");
	aboutDialogInfo.SetVersion("1.0");
	aboutDialogInfo.SetDescription("This program is an attempt to implement the game of Chess.");
	aboutDialogInfo.SetCopyright("Copyright (C) 2023 -- Spencer T. Parkin <SpencerTParkin@gmail.com>");

	wxAboutBox(aboutDialogInfo);
}

void ChessFrame::OnExit(wxCommandEvent& event)
{
	this->Close(true);
}