#include "ChessCanvas.h"

int ChessCanvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

ChessCanvas::ChessCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &ChessCanvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &ChessCanvas::OnSize, this);
}

/*virtual*/ ChessCanvas::~ChessCanvas()
{
	delete this->renderContext;
}

void ChessCanvas::OnPaint(wxPaintEvent& event)
{
	this->SetCurrent(*this->renderContext);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//...

	glFlush();

	this->SwapBuffers();
}

void ChessCanvas::OnSize(wxSizeEvent& event)
{
	this->SetCurrent(*this->renderContext);

	wxSize size = event.GetSize();
	glViewport(0, 0, size.GetWidth(), size.GetHeight());

	this->Refresh();
}