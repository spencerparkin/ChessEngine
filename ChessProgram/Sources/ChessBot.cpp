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
	if (this->maxDepth == COMPUTER_HARD_MAX_DEPTH)
	{
		this->progressDialog = new wxProgressDialog("Chess AI", "Thinking...", 100, wxGetApp().frame, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
		this->progressDialog->SetPosition(wxGetApp().frame->GetPosition());
	}
	else
	{
		wxGetApp().frame->gaugeBar->SetValue(0);
	}
}

/*virtual*/ void ChessBot::ProgressEnd()
{
	wxGetApp().frame->gaugeBar->SetValue(0);

	if (this->progressDialog)
	{
		delete this->progressDialog;
		this->progressDialog = nullptr;
	}
}

/*virtual*/ bool ChessBot::ProgressUpdate(float percentage)
{
	float range = 1.0f;
	if (this->maxDepth == COMPUTER_HARD_MAX_DEPTH)
		range = (float)this->progressDialog->GetRange();
	else
		range = (float)wxGetApp().frame->gaugeBar->GetRange();

	int progressValue = (int)::roundf(percentage * range);
	bool keepGoing = true;

	if (this->maxDepth == COMPUTER_HARD_MAX_DEPTH)
		keepGoing = this->progressDialog->Update(progressValue);
	else
		wxGetApp().frame->gaugeBar->SetValue(progressValue);

	return keepGoing;
}