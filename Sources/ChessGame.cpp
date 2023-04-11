#include "ChessGame.h"
#include "ChessPiece.h"
#include "ChessMove.h"

using namespace ChessEngine;

ChessGame::ChessGame()
{
	this->chessMoveStack = new ChessMoveArray();

	for (int i = 0; i < CHESS_BOARD_FILES; i++)
		for (int j = 0; j < CHESS_BOARD_RANKS; j++)
			this->boardMatrix[i][j] = nullptr;
}

/*virtual*/ ChessGame::~ChessGame()
{
	this->Clear();

	delete this->chessMoveStack;
}

void ChessGame::Clear()
{
	for (int i = 0; i < CHESS_BOARD_FILES; i++)
	{
		for (int j = 0; j < CHESS_BOARD_RANKS; j++)
		{
			delete this->boardMatrix[i][j];
			this->boardMatrix[i][j] = nullptr;
		}
	}

	for (int i = 0; i < (signed)this->chessMoveStack->size(); i++)
		delete (*this->chessMoveStack)[i];

	this->chessMoveStack->clear();
}

void ChessGame::Reset()
{
	this->Clear();

	for (int i = 0; i < CHESS_BOARD_FILES; i++)
	{
		new Pawn(this, ChessVector(i, 1), ChessColor::White);
		new Pawn(this, ChessVector(i, 6), ChessColor::Black);
	}

	new Rook(this, ChessVector(0, 0), ChessColor::White);
	new Rook(this, ChessVector(7, 0), ChessColor::White);
	new Rook(this, ChessVector(0, 7), ChessColor::Black);
	new Rook(this, ChessVector(7, 7), ChessColor::Black);

	new Knight(this, ChessVector(1, 0), ChessColor::White);
	new Knight(this, ChessVector(6, 0), ChessColor::White);
	new Knight(this, ChessVector(1, 7), ChessColor::Black);
	new Knight(this, ChessVector(6, 7), ChessColor::Black);

	new Bishop(this, ChessVector(2, 0), ChessColor::White);
	new Bishop(this, ChessVector(5, 0), ChessColor::White);
	new Bishop(this, ChessVector(2, 7), ChessColor::Black);
	new Bishop(this, ChessVector(5, 7), ChessColor::Black);

	new Queen(this, ChessVector(3, 0), ChessColor::White);
	new King(this, ChessVector(4, 0), ChessColor::White);
	new Queen(this, ChessVector(3, 7), ChessColor::Black);
	new King(this, ChessVector(4, 7), ChessColor::Black);
}

bool ChessGame::IsLocationValid(const ChessVector& location) const
{
	if (location.file < 0 || location.file >= CHESS_BOARD_FILES)
		return false;

	if (location.rank < 0 || location.rank >= CHESS_BOARD_RANKS)
		return false;

	return true;
}

bool ChessGame::GetSquareOccupant(const ChessVector& location, ChessPiece*& piece) const
{
	if (!this->IsLocationValid(location))
		return false;

	piece = this->boardMatrix[location.file][location.rank];
	return true;
}

bool ChessGame::SetSquareOccupant(const ChessVector& location, ChessPiece* piece)
{
	if (!this->IsLocationValid(location))
		return false;

	this->boardMatrix[location.file][location.rank] = piece;
	return true;
}

bool ChessGame::PushMove(ChessMove* move)
{
	if (!move->Do(this))
		return false;

	this->chessMoveStack->push_back(move);
	return true;
}

bool ChessGame::PopMove()
{
	if (this->chessMoveStack->size() == 0)
		return false;

	ChessMove* move = this->chessMoveStack->back();
	if (!move->Undo(this))
		return false;

	delete move;
	return true;
}