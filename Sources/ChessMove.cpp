#include "ChessMove.h"

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

/*virtual*/ bool Travel::Do(ChessGame* chessGame)
{
	return false;
}

/*virtual*/ bool Travel::Undo(ChessGame* chessGame)
{
	return false;
}

//---------------------------------------- Capture ----------------------------------------

Capture::Capture()
{
}

/*virtual*/ Capture::~Capture()
{
}

/*virtual*/ bool Capture::Do(ChessGame* chessGame)
{
	return false;
}

/*virtual*/ bool Capture::Undo(ChessGame* chessGame)
{
	return false;
}

//---------------------------------------- Castle ----------------------------------------

Castle::Castle()
{
}

/*virtual*/ Castle::~Castle()
{
}

/*virtual*/ bool Castle::Do(ChessGame* chessGame)
{
	return false;
}

/*virtual*/ bool Castle::Undo(ChessGame* chessGame)
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

/*virtual*/ bool Promotion::Do(ChessGame* chessGame)
{
	return false;
}

/*virtual*/ bool Promotion::Undo(ChessGame* chessGame)
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

/*virtual*/ bool EnPassant::Do(ChessGame* chessGame)
{
	return false;
}

/*virtual*/ bool EnPassant::Undo(ChessGame* chessGame)
{
	return false;
}