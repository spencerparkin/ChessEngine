#pragma once

#include "ChessAI.h"
#include <wx/progdlg.h>

#define COMPUTER_EASY_MAX_DEPTH		2
#define COMPUTER_MEDIUM_MAX_DEPTH	3
#define COMPUTER_HARD_MAX_DEPTH		4

class ChessBot : public ChessEngine::ChessMinimaxAI
{
public:
	ChessBot(int maxDepth);
	virtual ~ChessBot();

	virtual void ProgressBegin() override;
	virtual void ProgressEnd() override;

	virtual bool ProgressUpdate(float percentage) override;

	wxProgressDialog* progressDialog;
};