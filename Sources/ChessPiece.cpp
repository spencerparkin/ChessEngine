#include "ChessPiece.h"
#include "ChessGame.h"

using namespace ChessEngine;

//---------------------------------------- ChessPiece ----------------------------------------

ChessPiece::ChessPiece(ChessGame* game, const ChessVector& location, ChessColor color)
{
	this->game = game;
	this->location = location;
	this->color = color;
	this->game->SetSquareOccupant(this->location, this);
}

/*virtual*/ ChessPiece::~ChessPiece()
{
}

//---------------------------------------- Pawn ----------------------------------------

Pawn::Pawn(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{

}

/*virtual*/ Pawn::~Pawn()
{
}

/*virtual*/ void Pawn::GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const
{
}

//---------------------------------------- Knight ----------------------------------------

Knight::Knight(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{

}

/*virtual*/ Knight::~Knight()
{
}

/*virtual*/ void Knight::GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const
{
}

//---------------------------------------- Bishop ----------------------------------------

Bishop::Bishop(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{

}

/*virtual*/ Bishop::~Bishop()
{
}

/*virtual*/ void Bishop::GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const
{
}

//---------------------------------------- Rook ----------------------------------------

Rook::Rook(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{

}

/*virtual*/ Rook::~Rook()
{
}

/*virtual*/ void Rook::GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const
{
}

//---------------------------------------- Queen ----------------------------------------

Queen::Queen(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{

}

/*virtual*/ Queen::~Queen()
{
}

/*virtual*/ void Queen::GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const
{
}

//---------------------------------------- King ----------------------------------------

King::King(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{

}

/*virtual*/ King::~King()
{
}

/*virtual*/ void King::GenerateAllPossibleMoves(ChessMoveArray& chessMoveArray) const
{
}