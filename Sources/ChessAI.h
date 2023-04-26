#pragma once

#include "ChessCommon.h"

namespace ChessEngine
{
	class ChessGame;
	class ChessMove;

	class CHESS_ENGINE_API ChessAIProgressIndicator
	{
	public:
		ChessAIProgressIndicator();
		virtual ~ChessAIProgressIndicator();
		
		virtual bool ProgressUpdate(float alpha);
		virtual void ProgressBegin();
		virtual void ProgressEnd();
	};

	class CHESS_ENGINE_API ChessAI
	{
	public:
		ChessAI();
		virtual ~ChessAI();

		// The override can push moves onto the given game's move stack, but it must also pop them such
		// that by the end of the call, the stack is essentially left untouched.  This is a blocking
		// call, so it should be safe to manipulate the given game and then restore it before returning.
		virtual ChessMove* CalculateRecommendedMove(ChessColor favoredColor, ChessGame* game) = 0;

		// Derivatives of this class might implement a different evaluation function.
		virtual int EvaluationFunction(ChessColor favoredColor, const ChessGame* game);

		int Random(int min, int max);

		ChessAIProgressIndicator* progressIndicator;
	};

	// Useful resources:
	//		* https://medium.com/@SereneBiologist/the-anatomy-of-a-chess-ai-2087d0d565
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

	// Useful resources:
	//		* https://www.youtube.com/watch?v=UXW2yZndl7U
	//      * https://www.chessprogramming.org/Monte-Carlo_Tree_Search
	//		* https://en.wikipedia.org/wiki/Monte_Carlo_tree_search
	class CHESS_ENGINE_API ChessMonteCarloTreeSearchAI : public ChessAI
	{
	public:
		ChessMonteCarloTreeSearchAI(double maxTimeSeconds, int maxIterations);
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

	public:

		double maxTimeSeconds;
		int maxIterations;
	};
}