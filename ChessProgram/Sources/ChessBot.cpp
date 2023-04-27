#include "ChessBot.h"
#include "ChessApp.h"
#include "ChessFrame.h"

//---------------------------------------- ChessBotProgressIndicator ----------------------------------------

ChessBotProgressIndicator::ChessBotProgressIndicator()
{
	this->startTimeTicks = 0;
	this->dialogTimeoutSeconds = 2.0;
	this->progressDialog = nullptr;
}

/*virtual*/ ChessBotProgressIndicator::~ChessBotProgressIndicator()
{
}

/*virtual*/ void ChessBotProgressIndicator::ProgressBegin()
{
	this->startTimeTicks = ::clock();
	wxGetApp().frame->gaugeBar->SetValue(0);
}

/*virtual*/ bool ChessBotProgressIndicator::ProgressUpdate(float alpha)
{
	wxGetApp().frame->gaugeBar->SetValue((int)::roundf(alpha * float(wxGetApp().frame->gaugeBar->GetRange())));

	if (!this->progressDialog)
	{
		clock_t currentTimeTicks = ::clock();
		clock_t elapsedTimeTicks = currentTimeTicks - this->startTimeTicks;
		double elapsedTimeSeconds = double(elapsedTimeTicks) / double(CLOCKS_PER_SEC);
		if (elapsedTimeSeconds > this->dialogTimeoutSeconds)
		{
			this->progressDialog = new wxProgressDialog("Chess AI", "Thinking...", 100, wxGetApp().frame, wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_CAN_ABORT);
			this->progressDialog->SetPosition(wxGetApp().frame->GetPosition());
		}
	}

	bool keepGoing = true;
	if (this->progressDialog)
		keepGoing = this->progressDialog->Update((int)::roundf(alpha * float(this->progressDialog->GetRange())));

	return keepGoing;
}

/*virtual*/ void ChessBotProgressIndicator::ProgressEnd()
{
	wxGetApp().frame->gaugeBar->SetValue(0);

	if (this->progressDialog)
	{
		delete this->progressDialog;
		this->progressDialog = nullptr;
	}
}

//---------------------------------------- ChessBotInterface ----------------------------------------

ChessBotInterface::ChessBotInterface()
{
	this->difficulty = Difficulty::MEDIUM;
}

/*virtual*/ ChessBotInterface::~ChessBotInterface()
{
}

/*virtual*/ void ChessBotInterface::SetDifficulty(Difficulty difficulty)
{
	this->difficulty = Difficulty::MEDIUM;
}

/*virtual*/ ChessBotInterface::Difficulty ChessBotInterface::GetDifficulty()
{
	return this->difficulty;
}

//---------------------------------------- ChessMinimaxBot ----------------------------------------

ChessMinimaxBot::ChessMinimaxBot() : ChessEngine::ChessMinimaxAI(1)
{
	this->progressIndicator = new ChessBotProgressIndicator();
}

/*virtual*/ ChessMinimaxBot::~ChessMinimaxBot()
{
	delete this->progressIndicator;
}

/*virtual*/ void ChessMinimaxBot::SetDifficulty(Difficulty difficulty)
{
	ChessBotInterface::SetDifficulty(difficulty);

	switch (difficulty)
	{
		case Difficulty::EASY:
		{
			this->maxDepth = 2;
			break;
		}
		case Difficulty::MEDIUM:
		{
			this->maxDepth = 3;
			break;
		}
		case Difficulty::HARD:
		{
			this->maxDepth = 4;
			break;
		}
	}
}

//---------------------------------------- ChessMCTSBot ----------------------------------------

ChessMCTSBot::ChessMCTSBot() : ChessEngine::ChessMonteCarloTreeSearchAI(0.0, 0)
{
	this->progressIndicator = new ChessBotProgressIndicator();
}

/*virtual*/ ChessMCTSBot::~ChessMCTSBot()
{
	delete this->progressIndicator;
}

/*virtual*/ void ChessMCTSBot::SetDifficulty(Difficulty difficulty)
{
	ChessBotInterface::SetDifficulty(difficulty);

	switch (difficulty)
	{
		case Difficulty::EASY:
		{
			//this->maxTimeSeconds = 10.0;
			this->maxIterations = 30;
			break;
		}
		case Difficulty::MEDIUM:
		{
			//this->maxTimeSeconds = 20.0;
			this->maxIterations = 40;
			break;
		}
		case Difficulty::HARD:
		{
			//this->maxTimeSeconds = 30.0;
			this->maxIterations = 50;
			break;
		}
	}
}