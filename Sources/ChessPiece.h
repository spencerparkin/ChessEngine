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

		// Note that overrides of this method should generate all possible moves for the chess
		// piece regardless of who's turn it is, check, check-mate, stale mate, or even the
		// idea of capturing the king which, of course, is illegal in chess.  Rules with regard
		// to these things are handled at a higher level of the software.
		virtual void GenerateAllPossibleMoves(ChessMoveArray& moveArray) const = 0;

		ChessGame* game;
		ChessVector location;
		ChessColor color;
	};

	class CHESS_ENGINE_API Pawn : public ChessPiece
	{
	public:
		Pawn(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Pawn();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& moveArray) const override;
	};

	class CHESS_ENGINE_API Knight : public ChessPiece
	{
	public:
		Knight(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Knight();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& moveArray) const override;
	};

	class CHESS_ENGINE_API Bishop : public ChessPiece
	{
	public:
		Bishop(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Bishop();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& moveArray) const override;
	};

	class CHESS_ENGINE_API Rook : public ChessPiece
	{
	public:
		Rook(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Rook();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& moveArray) const override;
	};

	class CHESS_ENGINE_API Queen : public ChessPiece
	{
	public:
		Queen(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~Queen();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& moveArray) const override;
	};

	class CHESS_ENGINE_API King : public ChessPiece
	{
	public:
		King(ChessGame* game, const ChessVector& location, ChessColor color);
		virtual ~King();

		virtual void GenerateAllPossibleMoves(ChessMoveArray& moveArray) const override;
	};
}