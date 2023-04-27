#include "ChessAI.h"
#include "ChessGame.h"
#include "ChessPiece.h"
#include "ChessMove.h"
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

using namespace ChessEngine;

//---------------------------------------- ProgressIndicator ----------------------------------------

ChessAIProgressIndicator::ChessAIProgressIndicator()
{
}

/*virtual*/ ChessAIProgressIndicator::~ChessAIProgressIndicator()
{
}

/*virtual*/ bool ChessAIProgressIndicator::ProgressUpdate(float alpha)
{
	return true;
}

/*virtual*/ void ChessAIProgressIndicator::ProgressBegin()
{
}

/*virtual*/ void ChessAIProgressIndicator::ProgressEnd()
{
}

//---------------------------------------- ChessAI ----------------------------------------

ChessAI::ChessAI()
{
	this->progressIndicator = nullptr;
}

/*virtual*/ ChessAI::~ChessAI()
{
}

/*virtual*/ int ChessAI::EvaluationFunction(ChessColor favoredColor, const ChessGame* game)
{
	int totalScore = 0;

	// This is one possible way to boil the position down to a score indicating how well the given color is doing.
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

				// Bonus points for being closer to the center of the board.
				score += piece->location.ShortestDistanceToBoardEdge();

				if (piece->color == favoredColor)
					totalScore += score;
				else
					totalScore -= score;
			}
		}
	}

	return totalScore;
}

//---------------------------------------- ChessMinimaxAI ----------------------------------------

ChessMinimaxAI::ChessMinimaxAI(int maxDepth)
{
	this->bestMoveArray = new ChessMoveArray();
	this->maxDepth = maxDepth;
	std::srand((unsigned int)time(nullptr));
}

/*virtual*/ ChessMinimaxAI::~ChessMinimaxAI()
{
	delete this->bestMoveArray;
}

/*virtual*/ ChessMove* ChessMinimaxAI::CalculateRecommendedMove(ChessColor favoredColor, ChessGame* game)
{
	ChessMove* chosenMove = nullptr;

	if(this->progressIndicator)
		this->progressIndicator->ProgressBegin();

	this->bestMoveArray->clear();

	int numMoves = game->GetNumMoves();

	int score = 0;
	bool success = this->Minimax(Goal::MAXIMIZE, favoredColor, favoredColor, game, 0, score);

	assert(numMoves == game->GetNumMoves());

	if (success && this->bestMoveArray->size() > 0)
	{
		int i = Random(0, this->bestMoveArray->size() - 1);
		chosenMove = (*this->bestMoveArray)[i];
		(*this->bestMoveArray)[i] = nullptr;
		DeleteMoveArray(*this->bestMoveArray);
	}

	if (this->progressIndicator)
		this->progressIndicator->ProgressEnd();

	return chosenMove;
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

	// We sort the legal moves in order of possibly best to worst.  This may improve the alpha-beta pruning.
	std::sort(legalMoveArray.begin(), legalMoveArray.end(), [&legalMoveArray](const ChessMove* moveA, const ChessMove* moveB) -> bool {
		return moveA->GetSortKey() > moveB->GetSortKey();
	});

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

		if ((goal == Goal::MINIMIZE && score > subScore) || (goal == Goal::MAXIMIZE && score < subScore))
		{
			score = subScore;

			if (depth == 0)
			{
				this->bestMoveArray->clear();
				this->bestMoveArray->push_back(legalMove);
			}
			else if ((goal == Goal::MINIMIZE && score < *currentSuperScore) || (goal == Goal::MAXIMIZE && score > *currentSuperScore))
			{
				// This is the so-called "alpha-beta" prune case.
				break;
			}
		}
		else if (depth == 0 && score == subScore)
		{
			this->bestMoveArray->push_back(legalMove);
		}

		if (depth == 0 && this->progressIndicator)
		{
			float percentage = float(i + 1) / float(legalMoveArray.size());
			if (!this->progressIndicator->ProgressUpdate(percentage))
			{
				success = false;
				break;
			}
		}
	}

	if (depth == 0)
	{
		if (!success)
			this->bestMoveArray->clear();
		else
		{
			for (int i = 0; i < (signed)legalMoveArray.size(); i++)
			{
				for (int j = 0; j < (signed)this->bestMoveArray->size(); j++)
				{
					if (legalMoveArray[i] == (*this->bestMoveArray)[j])
					{
						legalMoveArray[i] = nullptr;
						break;
					}
				}
			}
		}
	}

	DeleteMoveArray(legalMoveArray);

	return success;
}

//---------------------------------------- ChessMontoCarloTreeSearchAI ----------------------------------------

ChessMonteCarloTreeSearchAI::ChessMonteCarloTreeSearchAI(double maxTimeSeconds, int maxIterations)
{
	this->maxTimeSeconds = maxTimeSeconds;
	this->maxIterations = maxIterations;
	this->numGamesPerRollout = 16;
	this->numRolloutThreads = 8;
	this->rolloutThreadArray = new RolloutThreadArray();
}

/*virtual*/ ChessMonteCarloTreeSearchAI::~ChessMonteCarloTreeSearchAI()
{
	delete this->rolloutThreadArray;
}

