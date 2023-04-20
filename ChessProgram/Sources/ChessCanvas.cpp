#include "ChessCanvas.h"
#include "ChessCommon.h"
#include "ChessPiece.h"
#include "ChessMove.h"
#include "ChessApp.h"
#include "ChessFrame.h"
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/choicdlg.h>
#include <gl/GLU.h>

wxDEFINE_EVENT(EVT_GAME_STATE_CHANGED, wxCommandEvent);

int ChessCanvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

ChessCanvas::ChessCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->formulatingMove = false;
	this->animating = false;

	this->settings.drawCoordinates = false;
	this->settings.drawCaptures = false;
	this->settings.lightSquareNum = 0;
	this->settings.darkSquareNum = 0;
	this->settings.Load();

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

	this->offsetLocation.file = -1;
	this->offsetLocation.rank = -1;

	this->offsetVector.x = 0.0;
	this->offsetVector.y = 0.0;
}

/*virtual*/ ChessCanvas::~ChessCanvas()
{
	this->settings.Save();

	delete this->renderContext;

	ChessEngine::DeleteMoveArray(this->legalMoveArray);
}

void ChessCanvas::Settings::Load()
{
	this->drawCoordinates = wxGetApp().config.ReadBool("drawCoordinates", false);
	this->drawCaptures = wxGetApp().config.ReadBool("drawCaptures", false);
	this->lightSquareNum = (int)wxGetApp().config.ReadLong("lightSquareNum", 0);
	this->darkSquareNum = (int)wxGetApp().config.ReadLong("darkSquareNum", 0);
	this->renderOrientation = (RenderOrientation)wxGetApp().config.ReadLong("renderOrientation", (long)RenderOrientation::RENDER_NORMAL);
}

void ChessCanvas::Settings::Save()
{
	wxGetApp().config.Write("drawCoordinates", this->drawCoordinates);
	wxGetApp().config.Write("drawCaptures", this->drawCaptures);
	wxGetApp().config.Write("lightSquareNum", this->lightSquareNum);
	wxGetApp().config.Write("darkSquareNum", this->darkSquareNum);
	wxGetApp().config.Write("renderOrientation", (long)this->renderOrientation);
}

void ChessCanvas::SetDrawCoordinates(bool draw)
{
	if (draw != this->settings.drawCoordinates)
	{
		this->settings.drawCoordinates = draw;
		this->Refresh();
	}
}

void ChessCanvas::SetDrawCaptures(bool draw)
{
	if (draw != this->settings.drawCaptures)
	{
		this->settings.drawCaptures = draw;
		this->Refresh();
	}
}

void ChessCanvas::SetRenderOrientation(RenderOrientation renderOrientation)
{
	if (renderOrientation != this->settings.renderOrientation)
	{
		this->settings.renderOrientation = renderOrientation;
		this->Refresh();
	}
}

void ChessCanvas::AnimateMove(const ChessEngine::ChessMove* move)
{
	Vector worldSourcePoint, worldDestinationPoint;

	this->CalculateSquareWorldCenter(move->sourceLocation, worldSourcePoint);
	this->CalculateSquareWorldCenter(move->destinationLocation, worldDestinationPoint);

	this->offsetLocation = move->destinationLocation;
	this->offsetVector = worldSourcePoint - worldDestinationPoint;
	
	this->animating = true;
}

void ChessCanvas::Animate(double deltaTimeSeconds)
{
	if (this->animating)
	{
		double oldLength = this->offsetVector.Length();
		double moveRate = 0.5;
		double newLength = oldLength - moveRate * deltaTimeSeconds;
		if (newLength <= 0.0)
		{
			this->animating = false;
			this->offsetLocation.file = -1;
			this->offsetLocation.rank = -1;
		}
		else
		{
			this->offsetVector /= oldLength;
			this->offsetVector *= newLength;
		}

		this->Refresh();
	}
}

void ChessCanvas::OnPaint(wxPaintEvent& event)
{
	this->SetCurrent(*this->renderContext);

	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
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
	gluOrtho2D(worldBox.min.x, worldBox.max.x, worldBox.min.y, worldBox.max.y);

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
	Vector worldPoint;
	this->CalculateSquareLocation(event.GetPosition(), location, &worldPoint);

	if (location != this->hoverLocation || this->formulatingMove)
	{
		this->hoverLocation = location;

		if (this->formulatingMove)
			this->offsetVector = worldPoint - this->clickOrigin;

		this->Refresh();
	}
}

