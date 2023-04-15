#include "ChessCommon.h"
#include "ChessMove.h"
#include <sstream>

using namespace ChessEngine;

ChessVector::ChessVector()
{
	this->file = 0;
	this->rank = 0;
}

ChessVector::ChessVector(const ChessVector& vector)
{
	this->file = vector.file;
	this->rank = vector.rank;
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

ChessVector ChessVector::operator*(int scalar) const
{
	return ChessVector(this->file * scalar, this->rank * scalar);
}

bool ChessVector::operator==(const ChessVector& vector) const
{
	return this->file == vector.file && this->rank == vector.rank;
}

bool ChessVector::operator!=(const ChessVector& vector) const
{
	return this->file != vector.file || this->rank != vector.rank;
}

std::string ChessVector::GetLocationString() const
{
	std::stringstream stream;
	if (this->file < 0 || this->file >= CHESS_BOARD_FILES)
		stream << "?";
	else
	{
		static char fileLetter[] = "ABCDEFGH";
		stream << fileLetter[this->file];
	}

	if (this->rank < 0 || this->rank >= CHESS_BOARD_RANKS)
		stream << "?";
	else
		stream << (this->rank + 1);

	return stream.str();
}

int ChessVector::TaxiCabLength() const
{
	return abs(this->file) + abs(this->rank);
}

int ChessVector::ShortestDistanceToBoardEdge() const
{
	int i = (this->file < CHESS_BOARD_FILES / 2) ? this->file : (CHESS_BOARD_FILES - 1 - this->file);
	int j = (this->rank < CHESS_BOARD_RANKS / 2) ? this->rank : (CHESS_BOARD_RANKS - 1 - this->rank);
	return i < j ? i : j;
}

namespace ChessEngine
{
	ChessVector operator+(const ChessVector& vectorA, const ChessVector& vectorB)
	{
		return ChessVector(vectorA.file + vectorB.file, vectorA.rank + vectorB.rank);
	}

	ChessVector operator-(const ChessVector& vectorA, const ChessVector& vectorB)
	{
		return ChessVector(vectorA.file - vectorB.file, vectorA.rank - vectorB.rank);
	}

	void DeleteMoveArray(ChessMoveArray& moveArray)
	{
		for (int i = 0; i < (signed)moveArray.size(); i++)
			delete moveArray[i];

		moveArray.clear();
	}
}