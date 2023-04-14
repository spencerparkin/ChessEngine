#include "ChessBot.h"
#include "ChessApp.h"
#include "ChessFrame.h"

ChessBot::ChessBot(int maxDepth) : ChessEngine::ChessMinimaxAI(maxDepth)
{
	this->progressDialog = nullptr;
}

/*virtual*/ ChessBot::~ChessBot()
{
}

/*virtual*/ void ChessBot::ProgressBegin()
{
	this->progressDialog = new wxProgressDialog("Chess AI", "Thinking...", 100, wxGetApp().frame, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
}

/*virtual*/ void ChessBot::ProgressEnd()
{
	delete this->progressDialog;
	this->progressDialog = nullptr;
}

/*virtual*/ bool ChessBot::ProgressUpdate(float percentage)
{
	int progressValue = (int)::roundf(percentage * 100.0f);
	return this->progressDialog->Update(progressValue);
}