void ChessCanvas::OnLeftMouseButtonDown(wxMouseEvent& event)
{
	if (wxGetApp().GetCurrentPlayerType() == ChessApp::PlayerType::HUMAN)
	{
		ChessEngine::ChessVector location;
		Vector worldPoint;
		this->CalculateSquareLocation(event.GetPosition(), location, &worldPoint);

		ChessEngine::ChessPiece* piece = wxGetApp().game->GetSquareOccupant(location);
		if (piece && piece->color == wxGetApp().whoseTurn)
		{
			this->selectedLocation = location;
			this->formulatingMove = true;
			this->CaptureMouse();

			this->CalculateSquareWorldCenter(location, this->clickOrigin);
			this->offsetLocation = location;
			this->offsetVector = worldPoint - this->clickOrigin;
			this->animating = false;

			wxGetApp().game->GenerateAllLegalMovesForColor(wxGetApp().whoseTurn, this->legalMoveArray);

			this->Refresh();
		}
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
					wxArrayString choicesArray;
					for (ChessEngine::ChessMove* possibleMove : moveArray)
						choicesArray.Add(possibleMove->GetDescription().c_str());
					wxSingleChoiceDialog dialog(this, "Please choose one.", "Make a Decision", choicesArray);
					if (wxID_OK == dialog.ShowModal())
					{
						wxString chosen = dialog.GetStringSelection();
						for (ChessEngine::ChessMove* possibleMove : moveArray)
						{
							if (wxString(possibleMove->GetDescription().c_str()) == chosen)
							{
								move = possibleMove;
								break;
							}
						}
					}
				}

				if (move)
				{
					wxGetApp().game->PushMove(move);

					for (int i = 0; i < (signed)this->legalMoveArray.size(); i++)
						if (this->legalMoveArray[i] == move)
							this->legalMoveArray[i] = nullptr;

					wxGetApp().FlipTurn();

					wxCommandEvent stateChangedEvent(EVT_GAME_STATE_CHANGED);
					wxPostEvent(wxGetApp().frame, stateChangedEvent);
				}

				ChessEngine::DeleteMoveArray(this->legalMoveArray);
			}
		}

		this->selectedLocation.file = -1;
		this->selectedLocation.rank = -1;

		this->offsetLocation.file = -1;
		this->offsetLocation.rank = -1;

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
			moveArray.push_back(move);
	}

	return moveArray.size() > 0;
}

void ChessCanvas::CalculateWorldBox(Box& worldBox) const
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float aspectRatio = float(viewport[2]) / float(viewport[3]);

	worldBox.min = Vector(0.0, 0.0);
	worldBox.max = Vector(1.0, 1.0);

	worldBox.ScaleToMatchAspectRatio(aspectRatio);
}

bool ChessCanvas::CalculateMarginBoxes(Box& marginBoxA, Box& marginBoxB) const
{
	Box worldBox;
	this->CalculateWorldBox(worldBox);

	if (worldBox.min.x < 0.0)
	{
		marginBoxA.min = worldBox.min;
		marginBoxA.max.x = 0.0;
		marginBoxA.max.y = 1.0;
		marginBoxB.min.x = 1.0;
		marginBoxB.min.y = 0.0;
		marginBoxB.max.x = worldBox.max.x;
		marginBoxB.max.y = 1.0;
		return true;
	}
	else if(worldBox.min.y < 0.0)
	{
		marginBoxA.min = worldBox.min;
		marginBoxA.max.x = 1.0;
		marginBoxA.max.y = 0.0;
		marginBoxB.min.x = 0.0;
		marginBoxB.min.y = 1.0;
		marginBoxB.max.x = 1.0;
		marginBoxB.max.y = worldBox.max.y;
		return true;
	}

	return false;
}

