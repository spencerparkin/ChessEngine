#include "ChessCanvas.h"
#include "ChessCommon.h"
#include "ChessPiece.h"
#include "ChessApp.h"
#include <wx/filename.h>
#include <wx/image.h>
#include <gl/GLU.h>

int ChessCanvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

ChessCanvas::ChessCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &ChessCanvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &ChessCanvas::OnSize, this);
	this->Bind(wxEVT_MOTION, &ChessCanvas::OnMouseMotion, this);
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

	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Box worldBox;
	this->CalculateWorldBox(worldBox);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(worldBox.xMin, worldBox.xMax, worldBox.yMin, worldBox.yMax);

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

void ChessCanvas::OnMouseMotion(wxMouseEvent& event)
{
	ChessEngine::ChessVector squareLocation;
	
	this->CalculateSquareLocation(event.GetPosition(), squareLocation);
}

void ChessCanvas::CalculateWorldBox(Box& worldBox) const
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float aspectRatio = float(viewport[2]) / float(viewport[3]);

	worldBox.xMin = 0.0f;
	worldBox.xMax = 1.0f;
	worldBox.yMin = 0.0f;
	worldBox.yMax = 1.0f;

	if (aspectRatio > 1.0f)
	{
		float xDelta = (aspectRatio * (worldBox.yMax - worldBox.yMin) - (worldBox.xMax - worldBox.xMin)) / 2.0f;
		worldBox.xMin -= xDelta;
		worldBox.xMax += xDelta;
	}
	else
	{
		float yDelta = ((1.0f / aspectRatio) * (worldBox.xMax - worldBox.xMin) - (worldBox.yMax - worldBox.yMin)) / 2.0f;
		worldBox.yMin -= yDelta;
		worldBox.yMax += yDelta;
	}
}

bool ChessCanvas::CalculateSquareLocation(const wxPoint& mousePoint, ChessEngine::ChessVector& squareLocation)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	Box viewportBox;
	viewportBox.xMin = float(viewport[0]);
	viewportBox.xMax = float(viewport[2]);
	viewportBox.yMin = float(viewport[1]);
	viewportBox.yMax = float(viewport[3]);

	Box worldBox;
	this->CalculateWorldBox(worldBox);

	float x = float(mousePoint.x);
	float y = float(mousePoint.y);

	float u, v;
	viewportBox.PointToUVs(x, y, u, v);

	worldBox.PointToUVs(x, y, u, v);

	Box chessBox;
	chessBox.xMin = 0.0f;
	chessBox.xMax = 1.0f;
	chessBox.yMin = 0.0f;
	chessBox.yMax = 1.0f;

	if (!chessBox.ContainsPoint(x, y))
		return false;

	squareLocation.file = int(::floor(float(CHESS_BOARD_FILES) * x));
	squareLocation.rank = int(::floor(float(CHESS_BOARD_RANKS) * y));

	if (this->renderOrientation == RenderOrientation::RENDER_FLIPPED)
		squareLocation.rank = CHESS_BOARD_RANKS - 1 - squareLocation.rank;

	return true;
}

void ChessCanvas::RenderBoard()
{
	this->ForEachBoardSquare([=](const ChessEngine::ChessVector& squareLocation, const Box& box) {
		this->RenderBoardSquare(squareLocation, box);
	});

	this->ForEachBoardSquare([=](const ChessEngine::ChessVector& squareLocation, const Box& box) {
		this->RenderBoardSquarePiece(squareLocation, box);
	});

	this->ForEachBoardSquare([=](const ChessEngine::ChessVector& squareLocation, const Box& box) {
		this->RenderBoardSquareHighlight(squareLocation, box);
	});
}

void ChessCanvas::ForEachBoardSquare(std::function<void(const ChessEngine::ChessVector&, const Box&)> renderFunc)
{
	Box box;

	for (int i = 0; i < CHESS_BOARD_FILES; i++)
	{
		box.xMin = float(i) / float(CHESS_BOARD_FILES);
		box.xMax = float(i + 1) / float(CHESS_BOARD_FILES);

		for (int j = 0; j < CHESS_BOARD_RANKS; j++)
		{
			box.yMin = float(j) / float(CHESS_BOARD_FILES);
			box.yMax = float(j + 1) / float(CHESS_BOARD_FILES);

			ChessEngine::ChessVector squareLocation;

			switch (this->renderOrientation)
			{
				case RenderOrientation::RENDER_NORMAL:
				{
					squareLocation = ChessEngine::ChessVector(i, j);
					break;
				}
				case RenderOrientation::RENDER_FLIPPED:
				{
					squareLocation = ChessEngine::ChessVector(i, CHESS_BOARD_RANKS - 1 - j);
					break;
				}
			}

			renderFunc(squareLocation, box);
		}
	}
}

