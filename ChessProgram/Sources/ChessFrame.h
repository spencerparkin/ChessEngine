#pragma once

#include <ChessCommon.h>
#include <wx/frame.h>
#include <wx/listbox.h>
#include <wx/button.h>

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

	enum
	{
		ID_NewGame = wxID_HIGHEST,
		ID_About,
		ID_Exit,
		ID_FlipBoard
	};

	void UpdateStatusBar();
	void UpdatePanel();

	ChessCanvas* canvas;
	wxListBox* moveListBox;
	wxButton* undoButton;
	wxButton* redoButton;
	ChessEngine::ChessMoveArray redoMoveArray;
};