bool ChessCanvas::CalculateSquareLocation(const wxPoint& mousePoint, ChessEngine::ChessVector& squareLocation, Vector* worldPoint /*= nullptr*/)
{
	Vector worldPointStorage;
	if (!worldPoint)
		worldPoint = &worldPointStorage;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	Box viewportBox;
	viewportBox.min.x = double(viewport[0]);
	viewportBox.max.x = double(viewport[2]);
	viewportBox.min.y = double(viewport[1]);
	viewportBox.max.y = double(viewport[3]);

	Box worldBox;
	this->CalculateWorldBox(worldBox);

	Vector viewportPoint(double(mousePoint.x), double(viewport[3] - mousePoint.y));

	Vector uvs;
	viewportBox.PointToUVs(viewportPoint, uvs);
	worldBox.PointFromUVs(*worldPoint, uvs);

	Box chessBox;
	chessBox.min.x = 0.0;
	chessBox.max.x = 1.0;
	chessBox.min.y = 0.0;
	chessBox.max.y = 1.0;

	if (!chessBox.ContainsPoint(*worldPoint))
	{
		squareLocation.file = -1;
		squareLocation.rank = -1;
		return false;
	}

	squareLocation.file = int(::floor(double(CHESS_BOARD_FILES) * worldPoint->x));
	squareLocation.rank = int(::floor(double(CHESS_BOARD_RANKS) * worldPoint->y));

	if (this->settings.renderOrientation == RenderOrientation::RENDER_FLIPPED)
	{
		squareLocation.file = CHESS_BOARD_RANKS - 1 - squareLocation.file;
		squareLocation.rank = CHESS_BOARD_RANKS - 1 - squareLocation.rank;
	}

	return true;
}

bool ChessCanvas::CalculateSquareWorldCenter(const ChessEngine::ChessVector& squareLocation, Vector& worldCenter)
{
	if (!wxGetApp().game->IsLocationValid(squareLocation))
		return false;

	double squareWidth = 1.0 / double(CHESS_BOARD_FILES);
	double squareHeight = 1.0 / double(CHESS_BOARD_RANKS);

	int i = (this->settings.renderOrientation == RenderOrientation::RENDER_FLIPPED) ? (CHESS_BOARD_FILES - 1 - squareLocation.file) : squareLocation.file;
	int j = (this->settings.renderOrientation == RenderOrientation::RENDER_FLIPPED) ? (CHESS_BOARD_RANKS - 1 - squareLocation.rank) : squareLocation.rank;

	worldCenter.x = double(i) * squareWidth + squareWidth / 2.0;
	worldCenter.y = double(j) * squareHeight + squareHeight / 2.0;

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

	if (this->settings.drawCoordinates)
	{
		this->ForEachBoardSquare([=](const ChessEngine::ChessVector& squareLocation, const Box& box) {
			this->RenderBoardCoordinates(squareLocation, box);
		});
	}

	if (this->settings.drawCaptures)
	{
		Box marginBoxA, marginBoxB;
		if (this->CalculateMarginBoxes(marginBoxA, marginBoxB))
		{
			std::vector<ChessEngine::ChessPiece*> captureArray[2];

			for (int i = 0; i < wxGetApp().game->GetNumMoves(); i++)
			{
				const ChessEngine::ChessMove* move = wxGetApp().game->GetMove(i);
				const ChessEngine::Capture* capture = dynamic_cast<const ChessEngine::Capture*>(move);
				if (capture)
					captureArray[int(capture->capturedPiece->color)].push_back(capture->capturedPiece);
				else
				{
					const ChessEngine::EnPassant* enPassant = dynamic_cast<const ChessEngine::EnPassant*>(move);
					if (enPassant)
						captureArray[int(enPassant->capturedPiece->color)].push_back(enPassant->capturedPiece);
				}
			}

			Box* whiteBox = nullptr;
			Box* blackBox = nullptr;

			switch (this->settings.renderOrientation)
			{
				case RenderOrientation::RENDER_NORMAL:
				{
					whiteBox = &marginBoxB;
					blackBox = &marginBoxA;
					break;
				}
				case RenderOrientation::RENDER_FLIPPED:
				{
					whiteBox = &marginBoxA;
					blackBox = &marginBoxB;
					break;
				}
			}

			this->RenderCaptures(*whiteBox, captureArray[int(ChessEngine::ChessColor::White)]);
			this->RenderCaptures(*blackBox, captureArray[int(ChessEngine::ChessColor::Black)]);
		}
	}
}