void ChessCanvas::RenderBoardSquare(const ChessEngine::ChessVector& squareLocation, const Box& box)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	if ((squareLocation.file + squareLocation.rank) % 2 == 0)
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	else
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glVertex2f(box.xMin, box.yMin);
	glVertex2f(box.xMax, box.yMin);
	glVertex2f(box.xMax, box.yMax);
	glVertex2f(box.xMin, box.yMax);

	glEnd();
}

void ChessCanvas::RenderBoardSquarePiece(const ChessEngine::ChessVector& squareLocation, const Box& box)
{
	ChessEngine::ChessGame* game = wxGetApp().game;
	if (game)
	{
		ChessEngine::ChessPiece* piece = game->GetSquareOccupant(squareLocation);
		if (piece)
		{
			int texture = this->GetTextureForChessPiece(piece->GetName().c_str(), piece->color);
			if (texture != GL_INVALID_VALUE)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture);
				glBegin(GL_QUADS);

				// I spent hours pulling my hair out wondering why the blending wasn't working until
				// finally putting in the following line of code.  WTF?!  This fixes it?!
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(box.xMin, box.yMin);

				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(box.xMax, box.yMin);

				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(box.xMax, box.yMax);

				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(box.xMin, box.yMax);

				glEnd();
			}
		}
	}
}

void ChessCanvas::RenderBoardSquareHighlight(const ChessEngine::ChessVector& squareLocation, const Box& box)
{
	//...
}

GLuint ChessCanvas::GetTextureForChessPiece(const wxString& pieceName, ChessEngine::ChessColor color)
{
	GLuint texture = GL_INVALID_VALUE;

	wxString prefix = (color == ChessEngine::ChessColor::White) ? "white" : "black";
	wxString postfix = pieceName.Lower();
	wxString textureKey = prefix + "_" + postfix;

	TextureMap::iterator iter = this->textureMap.find(textureKey);
	if (iter != this->textureMap.end())
		texture = iter->second;
	else
	{
		wxFileName textureFile(wxGetCwd() + "/Textures/" + textureKey + ".png");

		wxImage image;
		if (image.LoadFile(textureFile.GetFullPath()) && image.HasAlpha())
		{
			glGenTextures(1, &texture);
			if (texture != GL_INVALID_VALUE)
			{
				this->textureMap[textureKey] = texture;

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

				GLuint imageWidth = image.GetWidth();
				GLuint imageHeight = image.GetHeight();
				GLubyte* imageBuffer = image.GetData();
				GLubyte* alphaBuffer = image.GetAlpha();
				GLuint bytesPerPixel = 3;
				GLuint bytesPerAlpha = 1;
				GLuint bytesPerTexel = 4;
				GLubyte* textureBuffer = new GLubyte[imageWidth * imageHeight * bytesPerTexel];

				// We have to flip the image for OpenGL.
				for (GLuint i = 0; i < imageHeight; i++)
				{
					for (GLuint j = 0; j < imageWidth; j++)
					{
						GLubyte* pixel = &imageBuffer[(imageHeight - 1 - i) * imageWidth * bytesPerPixel + j * bytesPerPixel];
						GLubyte* alpha = &alphaBuffer[(imageHeight - 1 - i) * imageWidth * bytesPerAlpha + j * bytesPerAlpha];
						GLubyte* texel = &textureBuffer[i * imageWidth * bytesPerTexel + j * bytesPerTexel];

						texel[0] = pixel[0];
						texel[1] = pixel[1];
						texel[2] = pixel[2];
						texel[3] = alpha[0];
					}
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
				delete[] textureBuffer;
			}
		}
	}

	return texture;
}

void ChessCanvas::Box::PointToUVs(float x, float y, float& u, float& v) const
{
	u = (x - this->xMin) / (this->xMax - this->xMin);
	v = (y - this->yMin) / (this->yMax - this->yMin);
}

void ChessCanvas::Box::PointFromUVs(float& x, float& y, float u, float v) const
{
	x = this->xMin + u * (this->xMax - this->xMin);
	y = this->yMin + v * (this->yMax - this->yMin);
}

bool ChessCanvas::Box::ContainsPoint(float x, float y) const
{
	if (!(this->xMin <= x && x <= this->xMax))
		return false;

	if (!(this->yMin <= y && y <= this->yMax))
		return false;

	return true;
}