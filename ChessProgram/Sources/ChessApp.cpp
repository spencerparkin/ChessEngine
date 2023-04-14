#include "ChessApp.h"
#include "ChessFrame.h"
#include "ChessBot.h"
#include <wx/image.h>

wxIMPLEMENT_APP(ChessApp);

ChessApp::ChessApp()
{
	this->frame = nullptr;
	this->game = new ChessEngine::ChessGame();
	this->game->Reset();
	this->whoseTurn = ChessEngine::ChessColor::White;
	this->playerType[0] = PlayerType::HUMAN;
	this->playerType[1] = PlayerType::HUMAN;
	this->bot = new ChessBot(3);
}

/*virtual*/ ChessApp::~ChessApp()
{
	delete this->game;
	delete this->bot;
}

/*virtual*/ bool ChessApp::OnInit(void)
{
	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();

	this->frame = new ChessFrame(nullptr, wxDefaultPosition, wxSize(1300, 700));
	this->frame->Show();

	return true;
}

/*virtual*/ int ChessApp::OnExit(void)
{
	return 0;
}

void ChessApp::FlipTurn()
{
	if (this->whoseTurn == ChessEngine::ChessColor::White)
		this->whoseTurn = ChessEngine::ChessColor::Black;
	else
		this->whoseTurn = ChessEngine::ChessColor::White;
}

ChessApp::PlayerType ChessApp::GetCurrentPlayerType()
{
	return this->GetPlayerType(this->whoseTurn);
}

void ChessApp::SetCurrentPlayerType(PlayerType playerType)
{
	this->SetPlayerType(this->whoseTurn, playerType);
}

ChessApp::PlayerType ChessApp::GetPlayerType(ChessEngine::ChessColor color)
{
	return this->playerType[int(color)];
}

void ChessApp::SetPlayerType(ChessEngine::ChessColor color, PlayerType playerType)
{
	this->playerType[int(color)] = playerType;
}