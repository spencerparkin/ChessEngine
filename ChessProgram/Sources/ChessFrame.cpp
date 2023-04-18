#include "ChessFrame.h"
#include "ChessCanvas.h"
#include "ChessApp.h"
#include "ChessBot.h"
#include <ChessMove.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/aboutdlg.h>
#include <wx/msgdlg.h>

ChessFrame::ChessFrame(wxWindow* parent, const wxPoint& pos, const wxSize& size) : wxFrame(parent, wxID_ANY, "Chess", pos, size), timer(this)
{
	wxMenu* gameMenu = new wxMenu();
	gameMenu->Append(new wxMenuItem(gameMenu, ID_NewGame, "New Game", "Start a new game of Chess."));
	gameMenu->AppendSeparator();
	gameMenu->Append(new wxMenuItem(gameMenu, ID_Exit, "Exit", "Terminate this program."));

	wxMenu* optionsMenu = new wxMenu();
	optionsMenu->Append(new wxMenuItem(optionsMenu, ID_FlipBoard, "Flip Board", "Flip the board 180 degrees.", wxITEM_CHECK));
	optionsMenu->AppendSeparator();
	optionsMenu->Append(new wxMenuItem(optionsMenu, ID_WhitePlayedByComputer, "Computer Plays White", "When it is WHITE's turn to play, the computer will take the turn.", wxITEM_CHECK));
	optionsMenu->Append(new wxMenuItem(optionsMenu, ID_BlackPlayedByComputer, "Computer Plays Black", "When it is BLACK's turn to play, the computer will take the turn.", wxITEM_CHECK));
	optionsMenu->AppendSeparator();
	optionsMenu->Append(new wxMenuItem(optionsMenu, ID_ComputerDifficultyEasy, "Computer Difficulty Easy", "Make the AI dumb.", wxITEM_CHECK));
	optionsMenu->Append(new wxMenuItem(optionsMenu, ID_ComputerDifficultyMedium, "Computer Difficulty Medium", "Make the AI somewhat smart.", wxITEM_CHECK));
	optionsMenu->Append(new wxMenuItem(optionsMenu, ID_ComputerDifficultyHard, "Computer Difficulty Hard", "Make the AI as smart as it can be.", wxITEM_CHECK));

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
	this->Bind(wxEVT_MENU, &ChessFrame::OnFlipBoard, this, ID_FlipBoard);
	this->Bind(wxEVT_MENU, &ChessFrame::OnColorPlayerdByComputer, this, ID_WhitePlayedByComputer);
	this->Bind(wxEVT_MENU, &ChessFrame::OnColorPlayerdByComputer, this, ID_BlackPlayedByComputer);
	this->Bind(wxEVT_MENU, &ChessFrame::OnComputerDifficulty, this, ID_ComputerDifficultyEasy);
	this->Bind(wxEVT_MENU, &ChessFrame::OnComputerDifficulty, this, ID_ComputerDifficultyMedium);
	this->Bind(wxEVT_MENU, &ChessFrame::OnComputerDifficulty, this, ID_ComputerDifficultyHard);
	this->Bind(wxEVT_UPDATE_UI, &ChessFrame::OnUpdateMenuItemUI, this, ID_FlipBoard);
	this->Bind(wxEVT_UPDATE_UI, &ChessFrame::OnUpdateMenuItemUI, this, ID_WhitePlayedByComputer);
	this->Bind(wxEVT_UPDATE_UI, &ChessFrame::OnUpdateMenuItemUI, this, ID_BlackPlayedByComputer);
	this->Bind(wxEVT_UPDATE_UI, &ChessFrame::OnUpdateMenuItemUI, this, ID_ComputerDifficultyEasy);
	this->Bind(wxEVT_UPDATE_UI, &ChessFrame::OnUpdateMenuItemUI, this, ID_ComputerDifficultyMedium);
	this->Bind(wxEVT_UPDATE_UI, &ChessFrame::OnUpdateMenuItemUI, this, ID_ComputerDifficultyHard);
	this->Bind(EVT_GAME_STATE_CHANGED, &ChessFrame::OnGameStateChanged, this);
	this->Bind(wxEVT_TIMER, &ChessFrame::OnTimerTick, this);

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

	this->gaugeBar = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL | wxGA_SMOOTH);

	wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);
	vertSizer->Add(this->moveListBox, 1, wxGROW);
	vertSizer->Add(horizSizer, 0, wxGROW);
	vertSizer->Add(this->gaugeBar, 0, wxGROW);

	panel->SetSizer(vertSizer);

	splitter->SetSashPosition(300);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(splitter, 1, wxGROW);
	this->SetSizer(mainSizer);

	this->UpdateStatusBar();
	this->UpdatePanel();

	this->timer.Start(60);
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
		wxGetApp().SetPlayerType(ChessEngine::ChessColor::White, ChessApp::PlayerType::HUMAN);
		wxGetApp().SetPlayerType(ChessEngine::ChessColor::Black, ChessApp::PlayerType::HUMAN);

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
		wxGetApp().SetPlayerType(ChessEngine::ChessColor::White, ChessApp::PlayerType::HUMAN);
		wxGetApp().SetPlayerType(ChessEngine::ChessColor::Black, ChessApp::PlayerType::HUMAN);

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
		case ID_WhitePlayedByComputer:
		{
			event.Check(wxGetApp().GetPlayerType(ChessEngine::ChessColor::White) == ChessApp::PlayerType::COMPUTER);
			break;
		}
		case ID_BlackPlayedByComputer:
		{
			event.Check(wxGetApp().GetPlayerType(ChessEngine::ChessColor::Black) == ChessApp::PlayerType::COMPUTER);
			break;
		}
		case ID_ComputerDifficultyEasy:
		{
			event.Check(wxGetApp().bot->maxDepth == COMPUTER_EASY_MAX_DEPTH);
			break;
		}
		case ID_ComputerDifficultyMedium:
		{
			event.Check(wxGetApp().bot->maxDepth == COMPUTER_MEDIUM_MAX_DEPTH);
			break;
		}
		case ID_ComputerDifficultyHard:
		{
			event.Check(wxGetApp().bot->maxDepth == COMPUTER_HARD_MAX_DEPTH);
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

void ChessFrame::OnTimerTick(wxTimerEvent& event)
{
	double deltaTimeSeconds = double(this->timer.GetInterval()) / 1000.0;
	this->canvas->Animate(deltaTimeSeconds);

	if (wxGetApp().GetCurrentPlayerType() == ChessApp::PlayerType::COMPUTER && !this->canvas->IsAnimating())
		this->ComputerTakesTurn();
}

void ChessFrame::ComputerTakesTurn()
{
	ChessEngine::ChessMove* move = wxGetApp().bot->CalculateRecommendedMove(wxGetApp().whoseTurn, wxGetApp().game);
	if (!move)
	{
		wxGetApp().SetPlayerType(ChessEngine::ChessColor::White, ChessApp::PlayerType::HUMAN);
		wxGetApp().SetPlayerType(ChessEngine::ChessColor::Black, ChessApp::PlayerType::HUMAN);
	}
	else
	{
		this->canvas->AnimateMove(move);

		wxGetApp().game->PushMove(move);
		wxGetApp().FlipTurn();

		wxCommandEvent stateChangedEvent(EVT_GAME_STATE_CHANGED);
		wxPostEvent(wxGetApp().frame, stateChangedEvent);

		this->canvas->Refresh();
	}
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

	if (this->moveListBox->GetCount() > 0)
		this->moveListBox->EnsureVisible(this->moveListBox->GetCount() - 1);
}

void ChessFrame::OnNewGame(wxCommandEvent& event)
{
	wxGetApp().game->Reset();
	wxGetApp().whoseTurn = ChessEngine::ChessColor::White;

	wxCommandEvent stateChangedEvent(EVT_GAME_STATE_CHANGED);
	wxPostEvent(this, stateChangedEvent);

	this->Refresh();
}

void ChessFrame::OnColorPlayerdByComputer(wxCommandEvent& event)
{
	ChessEngine::ChessColor color;
	switch (event.GetId())
	{
		case ID_WhitePlayedByComputer:
		{
			color = ChessEngine::ChessColor::White;
			break;
		}
		case ID_BlackPlayedByComputer:
		{
			color = ChessEngine::ChessColor::Black;
			break;
		}
		default:
		{
			return;
		}
	}

	ChessApp::PlayerType playerType = wxGetApp().GetPlayerType(color);

	if (playerType == ChessApp::PlayerType::HUMAN)
		playerType = ChessApp::PlayerType::COMPUTER;
	else
		playerType = ChessApp::PlayerType::HUMAN;

	wxGetApp().SetPlayerType(color, playerType);

	wxCommandEvent stateChangedEvent(EVT_GAME_STATE_CHANGED);
	wxPostEvent(wxGetApp().frame, stateChangedEvent);
}

void ChessFrame::OnComputerDifficulty(wxCommandEvent& event)
{
	switch (event.GetId())
	{
		case ID_ComputerDifficultyEasy:
		{
			wxGetApp().bot->maxDepth = COMPUTER_EASY_MAX_DEPTH;
			break;
		}
		case ID_ComputerDifficultyMedium:
		{
			wxGetApp().bot->maxDepth = COMPUTER_MEDIUM_MAX_DEPTH;
			break;
		}
		case ID_ComputerDifficultyHard:
		{
			wxGetApp().bot->maxDepth = COMPUTER_HARD_MAX_DEPTH;
			break;
		}
	}
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