void ChessCanvas::RenderCaptures(const Box& marginBox, const std::vector<ChessEngine::ChessPiece*>& captureArray)
{
	double aspectRatio = marginBox.AspectRatio();
	double delta = 0.0;
	double divisor = (captureArray.size() == 1) ? 2.0 : double(captureArray.size());
	if (aspectRatio > 1.0)
		delta = marginBox.Height() / divisor;
	else
		delta = marginBox.Width() / divisor;

	for (int i = 0; i < (signed)captureArray.size(); i++)
	{
		const ChessEngine::ChessPiece* piece = captureArray[i];
		GLuint texture = this->GetTextureForChessPiece(piece->GetName(), piece->color);
		double alpha = (float(i + 1)) / float(captureArray.size() + 1);
		Box renderBox;
		Vector center;
		if (aspectRatio > 1.0)
			marginBox.PointFromUVs(center, Vector(alpha, 0.5));
		else
			marginBox.PointFromUVs(center, Vector(0.5, alpha));

		renderBox.min = center - Vector(delta, delta);
		renderBox.max = center + Vector(delta, delta);

		this->RenderTexturedQuad(renderBox, texture);
	}
}

void ChessCanvas::ForEachBoardSquare(std::function<void(const ChessEngine::ChessVector&, const Box&)> renderFunc)
{
	Box box;

	for (int i = 0; i < CHESS_BOARD_FILES; i++)
	{
		box.min.x = float(i) / float(CHESS_BOARD_FILES);
		box.max.x = float(i + 1) / float(CHESS_BOARD_FILES);

		for (int j = 0; j < CHESS_BOARD_RANKS; j++)
		{
			box.min.y = float(j) / float(CHESS_BOARD_FILES);
			box.max.y = float(j + 1) / float(CHESS_BOARD_FILES);

			ChessEngine::ChessVector squareLocation;

			switch (this->settings.renderOrientation)
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
	int* squareNum = nullptr;
	float color = 0.0f;
	if ((squareLocation.file + squareLocation.rank) % 2 == 0)
	{
		squareNum = &this->settings.darkSquareNum;
		color = 0.7f;
	}
	else
	{
		squareNum = &this->settings.lightSquareNum;
		color = 1.0f;
	}

	if (*squareNum == 0)
		this->RenderSolidColorQuad(box, color, color, color);
	else
	{
		wxString postfix = wxString::Format("square_%d", *squareNum - 1);
		wxString prefix = (squareNum == &this->settings.darkSquareNum) ? wxString("dark") : wxString("light");
		wxFileName textureFile(wxGetCwd() + "/Textures/" + prefix + "_" + postfix + ".png");
		GLuint texture = this->GetTexture(textureFile.GetFullPath());
		if (texture != GL_INVALID_VALUE)
			this->RenderTexturedQuad(box, texture);
	}
}

void ChessCanvas::CycleSquareTexture(SquareShade shade)
{
	switch (shade)
	{
		case SquareShade::Light:
		{
			// There are 4 shades, including the flat color.
			this->settings.lightSquareNum = (this->settings.lightSquareNum + 1) % 4;
			break;
		}
		case SquareShade::Dark:
		{
			// There are 5 shades, including the flat color.
			this->settings.darkSquareNum = (this->settings.darkSquareNum + 1) % 5;
			break;
		}
	}
}

void ChessCanvas::RenderSolidColorQuad(const Box& renderBox, float red, float green, float blue)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glColor4f(red, green, blue, 1.0f);

	glVertex2f(renderBox.min.x, renderBox.min.y);
	glVertex2f(renderBox.max.x, renderBox.min.y);
	glVertex2f(renderBox.max.x, renderBox.max.y);
	glVertex2f(renderBox.min.x, renderBox.max.y);

	glEnd();
}

void ChessCanvas::RenderTexturedQuad(const Box& renderBox, GLuint texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);

	// I spent hours pulling my hair out wondering why the blending wasn't working until
	// finally putting in the following line of code.  WTF?!  This fixes it?!
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(renderBox.min.x, renderBox.min.y);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(renderBox.max.x, renderBox.min.y);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(renderBox.max.x, renderBox.max.y);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(renderBox.min.x, renderBox.max.y);

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
				Box renderBox(box);

				if (squareLocation == this->offsetLocation)
					renderBox += this->offsetVector;

				this->RenderTexturedQuad(renderBox, texture);
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

	glVertex2f(box.min.x, box.min.y);
	glVertex2f(box.max.x, box.min.y);
	glVertex2f(box.max.x, box.max.y);
	glVertex2f(box.min.x, box.max.y);

	glEnd();
}

