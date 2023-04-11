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
	ChessPiece* piece = game->GetSquareOccupant(this->sourceLocation);
	if (!piece)
		return false;

	this->capturedPiece = game->GetSquareOccupant(this->destinationLocation);
	if (!this->capturedPiece)
		return false;

	game->SetSquareOccupant(this->destinationLocation, piece);
	return true;
}

/*virtual*/ bool Capture::Undo(ChessGame* game)
{
	if (!this->capturedPiece)
		return false;

	ChessPiece* piece = game->GetSquareOccupant(this->destinationLocation);
	if (!piece)
		return false;

	if (game->GetSquareOccupant(this->sourceLocation) != nullptr)
		return false;

	game->SetSquareOccupant(this->sourceLocation, piece);
	game->SetSquareOccupant(this->destinationLocation, this->capturedPiece);
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