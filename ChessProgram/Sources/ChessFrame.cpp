#include "ChessFrame.h"
#include "ChessCanvas.h"
#include "ChessApp.h"
#include <ChessMove.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/aboutdlg.h>

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

	wxSplitterWindow* splitter = new wxSplitterWindow(this);

	this->canvas = new ChessCanvas(splitter);
	wxPanel* panel = new wxPanel(splitter, wxID_ANY);

	splitter->SplitVertically(panel, this->canvas);

	this->moveListBox = new wxListBox(panel, wxID_ANY);

	this->undoButton = new wxButton(panel, wxID_ANY, "Undo");
	this->redoButton = new wxButton(panel, wxID_ANY, "Redo");

	this->undoButton->Bind(wxEVT_BUTTON, &ChessFrame::OnUndo, this);
	this->redoButton->Bind(wxEVT_BUTTON, &ChessFrame::OnRedo, this);

	wxBoxSizer* horizSizer = new wxBoxSizer(wxHORIZONTAL);
	horizSizer->Add(this->undoButton, 1, wxGROW);
	horizSizer->Add(this->redoButton, 1, wxGROW);

	wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);
	vertSizer->Add(this->moveListBox, 1, wxGROW);
	vertSizer->Add(horizSizer, 0, 0);

	panel->SetSizer(vertSizer);

	splitter->SetSashPosition(300);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(splitter, 1, wxGROW);
	this->SetSizer(mainSizer);

	this->UpdateStatusBar();
	this->UpdatePanel();
}

/*virtual*/ ChessFrame::~ChessFrame()
{
	ChessEngine::DeleteMoveArray(this->redoMoveArray);
}

void ChessFrame::OnUndo(wxCommandEvent& event)
{
	ChessEngine::ChessGame* game = wxGetApp().game;
	if (game->GetNumMoves() > 0)
	{
		ChessEngine::ChessMove* move = game->PopMove();
		this->redoMoveArray.push_back(move);

		wxGetApp().FlipTurn();

		wxCommandEvent stateChangedEvent(EVT_GAME_STATE_CHANGED);
		stateChangedEvent.SetString("undo");
		wxPostEvent(this, stateChangedEvent);

		this->canvas->Refresh();
	}
}

void ChessFrame::OnRedo(wxCommandEvent& event)
{
	if (this->redoMoveArray.size() > 0)
	{
		ChessEngine::ChessGame* game = wxGetApp().game;
		ChessEngine::ChessMove* move = this->redoMoveArray.back();
		this->redoMoveArray.pop_back();

		game->PushMove(move);

		wxGetApp().FlipTurn();

		wxCommandEvent stateChangedEvent(EVT_GAME_STATE_CHANGED);
		stateChangedEvent.SetString("redo");
		wxPostEvent(this, stateChangedEvent);

		this->canvas->Refresh();
	}
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
	if (event.GetString() != "undo" && event.GetString() != "redo")
	{
		ChessEngine::DeleteMoveArray(this->redoMoveArray);
	}

	this->UpdateStatusBar();
	this->UpdatePanel();
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

void ChessFrame::UpdatePanel()
{
	this->undoButton->Enable(wxGetApp().game->GetNumMoves() > 0);
	this->redoButton->Enable(this->redoMoveArray.size() > 0);

	this->moveListBox->Clear();

	for (int i = 0; i < wxGetApp().game->GetNumMoves(); i++)
	{
		const ChessEngine::ChessMove* move = wxGetApp().game->GetMove(i);
		this->moveListBox->Insert(wxString(move->GetDescription().c_str()), i);
	}
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