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

		bool GetSquareOccupant(const ChessVector& location, ChessPiece*& piece) const;
		bool SetSquareOccupant(const ChessVector& location, ChessPiece* piece);
		
		bool PushMove(ChessMove* move);
		bool PopMove();

	protected:

		void Clear();

		ChessPiece* boardMatrix[CHESS_BOARD_FILES][CHESS_BOARD_RANKS];
		ChessMoveArray* chessMoveStack;
	};
}