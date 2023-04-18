#include "ChessPiece.h"
#include "ChessGame.h"
#include "ChessMove.h"

using namespace ChessEngine;

//---------------------------------------- ChessPiece ----------------------------------------

ChessPiece::ChessPiece(ChessGame* game, const ChessVector& location, ChessColor color)
{
	this->game = game;
	this->location = location;
	this->color = color;

	if (this->game)
		this->game->SetSquareOccupant(this->location, this);
}

/*virtual*/ ChessPiece::~ChessPiece()
{
}

/*virtual*/ bool ChessPiece::WriteToStream(std::ostream& stream) const
{
	char colorByte = (char)this->color;
	stream << colorByte;
	return true;
}

/*virtual*/ bool ChessPiece::ReadFromStream(std::istream& stream)
{
	char colorByte = 0;
	stream >> colorByte;
	this->color = (ChessColor)colorByte;
	return true;
}

void ChessPiece::GenerateMovesWithRayCast(const ChessVector& rayDirection, ChessMoveArray& moveArray, int maxLength /*= INT_MAX*/) const
{
	ChessColor opponentColor = (this->color == ChessColor::White) ? ChessColor::Black : ChessColor::White;
	for(int i = 1; i <= maxLength; i++)
	{
		ChessVector rayLocation = this->location + rayDirection * i;
		if (!this->game->IsLocationValid(rayLocation))
			break;

		ChessPiece* piece = this->game->GetSquareOccupant(rayLocation);
		if (!piece)
		{
			Travel* travel = new Travel();
			travel->sourceLocation = this->location;
			travel->destinationLocation = rayLocation;
			moveArray.push_back(travel);
		}
		else
		{
			if (piece->color == opponentColor)
			{
				Capture* capture = new Capture();
				capture->sourceLocation = this->location;
				capture->destinationLocation = rayLocation;
				moveArray.push_back(capture);
			}

			break;
		}
	}
}

//---------------------------------------- Pawn ----------------------------------------

Pawn::Pawn(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ Pawn::~Pawn()
{
}

/*virtual*/ std::string Pawn::GetName() const
{
	return "Pawn";
}

/*virtual*/ int Pawn::GetScore() const
{
	return 10;
}

/*virtual*/ ChessPiece::Code Pawn::GetCode() const
{
	return Code::PAWN;
}

/*virtual*/ void Pawn::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
	ChessColor opponentColor = (this->color == ChessColor::White) ? ChessColor::Black : ChessColor::White;
	ChessVector forwardDirection(0, (this->color == ChessColor::White) ? 1 : -1);
	int initialRank = (this->color == ChessColor::White) ? 1 : 6;
	int finalRank = (this->color == ChessColor::White) ? 7 : 0;

	if (!this->game->GetSquareOccupant(this->location + forwardDirection))
	{
		if ((this->location + forwardDirection).rank == finalRank)
		{
			Promotion* promotion = new Promotion();
			promotion->sourceLocation = this->location;
			promotion->destinationLocation = this->location + forwardDirection;
			promotion->SetPromotedPiece(new Knight(nullptr, ChessVector(-1, -1), this->color));
			moveArray.push_back(promotion);

			promotion = new Promotion();
			promotion->sourceLocation = this->location;
			promotion->destinationLocation = this->location + forwardDirection;
			promotion->SetPromotedPiece(new Bishop(nullptr, ChessVector(-1, -1), this->color));
			moveArray.push_back(promotion);

			promotion = new Promotion();
			promotion->sourceLocation = this->location;
			promotion->destinationLocation = this->location + forwardDirection;
			promotion->SetPromotedPiece(new Rook(nullptr, ChessVector(-1, -1), this->color));
			moveArray.push_back(promotion);

			promotion = new Promotion();
			promotion->sourceLocation = this->location;
			promotion->destinationLocation = this->location + forwardDirection;
			promotion->SetPromotedPiece(new Queen(nullptr, ChessVector(-1, -1), this->color));
			moveArray.push_back(promotion);
		}
		else
		{
			Travel* travel = new Travel();
			travel->sourceLocation = this->location;
			travel->destinationLocation = this->location + forwardDirection;
			moveArray.push_back(travel);
		}

		if (this->location.rank == initialRank && !this->game->GetSquareOccupant(this->location + forwardDirection * 2))
		{
			Travel* travel = new Travel();
			travel->sourceLocation = this->location;
			travel->destinationLocation = this->location + forwardDirection * 2;
			moveArray.push_back(travel);
		}
	}

	ChessVector sideVector[2];
	sideVector[0].file = -1;
	sideVector[1].file = 1;
	for (int i = 0; i < 2; i++)
	{
		if (!this->game->IsLocationValid(this->location + forwardDirection + sideVector[i]))
			continue;

		ChessPiece* piece = this->game->GetSquareOccupant(this->location + forwardDirection + sideVector[i]);
		if (piece)
		{
			if (piece->color == opponentColor)
			{
				Capture* capture = new Capture();
				capture->sourceLocation = this->location;
				capture->destinationLocation = this->location + forwardDirection + sideVector[i];
				moveArray.push_back(capture);
			}
		}
		else
		{
			piece = this->game->GetSquareOccupant(this->location + sideVector[i]);
			if (piece && piece->color == opponentColor && dynamic_cast<Pawn*>(piece) && this->game->GetNumMoves() > 0)
			{
				const Travel* travel = dynamic_cast<const Travel*>(this->game->GetMove(this->game->GetNumMoves() - 1));
				if (travel && travel->sourceLocation == this->location + forwardDirection * 2 + sideVector[i] && travel->destinationLocation == this->location + sideVector[i])
				{
					EnPassant* enPassant = new EnPassant();
					enPassant->sourceLocation = this->location;
					enPassant->destinationLocation = this->location + forwardDirection + sideVector[i];
					enPassant->captureLocation = this->location + sideVector[i];
					moveArray.push_back(enPassant);
				}
			}
		}
	}
}

