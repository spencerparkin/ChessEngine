#include "ChessApp.h"
#include "ChessFrame.h"

wxIMPLEMENT_APP(ChessApp);

ChessApp::ChessApp()
{
	this->frame = nullptr;
	this->game = new ChessEngine::ChessGame();
}

/*virtual*/ ChessApp::~ChessApp()
{
	delete this->game;
}

/*virtual*/ bool ChessApp::OnInit(void)
{
	if (!wxApp::OnInit())
		return false;

	this->frame = new ChessFrame(nullptr, wxDefaultPosition, wxSize(1000, 600));
	this->frame->Show();

	return true;
}

/*virtual*/ int ChessApp::OnExit(void)
{
	return 0;
}