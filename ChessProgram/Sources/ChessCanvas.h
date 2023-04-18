#pragma once

#include "ChessCommon.h"
#include "Math.h"
#include <wx/glcanvas.h>
#include <wx/hashmap.h>
#include <wx/timer.h>
#include <functional>

WX_DECLARE_STRING_HASH_MAP(GLuint, TextureMap);

wxDECLARE_EVENT(EVT_GAME_STATE_CHANGED, wxCommandEvent);

class ChessCanvas : public wxGLCanvas
{
public:
	ChessCanvas(wxWindow* parent);
	virtual ~ChessCanvas();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnLeftMouseButtonDown(wxMouseEvent& event);
	void OnLeftMouseButtonUp(wxMouseEvent& event);
	void OnCaptureLost(wxMouseCaptureLostEvent& event);
	void OnTimerTick(wxTimerEvent& event);

	void AnimateMove(const ChessEngine::ChessMove* move);

	enum class RenderOrientation
	{
		RENDER_NORMAL,
		RENDER_FLIPPED
	};

	RenderOrientation renderOrientation;

private:

	void RenderBoard();
	void RenderBoardSquare(const ChessEngine::ChessVector& squareLocation, const Box& box);
	void RenderBoardSquarePiece(const ChessEngine::ChessVector& squareLocation, const Box& box);
	void RenderBoardSquareHighlight(const ChessEngine::ChessVector& squareLocation, const Box& box);

	void ForEachBoardSquare(std::function<void(const ChessEngine::ChessVector&, const Box&)> renderFunc);

	GLuint GetTextureForChessPiece(const wxString& pieceName, ChessEngine::ChessColor color);

	void CalculateWorldBox(Box& worldBox) const;
	bool CalculateSquareLocation(const wxPoint& mousePoint, ChessEngine::ChessVector& squareLocation, Vector* worldPoint = nullptr);
	bool CalculateSquareWorldCenter(const ChessEngine::ChessVector& squareLocation, Vector& worldCenter);

	bool FindLegalMoves(const ChessEngine::ChessVector& sourceLocation, const ChessEngine::ChessVector& destinationLocation, ChessEngine::ChessMoveArray& moveArray);

	wxGLContext* renderContext;
	static int attributeList[];
	TextureMap textureMap;
	ChessEngine::ChessVector hoverLocation;
	ChessEngine::ChessVector selectedLocation;
	ChessEngine::ChessMoveArray legalMoveArray;
	bool formulatingMove;
	ChessEngine::ChessVector offsetLocation;
	Vector offsetVector;
	Vector clickOrigin;
	wxTimer timer;
	bool animating;
};