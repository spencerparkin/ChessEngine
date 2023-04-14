#pragma once

#include "ChessAI.h"
#include <wx/progdlg.h>

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