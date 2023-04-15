#include "ChessMove.h"
#include "ChessGame.h"
#include "ChessPiece.h"
#include <sstream>

using namespace ChessEngine;

//---------------------------------------- ChessMove ----------------------------------------

ChessMove::ChessMove()
{
}

/*virtual*/ ChessMove::~ChessMove()
{
}

/*virtual*/ std::string ChessMove::GetDescription() const
{
	return "?";
}

/*virtual*/ int ChessMove::GetSortKey() const
{
	return 0;
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
	ChessPiece* piece = game->GetSquareOccupant(this->sourceLocation);
	if (!piece)
		return false;

	if (game->GetSquareOccupant(this->destinationLocation))
		return false;

	game->SetSquareOccupant(this->sourceLocation, nullptr);
	game->SetSquareOccupant(this->destinationLocation, piece);
	return true;
}

/*virtual*/ bool Travel::Undo(ChessGame* game)
{
	ChessPiece* piece = game->GetSquareOccupant(this->destinationLocation);
	if (!piece)
		return false;

	if (game->GetSquareOccupant(this->sourceLocation))
		return false;

	game->SetSquareOccupant(this->sourceLocation, piece);
	game->SetSquareOccupant(this->destinationLocation, nullptr);
	return true;
}

/*virtual*/ std::string Travel::GetDescription() const
{
	std::stringstream stream;
	stream << "Move " << this->sourceLocation.GetLocationString() << " to " << this->destinationLocation.GetLocationString();
	return stream.str();
}

/*virtual*/ int Travel::GetSortKey() const
{
	return 1;
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
	game->SetSquareOccupant(this->sourceLocation, nullptr);
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

/*virtual*/ std::string Capture::GetDescription() const
{
	std::stringstream stream;
	stream << this->sourceLocation.GetLocationString() << " captures " << this->destinationLocation.GetLocationString();
	return stream.str();
}

/*virtual*/ int Capture::GetSortKey() const
{
	return 2;
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
	if (game->GetSquareOccupant(this->destinationLocation) || game->GetSquareOccupant(this->rookDestinationLocation))
		return false;

	ChessPiece* king = game->GetSquareOccupant(this->sourceLocation);
	ChessPiece* rook = game->GetSquareOccupant(this->rookSourceLocation);

	if (!dynamic_cast<King*>(king) || !dynamic_cast<Rook*>(rook))
		return false;

	game->SetSquareOccupant(this->sourceLocation, nullptr);
	game->SetSquareOccupant(this->rookSourceLocation, nullptr);
	game->SetSquareOccupant(this->destinationLocation, king);
	game->SetSquareOccupant(this->rookDestinationLocation, rook);
	return true;
}

/*virtual*/ bool Castle::Undo(ChessGame* game)
{
	if (game->GetSquareOccupant(this->sourceLocation) || game->GetSquareOccupant(this->rookSourceLocation))
		return false;

	ChessPiece* king = game->GetSquareOccupant(this->destinationLocation);
	ChessPiece* rook = game->GetSquareOccupant(this->rookDestinationLocation);

	if (!dynamic_cast<King*>(king) || !dynamic_cast<Rook*>(rook))
		return false;

	game->SetSquareOccupant(this->sourceLocation, king);
	game->SetSquareOccupant(this->rookSourceLocation, rook);
	game->SetSquareOccupant(this->destinationLocation, nullptr);
	game->SetSquareOccupant(this->rookDestinationLocation, nullptr);
	return true;
}

/*virtual*/ std::string Castle::GetDescription() const
{
	std::stringstream stream;
	stream << "Castle " << this->sourceLocation.GetLocationString() << " to " << this->destinationLocation.GetLocationString();
	return stream.str();
}

/*virtual*/ int Castle::GetSortKey() const
{
	return 1;
}

//---------------------------------------- Promotion ----------------------------------------

Promotion::Promotion()
{
	this->newPiece = nullptr;
	this->oldPiece = nullptr;

	this->cachedDesc[0] = '\0';
}

/*virtual*/ Promotion::~Promotion()
{
	delete this->newPiece;
	delete this->oldPiece;
}

/*virtual*/ bool Promotion::Do(ChessGame* game)
{
	if (!this->newPiece)
		return false;

	this->oldPiece = game->GetSquareOccupant(this->sourceLocation);
	if (!this->oldPiece)
		return false;

	game->SetSquareOccupant(this->sourceLocation, nullptr);
	game->SetSquareOccupant(this->destinationLocation, this->newPiece);
	this->newPiece = nullptr;
	return true;
}

/*virtual*/ bool Promotion::Undo(ChessGame* game)
{
	if (!this->oldPiece)
		return false;

	this->newPiece = game->GetSquareOccupant(this->destinationLocation);
	if (!this->newPiece)
		return false;

	game->SetSquareOccupant(this->sourceLocation, this->oldPiece);
	game->SetSquareOccupant(this->destinationLocation, nullptr);
	this->oldPiece = nullptr;
	return true;
}

void Promotion::SetPromotedPiece(ChessPiece* piece)
{
	this->newPiece = piece;
	std::stringstream stream;
	stream << "Promote pawn to " << this->newPiece->GetName() << " at " << this->destinationLocation.GetLocationString();
	::strcpy_s(this->cachedDesc, sizeof(this->cachedDesc), stream.str().c_str());
}

/*virtual*/ std::string Promotion::GetDescription() const
{
	return this->cachedDesc;
}

/*virtual*/ int Promotion::GetSortKey() const
{
	return 3;
}

//---------------------------------------- EnPassant ----------------------------------------

EnPassant::EnPassant()
{
	this->capturedPiece = nullptr;
}

/*virtual*/ EnPassant::~EnPassant()
{
	delete this->capturedPiece;
}

/*virtual*/ bool EnPassant::Do(ChessGame* game)
{
	if (game->GetSquareOccupant(this->destinationLocation))
		return false;

	ChessPiece* piece = game->GetSquareOccupant(this->sourceLocation);
	if (!piece)
		return false;

	this->capturedPiece = game->GetSquareOccupant(this->captureLocation);
	if (!this->capturedPiece)
		return false;

	game->SetSquareOccupant(this->destinationLocation, piece);
	game->SetSquareOccupant(this->sourceLocation, nullptr);
	game->SetSquareOccupant(this->captureLocation, nullptr);
	return true;
}

/*virtual*/ bool EnPassant::Undo(ChessGame* game)
{
	if (!this->capturedPiece)
		return false;

	if (game->GetSquareOccupant(this->sourceLocation))
		return false;

	if (game->GetSquareOccupant(this->captureLocation))
		return false;

	ChessPiece* piece = game->GetSquareOccupant(this->destinationLocation);
	if (!piece)
		return false;

	game->SetSquareOccupant(this->destinationLocation, nullptr);
	game->SetSquareOccupant(this->sourceLocation, piece);
	game->SetSquareOccupant(this->captureLocation, this->capturedPiece);
	this->capturedPiece = nullptr;
	return true;
}

/*virtual*/ std::string EnPassant::GetDescription() const
{
	std::stringstream stream;
	stream << "En Passant from " << this->sourceLocation.GetLocationString() << " to " << this->destinationLocation.GetLocationString();
	return stream.str();
}

/*virtual*/ int EnPassant::GetSortKey() const
{
	return 2;
}