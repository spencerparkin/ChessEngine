#pragma once

#include <ChessGame.h>
#include <ChessAI.h>
#include <wx/setup.h>
#include <wx/app.h>

class ChessFrame;

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
	void SetPlayerType(ChessEngine::ChessColor color, PlayerType playerType);

	ChessFrame* frame;
	ChessEngine::ChessGame* game;
	ChessEngine::ChessColor whoseTurn;
	PlayerType playerType[2];
};

wxDECLARE_APP(ChessApp);