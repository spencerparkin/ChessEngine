#pragma once

#include <wx/glcanvas.h>

class ChessCanvas : public wxGLCanvas
{
public:
	ChessCanvas(wxWindow* parent);
	virtual ~ChessCanvas();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);

	wxGLContext* renderContext;
	static int attributeList[];
};