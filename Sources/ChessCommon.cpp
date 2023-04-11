#include "ChessCommon.h"
#include "ChessMove.h"

using namespace ChessEngine;

ChessVector::ChessVector()
{
	this->file = 0;
	this->rank = 0;
}

ChessVector::ChessVector(int file, int rank)
{
	this->file = file;
	this->rank = rank;
}

/*virtual*/ ChessVector::~ChessVector()
{
}

void ChessVector::operator=(const ChessVector& vector)
{
	this->file = vector.file;
	this->rank = vector.rank;
}

ChessVector operator+(const ChessVector& vectorA, const ChessVector& vectorB)
{
	return ChessVector(vectorA.file + vectorB.file, vectorA.rank + vectorB.rank);
}

ChessVector operator-(const ChessVector& vectorA, const ChessVector& vectorB)
{
	return ChessVector(vectorA.file - vectorB.file, vectorA.rank - vectorB.rank);
}

namespace ChessEngine
{
	void DeleteMoveArray(ChessMoveArray& moveArray)
	{
		for (int i = 0; i < (signed)moveArray.size(); i++)
			delete moveArray[i];

		moveArray.clear();
	}
}