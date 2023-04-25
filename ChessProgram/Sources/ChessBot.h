#pragma once

#include "ChessAI.h"
#include <wx/progdlg.h>
#include <time.h>

class ChessBotProgressIndicator : public ChessEngine::ChessAIProgressIndicator
{
public:
	ChessBotProgressIndicator();
	virtual ~ChessBotProgressIndicator();

	virtual bool ProgressUpdate(float alpha) override;
	virtual void ProgressBegin() override;
	virtual void ProgressEnd() override;

	clock_t startTimeTicks;
	double dialogTimeoutSeconds;
	wxProgressDialog* progressDialog;
};

class ChessBotInterface
{
public:
	ChessBotInterface();
	virtual ~ChessBotInterface();

	enum class Difficulty
	{
		EASY,
		MEDIUM,
		HARD
	};

	virtual void SetDifficulty(Difficulty difficulty);
	virtual Difficulty GetDifficulty();

protected:
	Difficulty difficulty;
};

class ChessMinimaxBot : public ChessEngine::ChessMinimaxAI, public ChessBotInterface
{
public:
	ChessMinimaxBot();
	virtual ~ChessMinimaxBot();

	virtual void SetDifficulty(Difficulty difficulty) override;
};

class ChessMCTSBot : public ChessEngine::ChessMonteCarloTreeSearchAI, public ChessBotInterface
{
public:
	ChessMCTSBot();
	virtual ~ChessMCTSBot();

	virtual void SetDifficulty(Difficulty difficulty) override;
};