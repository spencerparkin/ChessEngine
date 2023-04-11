#pragma once

#if defined CHESSENGINE_EXPORTS
#define CHESS_ENGINE_API		__declspec(dllexport)
#elif defined CHESSENGINE_IMPORTS
#define CHESS_ENGINE_API		__declspec(dllimport)
#else
#define CHESS_ENGINE_API
#endif

#include <vector>
#include <list>
#include <math.h>

#define CHESS_BOARD_RANKS		8
#define CHESS_BOARD_FILES		8

namespace ChessEngine
{
	enum class ChessColor
	{
		Black,
		White
	};

	enum class GameResult
	{
		None,
		CheckMate,
		StaleMate
	};

	class CHESS_ENGINE_API ChessVector
	{
	public:
		ChessVector();
		ChessVector(int file, int rank);
		virtual ~ChessVector();

		void operator=(const ChessVector& vector);

		int file, rank;
	};

	ChessVector operator+(const ChessVector& vectorA, const ChessVector& vectorB);
	ChessVector operator-(const ChessVector& vectorA, const ChessVector& vectorB);

	class ChessMove;
	typedef std::vector<ChessMove*> ChessMoveArray;

	CHESS_ENGINE_API void DeleteMoveArray(ChessMoveArray& moveArray);
}