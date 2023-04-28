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
	void OnSaveGame(wxCommandEvent& event);
	void OnLoadGame(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnGameStateChanged(wxCommandEvent& event);
	void OnFlipBoard(wxCommandEvent& event);
	void OnUpdateMenuItemUI(wxUpdateUIEvent& event);
	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnColorPlayerdByComputer(wxCommandEvent& event);
	void OnComputerDifficulty(wxCommandEvent& event);
	void OnComputerType(wxCommandEvent& event);
	void OnTimerTick(wxTimerEvent& event);
	void OnDrawCoordinates(wxCommandEvent& event);
	void OnDrawCaptures(wxCommandEvent& event);
	void OnDrawVisibilityArrows(wxCommandEvent& event);
	void OnCycleSquareTexture(wxCommandEvent& event);
	void OnHowToPlay(wxCommandEvent& event);
	void OnDoubleUndoRedo(wxCommandEvent& event);

	enum
	{
		ID_NewGame = wxID_HIGHEST,
		ID_SaveGame,
		ID_LoadGame,
		ID_About,
		ID_Exit,
		ID_FlipBoard,
		ID_WhitePlayedByComputer,
		ID_BlackPlayedByComputer,
		ID_ComputerDifficultyEasy,
		ID_ComputerDifficultyMedium,
		ID_ComputerDifficultyHard,
		ID_ComputerTypeMinimax,
		ID_ComputerTypeMCTS,
		ID_DrawCoordinates,
		ID_DrawCaptures,
		ID_DrawVisibilityArrows,
		ID_CycleLightSquareTexture,
		ID_CycleDarkSquareTexture,
		ID_HowToPlay,
		ID_DoubleUndo,
		ID_DoubleRedo
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