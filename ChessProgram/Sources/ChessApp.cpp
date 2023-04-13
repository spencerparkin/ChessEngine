#include "ChessApp.h"
#include "ChessFrame.h"
#include <wx/image.h>

wxIMPLEMENT_APP(ChessApp);

ChessApp::ChessApp()
{
	this->frame = nullptr;
	this->game = new ChessEngine::ChessGame();
	this->game->Reset();
	this->whoseTurn = ChessEngine::ChessColor::White;
}

/*virtual*/ ChessApp::~ChessApp()
{
	delete this->game;
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