#pragma once

#include <wx/frame.h>

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

	enum
	{
		ID_NewGame = wxID_HIGHEST,
		ID_About,
		ID_Exit,
		ID_FlipBoard
	};

	void UpdateStatusBar();

	ChessCanvas* canvas;
};