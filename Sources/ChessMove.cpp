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

/*virtual*/ bool ChessMove::WriteToStream(std::ostream& stream) const
{
	this->WriteInt(stream, this->sourceLocation.file);
	this->WriteInt(stream, this->sourceLocation.rank);
	this->WriteInt(stream, this->destinationLocation.file);
	this->WriteInt(stream, this->destinationLocation.rank);
	return true;
}

/*virtual*/ bool ChessMove::ReadFromStream(std::istream& stream)
{
	this->ReadInt(stream, this->sourceLocation.file);
	this->ReadInt(stream, this->sourceLocation.rank);
	this->ReadInt(stream, this->destinationLocation.file);
	this->ReadInt(stream, this->destinationLocation.rank);
	return true;
}

bool ChessMove::WritePiece(std::ostream& stream, const ChessPiece* piece) const
{
	if (!piece)
	{
		char code = (char)Code::EMPTY;
		stream << code;
	}
	else
	{
		char code = (char)piece->GetCode();
		stream << code;
		if (!piece->WriteToStream(stream))
			return false;
	}

	return true;
}

bool ChessMove::ReadPiece(std::istream& stream, ChessPiece*& piece) const
{
	char code = (char)Code::EMPTY;
	stream >> code;
	ChessObject* object = ChessObject::Factory((Code)code);
	if (object)
	{
		piece = dynamic_cast<ChessPiece*>(object);
		if (!piece)
		{
			delete object;
			return false;
		}

		if (!piece->ReadFromStream(stream))
			return false;
	}

	return true;
}

//---------------------------------------- Travel ----------------------------------------

Travel::Travel()
{
}

/*virtual*/ Travel::~Travel()
{
}

/*virtual*/ ChessObject::Code Travel::GetCode() const
{
	return Code::TRAVEL;
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

/*virtual*/ ChessObject::Code Capture::GetCode() const
{
	return Code::CAPTURE;
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

/*virtual*/ bool Capture::WriteToStream(std::ostream& stream) const
{
	if (!ChessMove::WriteToStream(stream))
		return false;

	if (!this->WritePiece(stream, this->capturedPiece))
		return false;

	return true;
}

/*virtual*/ bool Capture::ReadFromStream(std::istream& stream)
{
	if (!ChessMove::ReadFromStream(stream))
		return false;

	if (!this->ReadPiece(stream, this->capturedPiece))
		return false;

	return true;
}

//---------------------------------------- Castle ----------------------------------------

Castle::Castle()
{
}

/*virtual*/ Castle::~Castle()
{
}

/*virtual*/ ChessObject::Code Castle::GetCode() const
{
	return Code::CASTLE;
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

/*virtual*/ bool Castle::WriteToStream(std::ostream& stream) const
{
	if (!ChessMove::WriteToStream(stream))
		return false;

	this->WriteInt(stream, rookSourceLocation.file);
	this->WriteInt(stream, rookSourceLocation.rank);
	this->WriteInt(stream, rookDestinationLocation.file);
	this->WriteInt(stream, rookDestinationLocation.rank);
	return true;
}

/*virtual*/ bool Castle::ReadFromStream(std::istream& stream)
{
	if (!ChessMove::ReadFromStream(stream))
		return false;

	this->ReadInt(stream, rookSourceLocation.file);
	this->ReadInt(stream, rookSourceLocation.rank);
	this->ReadInt(stream, rookDestinationLocation.file);
	this->ReadInt(stream, rookDestinationLocation.rank);
	return true;
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

/*virtual*/ ChessObject::Code Promotion::GetCode() const
{
	return Code::PROMOTION;
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

/*virtual*/ bool Promotion::WriteToStream(std::ostream& stream) const
{
	if (!ChessMove::WriteToStream(stream))
		return false;

	if (!this->WritePiece(stream, this->newPiece))
		return false;

	if (!this->WritePiece(stream, this->oldPiece))
		return false;

	this->WriteString(stream, this->cachedDesc);
	return true;
}

/*virtual*/ bool Promotion::ReadFromStream(std::istream& stream)
{
	if (!ChessMove::ReadFromStream(stream))
		return false;

	if (!this->ReadPiece(stream, this->newPiece))
		return false;

	if (!this->ReadPiece(stream, this->oldPiece))
		return false;

	this->ReadString(stream, this->cachedDesc, sizeof(this->cachedDesc));
	return true;
}

//---------------------------------------- CapturePromotion ----------------------------------------

CapturePromotion::CapturePromotion()
{
}

/*virtual*/ CapturePromotion::~CapturePromotion()
{
}

/*virtual*/ bool CapturePromotion::Do(ChessGame* game)
{
	if (!this->newPiece)
		return false;

	this->oldPiece = game->GetSquareOccupant(this->sourceLocation);
	if (!this->oldPiece)
		return false;

	if (!Capture::Do(game))
		return false;

	game->SetSquareOccupant(this->destinationLocation, this->newPiece);
	this->newPiece = nullptr;
	return true;
}

/*virtual*/ bool CapturePromotion::Undo(ChessGame* game)
{
	if (!this->oldPiece)
		return false;

	this->newPiece = game->GetSquareOccupant(this->destinationLocation);
	if (!this->newPiece)
		return false;

	if (!Capture::Undo(game))
		return false;

	game->SetSquareOccupant(this->sourceLocation, this->oldPiece);
	this->oldPiece = nullptr;
	return true;
}

/*virtual*/ std::string CapturePromotion::GetDescription() const
{
	std::string descA = Capture::GetDescription();
	std::string descB = Promotion::GetDescription();
	return descA + " & " + descB;
}

/*virtual*/ int CapturePromotion::GetSortKey() const
{
	return 4;
}

/*virtual*/ ChessObject::Code CapturePromotion::GetCode() const
{
	return Code::CAPTURE_AND_PROMOTE;
}

/*virtual*/ bool CapturePromotion::WriteToStream(std::ostream& stream) const
{
	if (!Promotion::WriteToStream(stream))
		return false;

	// Here, the source & destination are written redundantly, but I'm okay with that.
	if (!Capture::WriteToStream(stream))
		return false;

	return true;
}

/*virtual*/ bool CapturePromotion::ReadFromStream(std::istream& stream)
{
	if (!Promotion::ReadFromStream(stream))
		return false;

	if (!Capture::ReadFromStream(stream))
		return false;

	return true;
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

/*virtual*/ ChessObject::Code EnPassant::GetCode() const
{
	return Code::EN_PASSANT;
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

/*virtual*/ bool EnPassant::WriteToStream(std::ostream& stream) const
{
	if (!ChessMove::WriteToStream(stream))
		return false;

	this->WriteInt(stream, this->captureLocation.file);
	this->WriteInt(stream, this->captureLocation.rank);

	if (!this->WritePiece(stream, this->capturedPiece))
		return false;

	return true;
}

/*virtual*/ bool EnPassant::ReadFromStream(std::istream& stream)
{
	if (!ChessMove::ReadFromStream(stream))
		return false;

	this->ReadInt(stream, this->captureLocation.file);
	this->ReadInt(stream, this->captureLocation.rank);

	if (!this->ReadPiece(stream, this->capturedPiece))
		return false;

	return true;
}