#pragma once

#include "ChessCommon.h"
#include "ChessObject.h"

namespace ChessEngine
{
	class ChessPiece;
	class ChessMove;
	class Castle;

	class CHESS_ENGINE_API ChessGame : public ChessObject
	{
	public:
		ChessGame();
		virtual ~ChessGame();

		void Reset();

		virtual bool WriteToStream(std::ostream& stream) const override;
		virtual bool ReadFromStream(std::istream& stream) override;

		ChessGame* Clone() const;

		virtual Code GetCode() const override;

		bool IsLocationValid(const ChessVector& location) const;

		ChessPiece* GetSquareOccupant(const ChessVector& location) const;
		void SetSquareOccupant(const ChessVector& location, ChessPiece* piece);
		
		bool PushMove(ChessMove* move);
		ChessMove* PopMove();

		// Assuming it is the given color's turn, generate all legal moves for that color.
		GameResult GenerateAllLegalMovesForColor(ChessColor color, ChessMoveArray& moveArray);

		const ChessMove* GetMove(int i) const;
		int GetNumMoves() const { return this->chessMoveStack->size(); }

		bool PieceEverMovedFromLocation(const ChessVector& location) const;

		int GetNumPiecesOnBoard() const;

		// Find all the ways the given color's pieces can move, barring the rules of check.
		void GatherAllMovesForColor(ChessColor color, ChessMoveArray& moveArray);

	protected:

		void Clear();

		bool IsColorInCheck(ChessColor color);
		bool KingMovesAcrossThreatenedSquare(const Castle* castle);

		ChessPiece* boardMatrix[CHESS_BOARD_FILES][CHESS_BOARD_RANKS];
		ChessMoveArray* chessMoveStack;
	};
}