void ChessCanvas::RenderBoardCoordinates(const ChessEngine::ChessVector& squareLocation, const Box& box)
{
	if (squareLocation.file == 0)
	{
		wxString rank = wxString::Format("%d", squareLocation.rank + 1);
		wxFileName textureFile(wxGetCwd() + "/Textures/" + rank + ".png");
		GLuint texture = this->GetTexture(textureFile.GetFullPath());
		if (texture != GL_INVALID_VALUE)
		{
			Box renderBox;

			switch (this->settings.renderOrientation)
			{
				case RenderOrientation::RENDER_NORMAL:
				{
					box.PointFromUVs(renderBox.min, Vector(0.0, 3.0 / 4.0));
					box.PointFromUVs(renderBox.max, Vector(1.0 / 4.0, 1.0));
					break;
				}
				case RenderOrientation::RENDER_FLIPPED:
				{
					box.PointFromUVs(renderBox.min, Vector(3.0 / 4.0, 0.0));
					box.PointFromUVs(renderBox.max, Vector(1.0, 1.0 / 4.0));
					break;
				}
			}

			this->RenderTexturedQuad(renderBox, texture);
		}
	}

	if (squareLocation.rank == 0)
	{
		char fileArray[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
		wxString file = fileArray[squareLocation.file];
		wxFileName textureFile(wxGetCwd() + "/Textures/" + file + ".png");
		GLuint texture = this->GetTexture(textureFile.GetFullPath());
		if (texture != GL_INVALID_VALUE)
		{
			Box renderBox;

			switch (this->settings.renderOrientation)
			{
				case RenderOrientation::RENDER_NORMAL:
				{
					box.PointFromUVs(renderBox.min, Vector(3.0 / 4.0, 0.0));
					box.PointFromUVs(renderBox.max, Vector(1.0, 1.0 / 4.0));
					break;
				}
				case RenderOrientation::RENDER_FLIPPED:
				{
					box.PointFromUVs(renderBox.min, Vector(0.0, 3.0 / 4.0));
					box.PointFromUVs(renderBox.max, Vector(1.0 / 4.0, 1.0));
					break;
				}
			}

			this->RenderTexturedQuad(renderBox, texture);
		}
	}
}

GLuint ChessCanvas::GetTextureForChessPiece(const wxString& pieceName, ChessEngine::ChessColor color)
{
	wxString prefix = (color == ChessEngine::ChessColor::White) ? "white" : "black";
	wxString postfix = pieceName.Lower();
	wxFileName textureFile(wxGetCwd() + "/Textures/" + prefix + "_" + postfix + ".png");
	return this->GetTexture(textureFile.GetFullPath());
}

GLuint ChessCanvas::GetTexture(const wxString& textureFile)
{
	GLuint texture = GL_INVALID_VALUE;

	TextureMap::iterator iter = this->textureMap.find(textureFile);
	if (iter != this->textureMap.end())
		texture = iter->second;
	else
	{
		wxImage image;
		if (image.LoadFile(textureFile))
		{
			glGenTextures(1, &texture);
			if (texture != GL_INVALID_VALUE)
			{
				this->textureMap[textureFile] = texture;

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

				GLuint imageWidth = image.GetWidth();
				GLuint imageHeight = image.GetHeight();
				GLubyte* imageBuffer = image.GetData();
				GLubyte* alphaBuffer = image.HasAlpha() ? image.GetAlpha() : nullptr;
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
						GLubyte* texel = &textureBuffer[i * imageWidth * bytesPerTexel + j * bytesPerTexel];

						texel[0] = pixel[0];
						texel[1] = pixel[1];
						texel[2] = pixel[2];

						if (alphaBuffer)
						{
							GLubyte* alpha = &alphaBuffer[(imageHeight - 1 - i) * imageWidth * bytesPerAlpha + j * bytesPerAlpha];
							texel[3] = alpha[0];
						}
						else
						{
							texel[3] = 255;
						}
					}
				}

				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);

				gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);

				delete[] textureBuffer;
			}
		}
	}

	return texture;
}