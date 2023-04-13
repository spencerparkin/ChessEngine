#include "ChessCanvas.h"
#include "ChessCommon.h"
#include "ChessPiece.h"
#include "ChessMove.h"
#include "ChessApp.h"
#include "ChessFrame.h"
#include <wx/filename.h>
#include <wx/image.h>
#include <gl/GLU.h>

wxDEFINE_EVENT(EVT_GAME_STATE_CHANGED, wxCommandEvent);

int ChessCanvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

ChessCanvas::ChessCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->formulatingMove = false;

	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &ChessCanvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &ChessCanvas::OnSize, this);
	this->Bind(wxEVT_MOTION, &ChessCanvas::OnMouseMotion, this);
	this->Bind(wxEVT_LEFT_DOWN, &ChessCanvas::OnLeftMouseButtonDown, this);
	this->Bind(wxEVT_LEFT_UP, &ChessCanvas::OnLeftMouseButtonUp, this);
	this->Bind(wxEVT_MOUSE_CAPTURE_LOST, &ChessCanvas::OnCaptureLost, this);

	this->hoverLocation.file = -1;
	this->hoverLocation.rank = -1;

	this->selectedLocation.file = -1;
	this->selectedLocation.rank = -1;

	this->hoverWorldX = 0.0f;
	this->hoverWorldY = 0.0f;
}

/*virtual*/ ChessCanvas::~ChessCanvas()
{
	delete this->renderContext;

	ChessEngine::DeleteMoveArray(this->legalMoveArray);
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
	ChessEngine::ChessVector location;
	this->CalculateSquareLocation(event.GetPosition(), location, &this->hoverWorldX, &this->hoverWorldY);

	if (location != this->hoverLocation || this->formulatingMove)
	{
		this->hoverLocation = location;
		this->Refresh();
	}
}

void ChessCanvas::OnLeftMouseButtonDown(wxMouseEvent& event)
{
	ChessEngine::ChessVector location;
	this->CalculateSquareLocation(event.GetPosition(), location);

	ChessEngine::ChessPiece* piece = wxGetApp().game->GetSquareOccupant(location);
	if (piece && piece->color == wxGetApp().whoseTurn)
	{
		this->selectedLocation = location;
		this->formulatingMove = true;
		this->CaptureMouse();

		wxGetApp().game->GenerateAllLegalMovesForColor(wxGetApp().whoseTurn, this->legalMoveArray);

		this->Refresh();
	}
}

void ChessCanvas::OnLeftMouseButtonUp(wxMouseEvent& event)
{
	if (this->formulatingMove)
	{
		this->formulatingMove = false;
		this->ReleaseMouse();

		ChessEngine::ChessVector targetLocation;
		this->CalculateSquareLocation(event.GetPosition(), targetLocation);

		if (wxGetApp().game->IsLocationValid(targetLocation))
		{
			ChessEngine::ChessMoveArray moveArray;
			if (this->FindLegalMoves(this->selectedLocation, targetLocation, moveArray))
			{
				ChessEngine::ChessMove* move = nullptr;
				if (moveArray.size() == 1)
					move = moveArray[0];
				else
				{
					// TODO: Let user pick which move they want to do.  Pawn promotions are the only case I'm aware of where this would happen.
				}

				if (move)
				{
					wxGetApp().game->PushMove(move);

					for (int i = 0; i < (signed)this->legalMoveArray.size(); i++)
						if (this->legalMoveArray[i] == move)
							this->legalMoveArray[i] = nullptr;

					if (wxGetApp().whoseTurn == ChessEngine::ChessColor::White)
						wxGetApp().whoseTurn = ChessEngine::ChessColor::Black;
					else
						wxGetApp().whoseTurn = ChessEngine::ChessColor::White;

					wxCommandEvent event(EVT_GAME_STATE_CHANGED);
					wxPostEvent(wxGetApp().frame, event);
				}

				ChessEngine::DeleteMoveArray(this->legalMoveArray);
			}
		}

		this->selectedLocation.file = -1;
		this->selectedLocation.rank = -1;

		this->Refresh();
	}
}

void ChessCanvas::OnCaptureLost(wxMouseCaptureLostEvent& event)
{
	if (this->formulatingMove)
	{
		this->formulatingMove = false;
		this->selectedLocation.file = -1;
		this->selectedLocation.rank = -1;
		ChessEngine::DeleteMoveArray(this->legalMoveArray);
		this->Refresh();
	}
}

