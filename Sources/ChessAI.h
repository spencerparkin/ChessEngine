#pragma once

#include "ChessCommon.h"

namespace ChessEngine
{
	class ChessGame;
	class ChessMove;

	class CHESS_ENGINE_API ChessAI
	{
	public:
		ChessAI();
		virtual ~ChessAI();

		// The override can push moves onto the given game's move stack, but it must also pop them such
		// that by the end of the call, the stack is essentially left untouched.  This is a blocking
		// call, so it should be safe to manipulate the given game and then restore it before returning.
		virtual ChessMove* CalculateRecommendedMove(ChessColor favoredColor, ChessGame* game) = 0;

		// While thinking, the class can call this periodically to let the user know how far along
		// it is in the process.  If this call returns false, thinking should stop and the entire
		// process should be canceled.  This is meant to be overridden by users of the engine, not
		// implementors of various chess solvers.
		virtual bool ProgressUpdate(float percentage);

		// Derivatives of this class might implement a different evaluation function.
		virtual int EvaluationFunction(ChessColor favoredColor, const ChessGame* game);

		// These serve a similar purpose.
		virtual void ProgressBegin();
		virtual void ProgressEnd();

		int Random(int min, int max);
	};

	// Useful resource: https://medium.com/@SereneBiologist/the-anatomy-of-a-chess-ai-2087d0d565
	class CHESS_ENGINE_API ChessMinimaxAI : public ChessAI
	{
	public:
		ChessMinimaxAI(int maxDepth);
		virtual ~ChessMinimaxAI();

		virtual ChessMove* CalculateRecommendedMove(ChessColor favoredColor, ChessGame* game) override;

		enum class Goal
		{
			MINIMIZE,
			MAXIMIZE
		};

		bool Minimax(Goal goal, ChessColor favoredColor, ChessColor whoseTurn, ChessGame* game, int depth, int& score, int* currentSuperScore = nullptr);

		ChessMoveArray* bestMoveArray;
		int maxDepth;
	};

	// Useful resource: https://www.youtube.com/watch?v=UXW2yZndl7U
	class CHESS_ENGINE_API ChessMonteCarloTreeSearchAI : public ChessAI
	{
	public:
		ChessMonteCarloTreeSearchAI(time_t maxTimeSeconds, int maxIterations);
		virtual ~ChessMonteCarloTreeSearchAI();

		virtual ChessMove* CalculateRecommendedMove(ChessColor favoredColor, ChessGame* game) override;

	private:

		double PerformRollout(ChessColor favoredColor, ChessColor whoseTurn, ChessGame* game);

		class Node
		{
		public:
			Node(ChessMove* move, Node* parent);
			virtual ~Node();

			double CalcUCB() const;

			Node* parent;
			std::vector<Node*> childArray;
			ChessMove* move;
			double totalScore;
			double numVisits;
			mutable double cachedUCB;
			mutable bool cachedUCBValid;
		};

		time_t maxTimeSeconds;
		int maxIterations;
	};
}