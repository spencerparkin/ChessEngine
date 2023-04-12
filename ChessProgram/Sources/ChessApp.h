#pragma once

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

	ChessFrame* frame;
};

wxDECLARE_APP(ChessApp);