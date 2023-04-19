#include "ChessApp.h"
#include "ChessFrame.h"
#include "ChessBot.h"
#include <wx/image.h>
#include <wx/filename.h>
#include <fstream>

wxIMPLEMENT_APP(ChessApp);

ChessApp::ChessApp() : config("Chess")
{
	this->frame = nullptr;
	this->game = new ChessEngine::ChessGame();
	this->game->Reset();
	this->whoseTurn = ChessEngine::ChessColor::White;
	this->playerType[0] = PlayerType::HUMAN;
	this->playerType[1] = PlayerType::HUMAN;
	this->bot = new ChessBot(COMPUTER_MEDIUM_MAX_DEPTH);
}

/*virtual*/ ChessApp::~ChessApp()
{
	delete this->game;
	delete this->bot;
}

/*virtual*/ bool ChessApp::OnInit(void)
{
	if (!wxApp::OnInit())
		return false;

	wxFileName fileName(wxGetCwd() + "/last_game.chess");
	if (fileName.Exists())
	{
		std::ifstream stream;
		stream.open((const char*)fileName.GetFullPath().c_str(), std::ios::binary | std::ios::in);
		if (stream.is_open())
		{
			if (!this->game->ReadFromStream(stream))
				this->game->Reset();

			stream.close();
		}
	}

	// I'm getting some warnings when some square tile textures are loaded, complaining about
	// inconsistent chunks in the PNG file format, so I'm suppressing the warning dialogs with
	// this call.  The PNG files load just fine.
	wxLog::EnableLogging(false);

	wxInitAllImageHandlers();

	this->frame = new ChessFrame(nullptr, wxDefaultPosition, wxSize(1300, 700));
	this->frame->Show();

	return true;
}

/*virtual*/ int ChessApp::OnExit(void)
{
	wxFileName fileName(wxGetCwd() + "/last_game.chess");
	if (fileName.Exists())
		wxRemoveFile(fileName.GetFullPath());

	std::ofstream stream;
	stream.open((const char*)fileName.GetFullPath().c_str(), std::ios::binary | std::ios::out);
	if (stream.is_open())
	{
		this->game->WriteToStream(stream);
		stream.close();
	}

	return 0;
}

void ChessApp::FlipTurn()
{
	if (this->whoseTurn == ChessEngine::ChessColor::White)
		this->whoseTurn = ChessEngine::ChessColor::Black;
	else
		this->whoseTurn = ChessEngine::ChessColor::White;
}

ChessApp::PlayerType ChessApp::GetCurrentPlayerType()
{
	return this->GetPlayerType(this->whoseTurn);
}

void ChessApp::SetCurrentPlayerType(PlayerType playerType)
{
	this->SetPlayerType(this->whoseTurn, playerType);
}

ChessApp::PlayerType ChessApp::GetPlayerType(ChessEngine::ChessColor color)
{
	return this->playerType[int(color)];
}

void ChessApp::SetPlayerType(ChessEngine::ChessColor color, PlayerType playerType)
{
	this->playerType[int(color)] = playerType;
}