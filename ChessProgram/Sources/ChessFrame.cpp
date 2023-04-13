#include "ChessFrame.h"
#include "ChessCanvas.h"
#include "ChessApp.h"
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/aboutdlg.h>

// TODO: Add undo/redo option.
// TODO: Show captures and move history, maybe in a list widget.
// TODO: Implement pawn promotion dialog.
// TODO: Implement computer-suggested move using mini-max algorithm.

ChessFrame::ChessFrame(wxWindow* parent, const wxPoint& pos, const wxSize& size) : wxFrame(parent, wxID_ANY, "Chess", pos, size)
{
	wxMenu* gameMenu = new wxMenu();
	gameMenu->Append(new wxMenuItem(gameMenu, ID_NewGame, "New Game", "Start a new game of Chess."));
	gameMenu->AppendSeparator();
	gameMenu->Append(new wxMenuItem(gameMenu, ID_Exit, "Exit", "Terminate this program."));

	wxMenu* optionsMenu = new wxMenu();
	optionsMenu->Append(new wxMenuItem(optionsMenu, ID_FlipBoard, "Flip Board", "Flip the board 180 degrees.", wxITEM_CHECK));

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(new wxMenuItem(helpMenu, ID_About, "About", "Show the about-box."));

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(gameMenu, "Game");
	menuBar->Append(optionsMenu, "Options");
	menuBar->Append(helpMenu, "Help");
	this->SetMenuBar(menuBar);

	this->SetStatusBar(new wxStatusBar(this));

	this->Bind(wxEVT_MENU, &ChessFrame::OnNewGame, this, ID_NewGame);
	this->Bind(wxEVT_MENU, &ChessFrame::OnAbout, this, ID_About);
	this->Bind(wxEVT_MENU, &ChessFrame::OnExit, this, ID_Exit);
	this->Bind(EVT_GAME_STATE_CHANGED, &ChessFrame::OnGameStateChanged, this);
	this->Bind(wxEVT_MENU, &ChessFrame::OnFlipBoard, this, ID_FlipBoard);
	this->Bind(wxEVT_UPDATE_UI, &ChessFrame::OnUpdateMenuItemUI, this, ID_FlipBoard);

	this->canvas = new ChessCanvas(this);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(this->canvas, 1, wxGROW);
	this->SetSizer(sizer);

	this->UpdateStatusBar();
}

/*virtual*/ ChessFrame::~ChessFrame()
{
}

void ChessFrame::OnFlipBoard(wxCommandEvent& event)
{
	switch (this->canvas->renderOrientation)
	{
		case ChessCanvas::RenderOrientation::RENDER_NORMAL:
		{
			this->canvas->renderOrientation = ChessCanvas::RenderOrientation::RENDER_FLIPPED;
			break;
		}
		case ChessCanvas::RenderOrientation::RENDER_FLIPPED:
		{
			this->canvas->renderOrientation = ChessCanvas::RenderOrientation::RENDER_NORMAL;
			break;
		}
	}

	this->canvas->Refresh();
}

void ChessFrame::OnUpdateMenuItemUI(wxUpdateUIEvent& event)
{
	switch (event.GetId())
	{
		case ID_FlipBoard:
		{
			event.Check(this->canvas->renderOrientation == ChessCanvas::RenderOrientation::RENDER_FLIPPED);
			break;
		}
	}
}

void ChessFrame::OnGameStateChanged(wxCommandEvent& event)
{
	this->UpdateStatusBar();
}

void ChessFrame::UpdateStatusBar()
{
	wxString text;

	if (wxGetApp().whoseTurn == ChessEngine::ChessColor::White)
		text = "Waiting for WHITE to move.";
	else
		text = "Waiting for BLACK to move.";

	ChessEngine::ChessMoveArray moveArray;
	ChessEngine::GameResult result = wxGetApp().game->GenerateAllLegalMovesForColor(wxGetApp().whoseTurn, moveArray);
	
	if (result == ChessEngine::GameResult::CheckMate)
		text += "  *** CHECK & MATE! ***";
	else if (result == ChessEngine::GameResult::StaleMate)
		text += "  *** STALE MATE! ***";
	else if (result == ChessEngine::GameResult::Check)
		text += "  *** You are in check! ***";

	ChessEngine::DeleteMoveArray(moveArray);

	this->GetStatusBar()->SetStatusText(text);
}

void ChessFrame::OnNewGame(wxCommandEvent& event)
{
	wxGetApp().game->Reset();
	wxGetApp().whoseTurn = ChessEngine::ChessColor::White;

	wxCommandEvent stateChangedEvent(EVT_GAME_STATE_CHANGED);
	wxPostEvent(this, stateChangedEvent);

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