/*virtual*/ ChessMove* ChessMonteCarloTreeSearchAI::CalculateRecommendedMove(ChessColor favoredColor, ChessGame* game)
{
	if (this->progressIndicator)
		this->progressIndicator->ProgressBegin();

	// Spin up a thread pool we can use to accelerate the processing of roll-outs.
	for (int i = 0; i < this->numRolloutThreads; i++)
	{
		RolloutThread* thread = new RolloutThread();
		thread->SpawnThread();
		this->rolloutThreadArray->push_back(thread);
	}

	Node* root = new Node(nullptr, nullptr);

	clock_t startTimeTicks = ::clock();
	int iterationCount = 0;

	while (true)
	{
		if (this->maxIterations > 0)
		{
			iterationCount++;
			if (this->progressIndicator)
				this->progressIndicator->ProgressUpdate(float(iterationCount) / float(this->maxIterations));
			if (iterationCount >= this->maxIterations)
				break;
		}
		else if (this->maxTimeSeconds > 0.0)
		{
			clock_t currentTimeTicks = ::clock();
			clock_t elapsedTimeTicks = currentTimeTicks - startTimeTicks;
			double elapsedTimeSeconds = double(elapsedTimeTicks) / double(CLOCKS_PER_SEC);
			if (elapsedTimeSeconds > this->maxTimeSeconds)
				break;
			if (this->progressIndicator)
				this->progressIndicator->ProgressUpdate(float(elapsedTimeSeconds) / float(this->maxTimeSeconds));
		}
		else
		{
			break;
		}

		int originalNumMoves = game->GetNumMoves();
		ChessColor whoseTurn = favoredColor;

		//
		// SELECTION PHASE
		//

		Node* selectedNode = root;
		while (selectedNode->childArray.size() > 0)
		{
			Node* nextNode = nullptr;
			double highestUCB = -DBL_MAX;
			for (Node* child : selectedNode->childArray)
			{
				double childUCB = child->CalcUCB();
				if (childUCB > highestUCB)
				{
					highestUCB = childUCB;
					nextNode = child;
				}
			}

			assert(nextNode != nullptr);
			selectedNode = nextNode;
			game->PushMove(nextNode->move);
			whoseTurn = (whoseTurn == ChessColor::Black) ? ChessColor::White : ChessColor::Black;
		}

		//
		// EXPANSION PHASE
		//

		if (selectedNode->numVisits > 0.0 || selectedNode == root)
		{
			ChessMoveArray moveArray;
			game->GenerateAllLegalMovesForColor(whoseTurn, moveArray);
			if (moveArray.size() > 0)
			{
				for (ChessMove* move : moveArray)
				{
					Node* node = new Node(move, selectedNode);
					selectedNode->childArray.push_back(node);
				}

				selectedNode = selectedNode->childArray.back();
				game->PushMove(selectedNode->move);
				whoseTurn = (whoseTurn == ChessColor::Black) ? ChessColor::White : ChessColor::Black;
			}
		}
		
		//
		// ROLLOUT PHASE
		//

		double rolloutScore = this->PerformRollout(favoredColor, whoseTurn, game);

		//
		// BACKPROPAGATION PHASE
		//

		for(Node* node = selectedNode; node; node = node->parent)
		{
			node->totalScore += rolloutScore;
			node->numVisits++;
			node->cachedUCBValid = false;
			if (node != root)
				game->PopMove();
		}

		// Make sure we leave the game state untouched after each iteration.
		assert(game->GetNumMoves() == originalNumMoves);
	}

	// Finally, choose the move from the root with the highest total score.
	ChessMove* bestMove = nullptr;
	double highestTotalScore = -DBL_MAX;
	for (Node* child : root->childArray)
	{
		if (child->totalScore > highestTotalScore)
		{
			bestMove = child->move;
			highestTotalScore = child->totalScore;
		}
	}

	for (Node* child : root->childArray)
		if (child->move == bestMove)
			child->move = nullptr;

	delete root;

	for (RolloutThread* thread : *this->rolloutThreadArray)
	{
		thread->SignalShutdown();
		thread->WaitForThreadExit();
		delete thread;
	}

	this->rolloutThreadArray->clear();

	if (this->progressIndicator)
		this->progressIndicator->ProgressEnd();

	return bestMove;
}

