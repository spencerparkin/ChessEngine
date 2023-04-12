#pragma once

#include <wx/setup.h>
#include <wx/app.h>

class ChessApp : public wxApp
{
public:
	ChessApp();
	virtual ~ChessApp();


};

wxDECLARE_APP(ChessApp);