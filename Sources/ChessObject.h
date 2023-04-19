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
			EMPTY = 100,
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
			CAPTURE_AND_PROMOTE,
			EN_PASSANT,
			GAME
		};

		virtual bool WriteToStream(std::ostream& stream) const = 0;
		virtual bool ReadFromStream(std::istream& stream) = 0;

		virtual Code GetCode() const = 0;

		static ChessObject* Factory(Code code);

		void WriteInt(std::ostream& stream, int value) const;
		void ReadInt(std::istream& stream, int& value);

		void WriteString(std::ostream& stream, const char* str) const;
		void ReadString(std::istream& stream, char* str, int strSize);
	};
}