// MCTS has some re-enforcement learning built into it, but the main principle upon which it is built is
// the law of large numbers.  Specifically, the more random samples we take of a board position (in the form
// of playing random games from it all the way to the very end), the more accurate becomes our view of how
// good or bad that particular board position is.  This is how we choose our move.  We want to move to the most
// advantageous board position.  Note also that this main principle replaces the evaluation function used in
// mini-max.  It's much slower in practice, but it's better in theory, because it captures, by experience, all
// the subtlies and nuonces of the game.  We could possibly do better here if we could draw from a database of
// board positions or maybe a trained nueral network or something like that, but I don't know much about either.
// Anyhow, a tree is built during the MCTS algorithm, but in theory, you don't even need that if you were able to
// play, say, an infinite number of random games from each possible board position of each possible move you're
// considering, then you would just know which one is the best.  The tree, however, can help us take more samples
// where there's more promise, and the UCB stuff can help us keep exploring so that we don't overlook other areas
// of the game tree.  Anyhow, that's my current understanding of all this.
double ChessMonteCarloTreeSearchAI::PerformRollout(ChessColor favoredColor, ChessColor whoseTurn, ChessGame* game)
{
	assert(this->numGamesPerRollout > 0);

	double gameResultsTotal = 0.0;
	int numGamesPlayed = 0;
	Mutex gameResultMutex;
	std::function<void(double)> aggregateGameResultFunc = [&](double gameResultValue) {
		MutexLocker locker(gameResultMutex);
		gameResultsTotal += gameResultValue;
		numGamesPlayed++;
	};

	// Evenly distribute the workload of performing all the games per roll-out.
	// Each thread gets a copy of the game, so there is no need to unwind the move stack after each game.
	std::vector<Event*> completionEventArray;
	for (int i = 0; i < this->numGamesPerRollout; i++)
	{
		int j = i % this->rolloutThreadArray->size();
		RolloutThread* thread = (*this->rolloutThreadArray)[j];
		Event* completionEvent = new Event();
		RolloutThread::Work work{ favoredColor, whoseTurn, game->Clone(), aggregateGameResultFunc, completionEvent };
		thread->EnqueueRandomGame(work);
		completionEventArray.push_back(completionEvent);
	}

	Event::WaitMultiple(completionEventArray);

	for (Event* completionEvent : completionEventArray)
		delete completionEvent;

	return gameResultsTotal / double(this->numGamesPerRollout);
}

//---------------------------------------- ChessMontoCarloTreeSearchAI::Node ----------------------------------------	
	
ChessMonteCarloTreeSearchAI::Node::Node(ChessMove* move, Node* parent)
{
	this->move = move;
	this->parent = parent;
	this->totalScore = 0.0;
	this->numVisits = 0.0;
	this->cachedUCB = DBL_MAX;
	this->cachedUCBValid = true;
}

/*virtual*/ ChessMonteCarloTreeSearchAI::Node::~Node()
{
	delete this->move;

	for (Node* child : this->childArray)
		delete child;
}

double ChessMonteCarloTreeSearchAI::Node::CalcUCB() const
{
	if (!this->cachedUCBValid)
	{
		if (this->numVisits == 0.0 || !this->parent)
			this->cachedUCB = DBL_MAX;
		else
		{
			static double C = 2.0;
			double exploitationTerm = this->totalScore / this->numVisits;
			double explorationTerm = C * ::sqrt(::log(this->parent->numVisits) / this->numVisits);
			this->cachedUCB = explorationTerm + exploitationTerm;
		}

		this->cachedUCBValid = true;
	}

	return this->cachedUCB;
}

//---------------------------------------- ChessMontoCarloTreeSearchAI::RolloutThread ----------------------------------------	

ChessMonteCarloTreeSearchAI::RolloutThread::RolloutThread() : workQueueSem(128)
{
}

/*virtual*/ ChessMonteCarloTreeSearchAI::RolloutThread::~RolloutThread()
{
}

void ChessMonteCarloTreeSearchAI::RolloutThread::EnqueueRandomGame(const Work& work)
{
	this->workQueue.AddTail(work);
	this->workQueueSem.Increment();
}

void ChessMonteCarloTreeSearchAI::RolloutThread::SignalShutdown()
{
	Work work{ ChessColor::Black, ChessColor::Black, nullptr, [](double) {} };
	this->workQueue.AddTail(work);
	this->workQueueSem.Increment();
}

/*virtual*/ int ChessMonteCarloTreeSearchAI::RolloutThread::ThreadFunc()
{
	while (true)
	{
		this->workQueueSem.Decrement();
		
		Work work;
		if (!this->workQueue.RemoveHead(work) || !work.game)
			break;

		double gameResultValue = 0.0;
		while (true)
		{
			ChessMoveArray moveArray;
			GameResult result = work.game->GenerateAllLegalMovesForColor(work.whoseTurn, moveArray);

			// Have we reached the end of the game?
			if (result == GameResult::CheckMate)
			{
				gameResultValue = (work.whoseTurn == work.favoredColor) ? -1.0 : 1.0;
				DeleteMoveArray(moveArray);
				break;
			}
			else if (result == GameResult::StaleMate || work.game->GetNumPiecesOnBoard() <= 2)
			{
				gameResultValue = 0.0;
				DeleteMoveArray(moveArray);
				break;
			}

			// Pick a random move and go with it.
			int i = Random(0, moveArray.size() - 1);
			ChessMove* move = moveArray[i];
			moveArray[i] = moveArray[moveArray.size() - 1];
			moveArray.pop_back();
			DeleteMoveArray(moveArray);
			work.game->PushMove(move);
			work.whoseTurn = (work.whoseTurn == ChessColor::Black) ? ChessColor::White : ChessColor::Black;
		}

		work.aggregateGameResultFunc(gameResultValue);
		work.completionEvent->Signal();
	}

	return 0;
}