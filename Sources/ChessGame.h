#pragma once

#include "ChessCommon.h"

namespace ChessEngine
{
	class ChessPiece;
	class ChessMove;

	class CHESS_ENGINE_API ChessGame
	{
	public:
		ChessGame();
		virtual ~ChessGame();

		void Reset();

		bool IsLocationValid(const ChessVector& location) const;

		ChessPiece* GetSquareOccupant(const ChessVector& location) const;
		void SetSquareOccupant(const ChessVector& location, ChessPiece* piece);
		
		bool PushMove(ChessMove* move);
		bool PopMove();

		// Assuming it is the given color's turn, generate all legal moves for that color.
		GameResult GenerateAllLegalMovesForColor(ChessColor color, ChessMoveArray& moveArray);

	protected:

		void Clear();

		bool IsColorInCheck(ChessColor color);
		void GatherAllMovesForColor(ChessColor color, ChessMoveArray& moveArray);

		ChessPiece* boardMatrix[CHESS_BOARD_FILES][CHESS_BOARD_RANKS];
		ChessMoveArray* chessMoveStack;
	};
}