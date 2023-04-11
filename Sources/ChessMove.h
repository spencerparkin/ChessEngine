#pragma once

#include "ChessCommon.h"

namespace ChessEngine
{
	class ChessGame;
	class ChessPiece;

	class CHESS_ENGINE_API ChessMove
	{
	public:
		ChessMove();
		virtual ~ChessMove();

		virtual bool Do(ChessGame* chessGame) = 0;
		virtual bool Undo(ChessGame* chessGame) = 0;

		ChessVector sourceLocation;
		ChessVector destinationLocation;
	};

	class CHESS_ENGINE_API Travel : public ChessMove
	{
	public:
		Travel();
		virtual ~Travel();

		virtual bool Do(ChessGame* chessGame) override;
		virtual bool Undo(ChessGame* chessGame) override;
	};

	class CHESS_ENGINE_API Capture : public ChessMove
	{
	public:
		Capture();
		virtual ~Capture();
		
		virtual bool Do(ChessGame* chessGame) override;
		virtual bool Undo(ChessGame* chessGame) override;

		ChessPiece* capturedPiece;
	};

	class CHESS_ENGINE_API Castle : public ChessMove
	{
	public:
		Castle();
		virtual ~Castle();

		virtual bool Do(ChessGame* chessGame) override;
		virtual bool Undo(ChessGame* chessGame) override;
	};

	class CHESS_ENGINE_API Promotion : public ChessMove
	{
	public:
		Promotion();
		virtual ~Promotion();

		virtual bool Do(ChessGame* chessGame) override;
		virtual bool Undo(ChessGame* chessGame) override;

		// TODO: Indicate what kind of promotion here.
	};

	class CHESS_ENGINE_API EnPassant : public ChessMove
	{
	public:
		EnPassant();
		virtual ~EnPassant();

		virtual bool Do(ChessGame* chessGame) override;
		virtual bool Undo(ChessGame* chessGame) override;
	};
}