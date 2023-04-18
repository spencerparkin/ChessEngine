#pragma once

#include "ChessCommon.h"
#include <istream>
#include <ostream>

namespace ChessEngine
{
	class CHESS_ENGINE_API ChessObject
	{
	public:
		ChessObject();
		virtual ~ChessObject();

		enum class Code
		{
			EMPTY,
			PAWN,
			KNIGHT,
			BISHOP,
			ROOK,
			QUEEN,
			KING,
			TRAVEL,
			CAPTURE,
			CASTLE,
			PROMOTION,
			EN_PASSANT,
			GAME
		};

		virtual bool WriteToStream(std::ostream& stream) const = 0;
		virtual bool ReadFromStream(std::istream& stream) = 0;

		virtual Code GetCode() const = 0;

		static ChessObject* Factory(Code code);
	};
}