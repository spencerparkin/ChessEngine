#pragma once

#include <wx/glcanvas.h>

class ChessCanvas : public wxGLCanvas
{
public:
	ChessCanvas(wxWindow* parent);
	virtual ~ChessCanvas();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);

private:

	void RenderBoard();

	wxGLContext* renderContext;
	static int attributeList[];
};