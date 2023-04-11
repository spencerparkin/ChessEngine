#include "ChessMove.h"
#include "ChessGame.h"
#include "ChessPiece.h"

using namespace ChessEngine;

//---------------------------------------- ChessMove ----------------------------------------

ChessMove::ChessMove()
{
}

/*virtual*/ ChessMove::~ChessMove()
{
}

//---------------------------------------- Travel ----------------------------------------

Travel::Travel()
{
}

/*virtual*/ Travel::~Travel()
{
}

/*virtual*/ bool Travel::Do(ChessGame* game)
{
	return false;
}

/*virtual*/ bool Travel::Undo(ChessGame* game)
{
	return false;
}

//---------------------------------------- Capture ----------------------------------------

Capture::Capture()
{
	this->capturedPiece = nullptr;
}

/*virtual*/ Capture::~Capture()
{
	delete this->capturedPiece;
}

/*virtual*/ bool Capture::Do(ChessGame* game)
{
	ChessPiece* piece = nullptr;
	if (!game->GetSquareOccupant(this->sourceLocation, piece))
		return false;

	if (!game->GetSquareOccupant(this->destinationLocation, this->capturedPiece))
		return false;

	if (!this->capturedPiece)
		return false;

	if (!game->SetSquareOccupant(this->destinationLocation, piece))
		return false;

	return true;
}

/*virtual*/ bool Capture::Undo(ChessGame* game)
{
	if (!this->capturedPiece)
		return false;

	ChessPiece* piece = nullptr;
	if (!game->GetSquareOccupant(this->destinationLocation, piece))
		return false;

	if (!piece)
		return false;

	if (!game->SetSquareOccupant(this->sourceLocation, piece))	// TODO: Really should check that it's empty first.
		return false;

	if (!game->SetSquareOccupant(this->destinationLocation, this->capturedPiece))
		return false;

	this->capturedPiece = nullptr;
	return true;
}

//---------------------------------------- Castle ----------------------------------------

Castle::Castle()
{
}

/*virtual*/ Castle::~Castle()
{
}

/*virtual*/ bool Castle::Do(ChessGame* game)
{
	return false;
}

/*virtual*/ bool Castle::Undo(ChessGame* game)
{
	return false;
}

//---------------------------------------- Promotion ----------------------------------------

Promotion::Promotion()
{
}

/*virtual*/ Promotion::~Promotion()
{
}

/*virtual*/ bool Promotion::Do(ChessGame* game)
{
	return false;
}

/*virtual*/ bool Promotion::Undo(ChessGame* game)
{
	return false;
}

//---------------------------------------- EnPassant ----------------------------------------

EnPassant::EnPassant()
{
}

/*virtual*/ EnPassant::~EnPassant()
{
}

/*virtual*/ bool EnPassant::Do(ChessGame* game)
{
	return false;
}

/*virtual*/ bool EnPassant::Undo(ChessGame* game)
{
	return false;
}