//---------------------------------------- Knight ----------------------------------------

Knight::Knight(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ Knight::~Knight()
{
}

/*virtual*/ std::string Knight::GetName() const
{
	return "Knight";
}

/*virtual*/ int Knight::GetScore() const
{
	return 30;
}

/*virtual*/ ChessPiece::Code Knight::GetCode() const
{
	return Code::KNIGHT;
}

/*virtual*/ void Knight::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
	ChessColor opponentColor = (this->color == ChessColor::White) ? ChessColor::Black : ChessColor::White;

	ChessVector ellVectorArray[2];
	ellVectorArray[0] = ChessVector(1, 2);
	ellVectorArray[1] = ChessVector(2, 1);
	for (int i = 0; i < 2; i++)
	{
		const ChessVector& ellVector = ellVectorArray[i];
		for (int j = 0; j < 4; j++)
		{
			int fileScale = (j & 0x1) ? 1 : -1;
			int rankScale = (j & 0x2) ? 1 : -1;
			ChessVector moveVector(ellVector.file * fileScale, ellVector.rank * rankScale);

			if (this->game->IsLocationValid(this->location + moveVector))
			{
				ChessPiece* piece = this->game->GetSquareOccupant(this->location + moveVector);
				if (!piece)
				{
					Travel* travel = new Travel();
					travel->sourceLocation = this->location;
					travel->destinationLocation = this->location + moveVector;
					moveArray.push_back(travel);
				}
				else if (piece->color == opponentColor)
				{
					Capture* capture = new Capture();
					capture->sourceLocation = this->location;
					capture->destinationLocation = this->location + moveVector;
					moveArray.push_back(capture);
				}
			}
		}
	}
}

//---------------------------------------- Bishop ----------------------------------------

