#pragma once

#include <ChessCommon.h>
#include <wx/frame.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/gauge.h>
#include <wx/timer.h>

class ChessCanvas;

class ChessFrame : public wxFrame
{
public:
	ChessFrame(wxWindow* parent, const wxPoint& pos, const wxSize& size);
	virtual ~ChessFrame();

	void OnNewGame(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnGameStateChanged(wxCommandEvent& event);
	void OnFlipBoard(wxCommandEvent& event);
	void OnUpdateMenuItemUI(wxUpdateUIEvent& event);
	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnColorPlayerdByComputer(wxCommandEvent& event);
	void OnComputerDifficulty(wxCommandEvent& event);
	void OnTimerTick(wxTimerEvent& event);

	enum
	{
		ID_NewGame = wxID_HIGHEST,
		ID_About,
		ID_Exit,
		ID_FlipBoard,
		ID_WhitePlayedByComputer,
		ID_BlackPlayedByComputer,
		ID_ComputerDifficultyEasy,
		ID_ComputerDifficultyMedium,
		ID_ComputerDifficultyHard
	};

	void UpdateStatusBar();
	void UpdatePanel();
	void ComputerTakesTurn();

	ChessCanvas* canvas;
	wxListBox* moveListBox;
	wxButton* undoButton;
	wxButton* redoButton;
	wxGauge* gaugeBar;
	ChessEngine::ChessMoveArray redoMoveArray;
	wxTimer timer;
	bool inTimerTick;
};