bool ChessCanvas::FindLegalMoves(const ChessEngine::ChessVector& sourceLocation, const ChessEngine::ChessVector& destinationLocation, ChessEngine::ChessMoveArray& moveArray)
{
	for (int i = 0; i < (signed)this->legalMoveArray.size(); i++)
	{
		ChessEngine::ChessMove* move = this->legalMoveArray[i];
		if (move->sourceLocation == sourceLocation && move->destinationLocation == destinationLocation)
		{
			moveArray.push_back(move);
			return true;
		}
	}

	return false;
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

bool ChessCanvas::CalculateSquareLocation(const wxPoint& mousePoint, ChessEngine::ChessVector& squareLocation, float* worldX /*= nullptr*/, float* worldY /*= nullptr*/)
{
	float worldXStorage, worldYStorage;
	
	if (!worldX)
		worldX = &worldXStorage;

	if (!worldY)
		worldY = &worldYStorage;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	Box viewportBox;
	viewportBox.xMin = float(viewport[0]);
	viewportBox.xMax = float(viewport[2]);
	viewportBox.yMin = float(viewport[1]);
	viewportBox.yMax = float(viewport[3]);

	Box worldBox;
	this->CalculateWorldBox(worldBox);

	float viewportX = float(mousePoint.x);
	float viewportY = float(viewport[3] - mousePoint.y);

	float u, v;

	viewportBox.PointToUVs(viewportX, viewportY, u, v);
	worldBox.PointFromUVs(*worldX, *worldY, u, v);

	Box chessBox;
	chessBox.xMin = 0.0f;
	chessBox.xMax = 1.0f;
	chessBox.yMin = 0.0f;
	chessBox.yMax = 1.0f;

	if (!chessBox.ContainsPoint(*worldX, *worldY))
	{
		squareLocation.file = -1;
		squareLocation.rank = -1;
		return false;
	}

	squareLocation.file = int(::floor(float(CHESS_BOARD_FILES) * *worldX));
	squareLocation.rank = int(::floor(float(CHESS_BOARD_RANKS) * *worldY));

	if (this->renderOrientation == RenderOrientation::RENDER_FLIPPED)
	{
		squareLocation.file = CHESS_BOARD_RANKS - 1 - squareLocation.file;
		squareLocation.rank = CHESS_BOARD_RANKS - 1 - squareLocation.rank;
	}

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
					squareLocation = ChessEngine::ChessVector(CHESS_BOARD_FILES - 1 - i, CHESS_BOARD_RANKS - 1 - j);
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
		glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
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
				const Box* renderBox = &box;

				Box floatingBox;

				if (this->formulatingMove && squareLocation == this->selectedLocation)
				{
					float width = 1.0f / float(CHESS_BOARD_FILES);
					float height = 1.0f / float(CHESS_BOARD_RANKS);

					floatingBox.xMin = this->hoverWorldX - width / 2.0f;
					floatingBox.xMax = this->hoverWorldX + width / 2.0f;
					floatingBox.yMin = this->hoverWorldY - height / 2.0f;
					floatingBox.yMax = this->hoverWorldY + height / 2.0f;

					renderBox = &floatingBox;
				}

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture);
				glBegin(GL_QUADS);

				// I spent hours pulling my hair out wondering why the blending wasn't working until
				// finally putting in the following line of code.  WTF?!  This fixes it?!
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(renderBox->xMin, renderBox->yMin);

				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(renderBox->xMax, renderBox->yMin);

				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(renderBox->xMax, renderBox->yMax);

				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(renderBox->xMin, renderBox->yMax);

				glEnd();
			}
		}
	}
}

void ChessCanvas::RenderBoardSquareHighlight(const ChessEngine::ChessVector& squareLocation, const Box& box)
{
	if (squareLocation == this->hoverLocation)
	{
		if (this->formulatingMove)
		{
			ChessEngine::ChessMoveArray moveArray;
			this->FindLegalMoves(this->selectedLocation, squareLocation, moveArray);

			if (moveArray.size() > 0)
				glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			else
				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		}
	}
	else if (squareLocation == this->selectedLocation)
	{
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else
	{
		return;
	}

	glDisable(GL_TEXTURE_2D);
	glLineWidth(5.0f);

	glBegin(GL_LINE_LOOP);	

	glVertex2f(box.xMin, box.yMin);
	glVertex2f(box.xMax, box.yMin);
	glVertex2f(box.xMax, box.yMax);
	glVertex2f(box.xMin, box.yMax);

	glEnd();
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