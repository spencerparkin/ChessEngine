#include "ChessApp.h"
#include "ChessFrame.h"
#include "ChessBot.h"
#include "ChessSound.h"
#include <wx/image.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include <fstream>

wxIMPLEMENT_APP(ChessApp);

ChessApp::ChessApp() : config("Chess")
{
	this->frame = nullptr;
	this->sound = nullptr;
	this->game = new ChessEngine::ChessGame();
	this->game->Reset();
	this->whoseTurn = ChessEngine::ChessColor::White;
	this->playerType[0] = PlayerType::HUMAN;
	this->playerType[1] = PlayerType::HUMAN;
	this->bot = new ChessMinimaxBot();
	dynamic_cast<ChessBotInterface*>(this->bot)->SetDifficulty(ChessBotInterface::Difficulty::MEDIUM);
}

/*virtual*/ ChessApp::~ChessApp()
{
	delete this->game;
	delete this->bot;
	delete this->sound;
}

/*virtual*/ bool ChessApp::OnInit(void)
{
	if (!wxApp::OnInit())
		return false;

	wxFileName fileName(wxStandardPaths::Get().GetTempDir() + "/last_game.chess");
	if (fileName.Exists())
		this->LoadGame(fileName.GetFullPath());

	// I'm getting some warnings when some square tile textures are loaded, complaining about
	// inconsistent chunks in the PNG file format, so I'm suppressing the warning dialogs with
	// this call.  The PNG files load just fine.
	wxLog::EnableLogging(false);

	wxInitAllImageHandlers();

	this->frame = new ChessFrame(nullptr, wxDefaultPosition, wxSize(1300, 700));

	wxFileName iconFile(wxGetCwd() + "/chess.bmp");
	if (iconFile.Exists())
	{
		wxString iconFilePath = iconFile.GetFullPath();
		wxImage iconImage;
		if (iconImage.LoadFile(iconFilePath, wxBITMAP_TYPE_BMP))
		{
			wxIcon icon;
			icon.CopyFromBitmap(iconImage);
			this->frame->SetIcon(icon);
		}
	}

	this->frame->Show();

#if __WXMSW__
	this->sound = new ChessSound_DirectSound();
#else
	this->sound = new ChessSound();
#endif
	if (!this->sound->Initialize())
	{
		wxMessageBox("Failed to initialize sound system.", "Error!", wxICON_ERROR | wxOK);
		// Don't let this be a fatal error.  Continue on...
	}

	return true;
}

/*virtual*/ int ChessApp::OnExit(void)
{
	this->sound->Shutdown();

	wxFileName fileName(wxStandardPaths::Get().GetTempDir() + "/last_game.chess");
	if (fileName.Exists())
		wxRemoveFile(fileName.GetFullPath());

	this->SaveGame(fileName.GetFullPath());
	return 0;
}

bool ChessApp::LoadGame(const wxString& gameFile)
{
	std::ifstream stream;
	stream.open((const char*)gameFile.c_str(), std::ios::binary | std::ios::in);
	if (!stream.is_open())
		return false;
	
	if (!this->game->ReadFromStream(stream))
	{
		this->game->Reset();
		return false;
	}

	char colorByte = 0;
	stream >> colorByte;
	stream.close();
	this->whoseTurn = (ChessEngine::ChessColor)colorByte;
	return true;
}

bool ChessApp::SaveGame(const wxString& gameFile)
{
	std::ofstream stream;
	stream.open((const char*)gameFile.c_str(), std::ios::binary | std::ios::out);
	if (!stream.is_open())
		return false;
	
	this->game->WriteToStream(stream);
	stream << (char)this->whoseTurn;
	stream.close();
	return true;
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