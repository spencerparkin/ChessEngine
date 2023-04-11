#pragma once

#include "ChessCommon.h"

namespace ChessEngine
{
	class ChessGame;

	class CHESS_ENGINE_API ChessPiece
	{
	public:
		ChessPiece(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~ChessPiece();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const = 0;

		ChessGame* game;
		ChessVector location;
		ChessColor color;
	};

	class CHESS_ENGINE_API Pawn : public ChessPiece
	{
	public:
		Pawn(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Pawn();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const override;

		// TODO: Add info here indicating whether this pawn can be captured using en-passant?  This rule is tricky, because there is only a limited time when the move is available.
		//       Similarly for castling, we have to know if a piece has ever moved.
	};

	class CHESS_ENGINE_API Knight : public ChessPiece
	{
	public:
		Knight(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Knight();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const override;
	};

	class CHESS_ENGINE_API Bishop : public ChessPiece
	{
	public:
		Bishop(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Bishop();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const override;
	};

	class CHESS_ENGINE_API Rook : public ChessPiece
	{
	public:
		Rook(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Rook();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const override;
	};

	class CHESS_ENGINE_API Queen : public ChessPiece
	{
	public:
		Queen(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Queen();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const override;
	};

	class CHESS_ENGINE_API King : public ChessPiece
	{
	public:
		King(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~King();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const override;
	};
}