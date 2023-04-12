#pragma once

#include "ChessCommon.h"
#include <wx/glcanvas.h>
#include <wx/hashmap.h>
#include <functional>

WX_DECLARE_STRING_HASH_MAP(GLuint, TextureMap);

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

	struct Box
	{
		float xMin, xMax;
		float yMin, yMax;

		void PointToUVs(float x, float y, float& u, float& v) const;
		void PointFromUVs(float& x, float& y, float u, float v) const;
		
		bool ContainsPoint(float x, float y) const;
	};

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
	bool CalculateSquareLocation(const wxPoint& mousePoint, ChessEngine::ChessVector& squareLocation, float* worldX = nullptr, float* worldY = nullptr);

	wxGLContext* renderContext;
	static int attributeList[];
	TextureMap textureMap;
	ChessEngine::ChessVector hoverLocation;
	ChessEngine::ChessVector selectedLocation;
	bool formulatingMove;
	float hoverWorldX;
	float hoverWorldY;
};