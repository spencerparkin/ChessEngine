#include "ChessAI.h"
#include "ChessGame.h"
#include "ChessPiece.h"

using namespace ChessEngine;

//---------------------------------------- ChessAI ----------------------------------------

ChessAI::ChessAI()
{
}

/*virtual*/ ChessAI::~ChessAI()
{
}

/*virtual*/ bool ChessAI::ProgressUpdate(float percentage)
{
	return true;
}

//---------------------------------------- ChessMinimaxAI ----------------------------------------

ChessMinimaxAI::ChessMinimaxAI(int maxDepth)
{
	this->bestMove = nullptr;
	this->maxDepth = maxDepth;
}

/*virtual*/ ChessMinimaxAI::~ChessMinimaxAI()
{
}

/*virtual*/ ChessMove* ChessMinimaxAI::CalculateRecommendedMove(ChessColor favoredColor, ChessGame* game)
{
	this->bestMove = nullptr;

	int numMoves = game->GetNumMoves();

	int score = 0;
	bool success = this->Minimax(Goal::MAXIMIZE, favoredColor, favoredColor, game, 0, score);

	assert(numMoves == game->GetNumMoves());

	if (!success)
		this->bestMove = nullptr;

	return this->bestMove;
}

bool ChessMinimaxAI::Minimax(Goal goal, ChessColor favoredColor, ChessColor whoseTurn, ChessGame* game, int depth, int& score, int* currentSuperScore /*= nullptr*/)
{
	if (depth >= this->maxDepth)
	{
		score = this->EvaluationFunction(favoredColor, game);
		return true;
	}

	ChessMoveArray legalMoveArray;
	GameResult result = game->GenerateAllLegalMovesForColor(whoseTurn, legalMoveArray);
	switch (result)
	{
		case GameResult::CheckMate:
		{
			score = (whoseTurn == favoredColor) ? -10000 : 10000;
			return true;
		}
		case GameResult::StaleMate:
		{
			score = -10000;
			return true;
		}
	}

	// TODO: Sort legal move array in a way that might promote alpha-beta pruning.

	score = 0;
	switch (goal)
	{
		case Goal::MINIMIZE:
		{
			score = INT_MAX;
			break;
		}
		case Goal::MAXIMIZE:
		{
			score = INT_MIN;
			break;
		}
		default:
		{
			return false;
		}
	}

	bool success = true;

	for (int i = 0; i < (signed)legalMoveArray.size(); i++)
	{
		ChessMove* legalMove = legalMoveArray[i];

		game->PushMove(legalMove);

		ChessColor otherColor = (whoseTurn == ChessColor::Black) ? ChessColor::White : ChessColor::Black;
		Goal opponentGoal = (goal == Goal::MAXIMIZE) ? Goal::MINIMIZE : Goal::MAXIMIZE;

		int subScore = 0;
		success = this->Minimax(opponentGoal, favoredColor, otherColor, game, depth + 1, subScore, &score);

		game->PopMove();

		if (!success)
			break;

		bool earlyOut = false;
		switch (goal)
		{
			case Goal::MINIMIZE:
			{
				if (score > subScore)
				{
					score = subScore;

					if (depth == 0)
						this->bestMove = legalMove;
					else if (score < *currentSuperScore)
						earlyOut = true;
				}

				break;
			}
			case Goal::MAXIMIZE:
			{
				if (score < subScore)
				{
					score = subScore;

					if (depth == 0)
						this->bestMove = legalMove;
					else if (score > *currentSuperScore)
						earlyOut = true;
				}

				break;
			}
		}

		if (depth == 0)
		{
			float percentage = float(i + 1) / float(legalMoveArray.size());
			if (!this->ProgressUpdate(percentage))
			{
				success = false;
				break;
			}
		}
		else if (earlyOut)	// This is the so-called "alpha-beta" prune case.
			break;
	}

	if (depth == 0 && success)
	{
		for (int i = 0; i < (signed)legalMoveArray.size(); i++)
			if (legalMoveArray[i] == this->bestMove)
				legalMoveArray[i] = nullptr;
	}

	DeleteMoveArray(legalMoveArray);

	return success;
}

/*virtual*/ int ChessMinimaxAI::EvaluationFunction(ChessColor favoredColor, const ChessGame* game)
{
	int totalScore = 0;

	// This is one possible way to boild the position down to a score indicating how well the given color is doing.
	// There are, of course, perhaps others, each taking their own verying degree of time to evaluate.  This one is
	// quick and simple.  It also makes clear the zero-sum nature of the game.
	for (int i = 0; i < CHESS_BOARD_FILES; i++)
	{
		for (int j = 0; j < CHESS_BOARD_RANKS; j++)
		{
			const ChessPiece* piece = game->GetSquareOccupant(ChessVector(i, j));
			if (piece)
			{
				int score = piece->GetScore();

				if (piece->color == favoredColor)
					totalScore += score;
				else
					totalScore -= score;
			}
		}
	}

	return totalScore;
}