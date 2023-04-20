#pragma once

#include <ChessGame.h>
#include <ChessAI.h>
#include <wx/setup.h>
#include <wx/app.h>
#include <wx/config.h>

class ChessFrame;
class ChessBot;

class ChessApp : public wxApp
{
public:
	ChessApp();
	virtual ~ChessApp();

	virtual bool OnInit(void) override;
	virtual int OnExit(void) override;

	void FlipTurn();

	enum class PlayerType
	{
		HUMAN,
		COMPUTER
	};

	PlayerType GetCurrentPlayerType();
	PlayerType GetPlayerType(ChessEngine::ChessColor color);

	void SetCurrentPlayerType(PlayerType playerType);
	void SetPlayerType(ChessEngine::ChessColor color, PlayerType playerType);

	bool LoadGame(const wxString& gameFile);
	bool SaveGame(const wxString& gameFile);

	ChessFrame* frame;
	ChessEngine::ChessGame* game;
	ChessEngine::ChessColor whoseTurn;
	PlayerType playerType[2];
	ChessBot* bot;
	wxConfig config;
};

wxDECLARE_APP(ChessApp);