Bishop::Bishop(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ Bishop::~Bishop()
{
}

/*virtual*/ void Bishop::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
	this->GenerateMovesWithRayCast(ChessVector(1, 1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(-1, 1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(1, -1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(-1, -1), moveArray);
}

/*virtual*/ std::string Bishop::GetName() const
{
	return "Bishop";
}

/*virtual*/ int Bishop::GetScore() const
{
	return 30;
}

/*virtual*/ ChessPiece::Code Bishop::GetCode() const
{
	return Code::BISHOP;
}

//---------------------------------------- Rook ----------------------------------------

Rook::Rook(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ Rook::~Rook()
{
}

std::string Rook::GetName() const
{
	return "Rook";
}

/*virtual*/ int Rook::GetScore() const
{
	return 50;
}

/*virtual*/ ChessPiece::Code Rook::GetCode() const
{
	return Code::ROOK;
}

/*virtual*/ void Rook::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
	this->GenerateMovesWithRayCast(ChessVector(1, 0), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(0, 1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(-1, 0), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(0, -1), moveArray);
}

//---------------------------------------- Queen ----------------------------------------

Queen::Queen(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ Queen::~Queen()
{
}

/*virtual*/ std::string Queen::GetName() const
{
	return "Queen";
}

/*virtual*/ int Queen::GetScore() const
{
	return 90;
}

/*virtual*/ ChessPiece::Code Queen::GetCode() const
{
	return Code::QUEEN;
}

/*virtual*/ void Queen::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
	this->GenerateMovesWithRayCast(ChessVector(1, 1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(-1, 1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(1, -1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(-1, -1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(1, 0), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(0, 1), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(-1, 0), moveArray);
	this->GenerateMovesWithRayCast(ChessVector(0, -1), moveArray);
}

//---------------------------------------- King ----------------------------------------

King::King(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ King::~King()
{
}

/*virtual*/ std::string King::GetName() const
{
	return "King";
}

/*virtual*/ int King::GetScore() const
{
	return 900;
}

/*virtual*/ ChessPiece::Code King::GetCode() const
{
	return Code::KING;
}

/*virtual*/ void King::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
	this->GenerateMovesWithRayCast(ChessVector(1, 1), moveArray, 1);
	this->GenerateMovesWithRayCast(ChessVector(-1, 1), moveArray, 1);
	this->GenerateMovesWithRayCast(ChessVector(1, -1), moveArray, 1);
	this->GenerateMovesWithRayCast(ChessVector(-1, -1), moveArray, 1);
	this->GenerateMovesWithRayCast(ChessVector(1, 0), moveArray, 1);
	this->GenerateMovesWithRayCast(ChessVector(0, 1), moveArray, 1);
	this->GenerateMovesWithRayCast(ChessVector(-1, 0), moveArray, 1);
	this->GenerateMovesWithRayCast(ChessVector(0, -1), moveArray, 1);

	ChessVector initialKingLocation;
	ChessVector initialKingSideRookLocation;
	ChessVector initialQueenSideRookLocation;
	ChessVector kingSideDirection;
	ChessVector queenSideDirection;

	switch (this->color)
	{
		case ChessColor::White:
		{
			initialKingLocation = ChessVector(4, 0);
			initialKingSideRookLocation = ChessVector(7, 0);
			initialQueenSideRookLocation = ChessVector(0, 0);
			kingSideDirection = ChessVector(1, 0);
			queenSideDirection = ChessVector(-1, 0);
			break;
		}
		case ChessColor::Black:
		{
			initialKingLocation = ChessVector(4, 7);
			initialKingSideRookLocation = ChessVector(7, 7);
			initialQueenSideRookLocation = ChessVector(0, 7);
			kingSideDirection = ChessVector(1, 0);
			queenSideDirection = ChessVector(-1, 0);
			break;
		}
	}

	if (this->location == initialKingLocation && !this->game->PieceEverMovedFromLocation(initialKingLocation))
	{
		Rook* kingSideRook = dynamic_cast<Rook*>(this->game->GetSquareOccupant(initialKingSideRookLocation));
		Rook* queenSideRook = dynamic_cast<Rook*>(this->game->GetSquareOccupant(initialQueenSideRookLocation));

		if (kingSideRook && !this->game->PieceEverMovedFromLocation(initialKingSideRookLocation))
		{
			if (!this->game->GetSquareOccupant(this->location + kingSideDirection) &&
				!this->game->GetSquareOccupant(this->location + kingSideDirection * 2))
			{
				Castle* castle = new Castle();
				castle->sourceLocation = this->location;
				castle->destinationLocation = this->location + kingSideDirection * 2;
				castle->rookSourceLocation = initialKingSideRookLocation;
				castle->rookDestinationLocation = this->location + kingSideDirection;
				moveArray.push_back(castle);
			}
		}

		if (queenSideRook && !this->game->PieceEverMovedFromLocation(initialQueenSideRookLocation))
		{
			if (!this->game->GetSquareOccupant(this->location + queenSideDirection) &&
				!this->game->GetSquareOccupant(this->location + queenSideDirection * 2) &&
				!this->game->GetSquareOccupant(this->location + queenSideDirection * 3))
			{
				Castle* castle = new Castle();
				castle->sourceLocation = this->location;
				castle->destinationLocation = this->location + queenSideDirection * 2;
				castle->rookSourceLocation = initialQueenSideRookLocation;
				castle->rookDestinationLocation = this->location + queenSideDirection;
				moveArray.push_back(castle);
			}
		}
	}
}