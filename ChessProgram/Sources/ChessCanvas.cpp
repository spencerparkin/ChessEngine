#include "ChessCanvas.h"
#include "ChessCommon.h"
#include <gl/GLU.h>

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

	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float aspectRatio = float(viewport[2]) / float(viewport[3]);

	float xMin = 0.0f;
	float xMax = 1.0f;
	float yMin = 0.0f;
	float yMax = 1.0f;

	if (aspectRatio > 1.0f)
	{
		float xDelta = (aspectRatio * (yMax - yMin) - (xMax - xMin)) / 2.0f;
		xMin -= xDelta;
		xMax += xDelta;
	}
	else
	{
		float yDelta = ((1.0f / aspectRatio) * (xMax - xMin) - (yMax - yMin)) / 2.0f;
		yMin -= yDelta;
		yMax += yDelta;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(xMin, xMax, yMin, yMax);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	this->RenderBoard();

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

void ChessCanvas::RenderBoard()
{
	for (int i = 0; i < CHESS_BOARD_FILES; i++)
	{
		float xMin = float(i) / float(CHESS_BOARD_FILES);
		float xMax = float(i + 1) / float(CHESS_BOARD_FILES);

		for (int j = 0; j < CHESS_BOARD_RANKS; j++)
		{
			float yMin = float(j) / float(CHESS_BOARD_FILES);
			float yMax = float(j + 1) / float(CHESS_BOARD_FILES);
			
			glBegin(GL_QUADS);

			if ((i + j) % 2 == 0)
				glColor3f(0.0f, 0.0f, 0.0f);
			else
				glColor3f(1.0f, 1.0f, 1.0f);

			glVertex2f(xMin, yMin);
			glVertex2f(xMax, yMin);
			glVertex2f(xMax, yMax);
			glVertex2f(xMin, yMax);

			glEnd();
		}
	}
}