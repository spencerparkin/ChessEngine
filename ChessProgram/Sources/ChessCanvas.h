#pragma once

#include "ChessCommon.h"
#include "ChessPiece.h"
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

	void AnimateMove(const ChessEngine::ChessMove* move);
	void Animate(double deltaTimeSeconds);
	bool IsAnimating() { return this->animating; }
	bool GetDrawCoordinates() { return this->settings.drawCoordinates; }
	void SetDrawCoordinates(bool draw);
	bool GetDrawCaptures() { return this->settings.drawCaptures; }
	void SetDrawCaptures(bool draw);
	bool GetDrawVisibilityArrows() { return this->settings.drawVisibilityArrows; }
	void SetDrawVisibilityArrows(bool draw);

	enum class RenderOrientation
	{
		RENDER_NORMAL,
		RENDER_FLIPPED
	};

	RenderOrientation GetRenderOrientation() { return this->settings.renderOrientation; }
	void SetRenderOrientation(RenderOrientation renderOrientation);

	struct Settings
	{
		bool drawCoordinates;
		bool drawCaptures;
		bool drawVisibilityArrows;
		int lightSquareNum;
		int darkSquareNum;
		RenderOrientation renderOrientation;

		void Load();
		void Save();
	};

	enum class SquareShade
	{
		Light,
		Dark
	};

	void CycleSquareTexture(SquareShade shade);

private:

	void RenderBoard();
	void RenderBoardSquare(const ChessEngine::ChessVector& squareLocation, const Box& box);
	void RenderBoardSquarePiece(const ChessEngine::ChessVector& squareLocation, const Box& box);
	void RenderBoardSquareHighlight(const ChessEngine::ChessVector& squareLocation, const Box& box);
	void RenderBoardCoordinates(const ChessEngine::ChessVector& squareLocation, const Box& box);
	void RenderTexturedQuad(const Box& renderBox, GLuint texture);
	void RenderSolidColorQuad(const Box& renderBox, float red, float green, float blue);
	void RenderCaptures(const Box& marginBox, const std::vector<ChessEngine::ChessPiece*>& captureArray);
	void RenderArrows();

	void RecalculateVisibilityArrows();

	void ForEachBoardSquare(std::function<void(const ChessEngine::ChessVector&, const Box&)> renderFunc);

	GLuint GetTextureForChessPiece(const wxString& pieceName, ChessEngine::ChessColor color);
	GLuint GetTexture(const wxString& textureFile);

	void CalculateBoardSquareBox(const ChessEngine::ChessVector& squareLocation, Box& box);
	void CalculateWorldBox(Box& worldBox) const;
	bool CalculateMarginBoxes(Box& marginBoxA, Box& marginBoxB) const;
	bool CalculateSquareLocation(const wxPoint& mousePoint, ChessEngine::ChessVector& squareLocation, Vector* worldPoint = nullptr);
	bool CalculateSquareWorldCenter(const ChessEngine::ChessVector& squareLocation, Vector& worldCenter);

	bool FindLegalMoves(const ChessEngine::ChessVector& sourceLocation, const ChessEngine::ChessVector& destinationLocation, ChessEngine::ChessMoveArray& moveArray);

	struct Arrow
	{
		float r, g, b;
		Vector head, tail;
	};

	Settings settings;
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
	bool animating;
	std::vector<Arrow> arrowArray;
};