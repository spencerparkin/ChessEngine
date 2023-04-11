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
			promotion->newPiece = new Knight(this->game, ChessVector(), this->color);
			moveArray.push_back(promotion);

			promotion = new Promotion();
			promotion->sourceLocation = this->location;
			promotion->destinationLocation = this->location + forwardDirection;
			promotion->newPiece = new Bishop(this->game, ChessVector(), this->color);
			moveArray.push_back(promotion);

			promotion = new Promotion();
			promotion->sourceLocation = this->location;
			promotion->destinationLocation = this->location + forwardDirection;
			promotion->newPiece = new Rook(this->game, ChessVector(), this->color);
			moveArray.push_back(promotion);

			promotion = new Promotion();
			promotion->sourceLocation = this->location;
			promotion->destinationLocation = this->location + forwardDirection;
			promotion->newPiece = new Queen(this->game, ChessVector(), this->color);
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

/*virtual*/ void Knight::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
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
}

//---------------------------------------- Rook ----------------------------------------

Rook::Rook(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ Rook::~Rook()
{
}

/*virtual*/ void Rook::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
}

//---------------------------------------- Queen ----------------------------------------

Queen::Queen(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ Queen::~Queen()
{
}

/*virtual*/ void Queen::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
}

//---------------------------------------- King ----------------------------------------

King::King(ChessGame* game, const ChessVector& location, ChessColor color) : ChessPiece(game, location, color)
{
}

/*virtual*/ King::~King()
{
}

/*virtual*/ void King::GenerateAllPossibleMoves(ChessMoveArray& moveArray) const
{
	// Note that we can look at the entire game's move history to determine if castling is possible.
}