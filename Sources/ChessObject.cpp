#include "ChessObject.h"
#include "ChessPiece.h"
#include "ChessMove.h"

using namespace ChessEngine;

ChessObject::ChessObject()
{
}

/*virtual*/ ChessObject::~ChessObject()
{
}

/*static*/ ChessObject* ChessObject::Factory(Code code)
{
	switch (code)
	{
		case Code::EMPTY:		return nullptr;
		case Code::PAWN:		return new Pawn(nullptr, ChessVector(-1, -1), ChessColor::Black);
		case Code::KNIGHT:		return new Knight(nullptr, ChessVector(-1, -1), ChessColor::Black);
		case Code::BISHOP:		return new Bishop(nullptr, ChessVector(-1, -1), ChessColor::Black);
		case Code::ROOK:		return new Rook(nullptr, ChessVector(-1, -1), ChessColor::Black);
		case Code::QUEEN:		return new Queen(nullptr, ChessVector(-1, -1), ChessColor::Black);
		case Code::KING:		return new King(nullptr, ChessVector(-1, -1), ChessColor::Black);
		case Code::TRAVEL:		return new Travel();
		case Code::CAPTURE:		return new Capture();
		case Code::CASTLE:		return new Castle();
		case Code::PROMOTION:	return new Promotion();
		case Code::EN_PASSANT:	return new EnPassant();
	}

	return nullptr;
}

void ChessObject::WriteInt(std::ostream& stream, int value) const
{
	stream.write((const char*)&value, sizeof(int));
}

void ChessObject::ReadInt(std::istream& stream, int& value)
{
	stream.read((char*)&value, sizeof(int));
}