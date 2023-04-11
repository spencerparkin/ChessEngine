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

		// Note that failure here ideally wouldn't put the board into a bad state, but if everything
		// is programmed correctly, a chess move should never fail to be done or undone, because the
		// only chess moves that should be applied are those generated by the chess pieces themselves.
		virtual bool Do(ChessGame* game) = 0;
		virtual bool Undo(ChessGame* game) = 0;

		ChessVector sourceLocation;
		ChessVector destinationLocation;
	};

	class CHESS_ENGINE_API Travel : public ChessMove
	{
	public:
		Travel();
		virtual ~Travel();

		virtual bool Do(ChessGame* game) override;
		virtual bool Undo(ChessGame* game) override;
	};

	class CHESS_ENGINE_API Capture : public ChessMove
	{
	public:
		Capture();
		virtual ~Capture();
		
		virtual bool Do(ChessGame* game) override;
		virtual bool Undo(ChessGame* game) override;

		ChessPiece* capturedPiece;
	};

	class CHESS_ENGINE_API Castle : public ChessMove
	{
	public:
		Castle();
		virtual ~Castle();

		virtual bool Do(ChessGame* game) override;
		virtual bool Undo(ChessGame* game) override;
	};

	class CHESS_ENGINE_API Promotion : public ChessMove
	{
	public:
		Promotion();
		virtual ~Promotion();

		virtual bool Do(ChessGame* game) override;
		virtual bool Undo(ChessGame* game) override;

		// TODO: Indicate what kind of promotion here.
	};

	class CHESS_ENGINE_API EnPassant : public ChessMove
	{
	public:
		EnPassant();
		virtual ~EnPassant();

		virtual bool Do(ChessGame* game) override;
		virtual bool Undo(ChessGame* game) override;
	};
}