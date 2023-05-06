#include "ChessSound.h"
#include "ChessApp.h"
#include "ChessFrame.h"
#include "ChessMove.h"
#include "ChessPiece.h"

//------------------------------- ChessSound -------------------------------

ChessSound::ChessSound()
{
	this->enabled = false;
}

/*virtual*/ ChessSound::~ChessSound()
{
}

/*virtual*/ bool ChessSound::Initialize()
{
	return true;
}

/*virtual*/ bool ChessSound::Shutdown()
{
	return true;
}

/*virtual*/ bool ChessSound::PlaySoundEvent(Event event)
{
	return false;
}

/*virtual*/ bool ChessSound::SoundStillPlaying()
{
	return false;
}

// TODO: Add sound for castling and stale-mate and maybe a few other cases.
const char* ChessSound::GetAudioFileForEvent(Event event)
{
	switch (event)
	{
		case Event::PIECE_MOVED:				return "Sounds\\plop.wav";
		case Event::HUMAN_PIECE_CAPTURED:		return "Sounds\\the_pain.wav";
		case Event::HUMAN_PIECE_PROMOTED:		return "Sounds\\oh_yeah.wav";
		case Event::HUMAN_QUEEN_THREATENED:		return "Sounds\\sac_the_queen_brugh.wav";
		case Event::HUMAN_KING_CHECKED:			return "Sounds\\get_to_the_choppa.wav";
		case Event::HUMAN_KING_CHECKMATED:		return "Sounds\\better_luck_next_time.wav";
		case Event::COMPUTER_PIECE_CAPTURED:	return "Sounds\\fatality.wav";
		case Event::COMPUTER_PIECE_PROMOTED:	return "Sounds\\uh_oh.wav";
		case Event::COMPUTER_QUEEN_THREATENED:	return "Sounds\\that_queen_looks_pretty_hot.wav";
		case Event::COMPUTER_KING_CHECKED:		return "Sounds\\oh_yeah.wav";
		case Event::COMPUTER_KING_CHECKMATED:	return "Sounds\\he_is_the_quizzats_haderact.wav";
	}

	return nullptr;
}

bool ChessSound::PlaySoundEventForGameState(ChessEngine::ChessGame* game)
{
	if (!this->enabled)
		return false;

	Event event = Event::NUM_EVENTS;

	while (true)
	{
		ChessEngine::ChessColor whoseTurn = wxGetApp().whoseTurn;
		ChessEngine::ChessColor otherPlayer = (whoseTurn == ChessEngine::ChessColor::White) ? ChessEngine::ChessColor::Black : ChessEngine::ChessColor::White;

		ChessEngine::ChessMoveArray moveArray;
		ChessEngine::GameResult gameResult = game->GenerateAllLegalMovesForColor(whoseTurn, moveArray);

		if (gameResult == ChessEngine::GameResult::Check)
		{
			switch (wxGetApp().playerType[int(whoseTurn)])
			{
				case ChessApp::PlayerType::HUMAN:		event = Event::HUMAN_KING_CHECKED;		break;
				case ChessApp::PlayerType::COMPUTER:	event = Event::COMPUTER_KING_CHECKED;	break;
			}
		}
		else if (gameResult == ChessEngine::GameResult::CheckMate)
		{
			switch (wxGetApp().playerType[int(whoseTurn)])
			{
				case ChessApp::PlayerType::HUMAN:		event = Event::HUMAN_KING_CHECKMATED;		break;
				case ChessApp::PlayerType::COMPUTER:	event = Event::COMPUTER_KING_CHECKMATED;	break;
			}
		}

		ChessEngine::DeleteMoveArray(moveArray);

		if (event != Event::NUM_EVENTS)
			break;

		gameResult = game->GenerateAllLegalMovesForColor(otherPlayer, moveArray);

		for (ChessEngine::ChessMove* move : moveArray)
		{
			ChessEngine::Capture* capture = dynamic_cast<ChessEngine::Capture*>(move);
			if (capture)
			{
				ChessEngine::ChessPiece* piece = game->GetSquareOccupant(capture->destinationLocation);
				ChessEngine::Queen* queen = dynamic_cast<ChessEngine::Queen*>(piece);
				if (queen && queen->color == whoseTurn)
				{
					switch (wxGetApp().playerType[int(whoseTurn)])
					{
						case ChessApp::PlayerType::HUMAN:		event = Event::HUMAN_QUEEN_THREATENED;		break;
						case ChessApp::PlayerType::COMPUTER:	event = Event::COMPUTER_QUEEN_THREATENED;	break;
					}

					break;
				}
			}
		}

		ChessEngine::DeleteMoveArray(moveArray);

		if (event != Event::NUM_EVENTS)
			break;

		if (game->GetNumMoves() > 0)
		{
			const ChessEngine::ChessMove* move = game->GetMove(game->GetNumMoves() - 1);

			const ChessEngine::Travel* travel = dynamic_cast<const ChessEngine::Travel*>(move);
			if (travel)
			{
				event = Event::PIECE_MOVED;
				break;
			}

			const ChessEngine::Promotion* promotion = dynamic_cast<const ChessEngine::Promotion*>(move);
			const ChessEngine::CapturePromotion* capturePromotion = dynamic_cast<const ChessEngine::CapturePromotion*>(move);
			if (promotion || capturePromotion)
			{
				switch (wxGetApp().playerType[int(otherPlayer)])
				{
					case ChessApp::PlayerType::HUMAN:		event = Event::HUMAN_PIECE_PROMOTED;	break;
					case ChessApp::PlayerType::COMPUTER:	event = Event::COMPUTER_PIECE_PROMOTED;	break;
				}

				break;
			}

			const ChessEngine::Capture* capture = dynamic_cast<const ChessEngine::Capture*>(move);
			if (capture)
			{
				switch (wxGetApp().playerType[int(capture->capturedPiece->color)])
				{
					case ChessApp::PlayerType::HUMAN:		event = Event::HUMAN_PIECE_CAPTURED;	break;
					case ChessApp::PlayerType::COMPUTER:	event = Event::COMPUTER_PIECE_CAPTURED;	break;
				}

				break;
			}
		}

		break;
	}

	return this->PlaySoundEvent(event);
}

//------------------------------- ChessSound_DirectSound -------------------------------

#if defined __WXMSW__

ChessSound_DirectSound::ChessSound_DirectSound()
{
	this->device = nullptr;
	this->sfxMap = new SoundEffectMap;
	this->lastPlayedEvent = Event::NUM_EVENTS;
}

/*virtual*/ ChessSound_DirectSound::~ChessSound_DirectSound()
{
	delete this->sfxMap;
}

/*virtual*/ bool ChessSound_DirectSound::Initialize()
{
	bool success = false;
	HRESULT result = 0;
	
	while (true)
	{
		// TODO: Enumerate devices and choose best one?
		result = ::DirectSoundCreate8(&DSDEVID_DefaultPlayback, &this->device, nullptr);
		if (result != DS_OK)
			break;

		result = this->device->SetCooperativeLevel(wxGetApp().frame->GetHWND(), DSSCL_PRIORITY);
		if (result != DS_OK)
			break;

		int i;
		for (i = 0; i < (int)Event::NUM_EVENTS; i++)
		{
			std::string audioFilePath(this->GetAudioFileForEvent((Event)i));
			AudioFile<int> audioFile;
			if (!audioFile.load(audioFilePath))
				break;

			if (!audioFile.isStereo())
				break;

			SoundEffect* sfx = new SoundEffect;
			sfx->buffer = nullptr;
			this->sfxMap->insert(std::pair<Event, SoundEffect*>((Event)i, sfx));

			WAVEFORMATEX waveFormat;
			waveFormat.wFormatTag = WAVE_FORMAT_PCM;
			waveFormat.nChannels = audioFile.getNumChannels();
			waveFormat.nSamplesPerSec = audioFile.getSampleRate();
			waveFormat.nBlockAlign = audioFile.getNumChannels() * audioFile.getBitDepth() / 8;
			waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
			waveFormat.wBitsPerSample = audioFile.getBitDepth();
			waveFormat.cbSize = 0;

			DSBUFFERDESC bufDesc;
			bufDesc.dwSize = sizeof(DSBUFFERDESC);
			bufDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
			bufDesc.dwBufferBytes = audioFile.getNumChannels() * audioFile.getNumSamplesPerChannel() * audioFile.getBitDepth() / 8;
			bufDesc.dwReserved = 0;
			bufDesc.lpwfxFormat = &waveFormat;
			bufDesc.guid3DAlgorithm = DS3DALG_DEFAULT;

			result = this->device->CreateSoundBuffer(&bufDesc, &sfx->buffer, nullptr);
			if (result != DS_OK)
				break;

			void* audioBuf1 = nullptr;
			void* audioBuf2 = nullptr;
			DWORD audioBuf1Size = 0;
			DWORD audioBuf2Size = 0;

			result = sfx->buffer->Lock(0, 0, &audioBuf1, &audioBuf1Size, &audioBuf2, &audioBuf2Size, DSBLOCK_ENTIREBUFFER);
			if (result != DS_OK)
				break;

			if (audioBuf1Size != audioFile.samples[0].size() * sizeof(int))
				break;

			// TODO: Does this make sense?  And what about the other channel?
			::memcpy(audioBuf1, audioFile.samples[0].data(), audioBuf1Size);

			result = sfx->buffer->Unlock(audioBuf1, audioBuf1Size, audioBuf2, audioBuf2Size);
			if (result != DS_OK)
				break;
		}

		if (i < (int)Event::NUM_EVENTS)
			break;

		success = true;
		break;
	}

	if (!success)
		this->Shutdown();

	return success;
}

/*virtual*/ bool ChessSound_DirectSound::Shutdown()
{
	for(SoundEffectMap::iterator iter = this->sfxMap->begin(); iter != this->sfxMap->end(); iter++)
	{
		SoundEffect* sfx = iter->second;
		if (sfx->buffer)
			sfx->buffer->Release();
		delete sfx;
	}

	this->sfxMap->clear();

	if (this->device)
	{
		this->device->Release();
		this->device = nullptr;
	}

	return true;
}

/*virtual*/ bool ChessSound_DirectSound::PlaySoundEvent(Event event)
{
	if (!this->enabled)
		return false;

	SoundEffectMap::iterator iter = this->sfxMap->find(event);
	if (iter == this->sfxMap->end())
		return false;

	SoundEffect* sfx = iter->second;
	if (!sfx->buffer)
		return false;

	HRESULT result = sfx->buffer->Play(0, 0, 0);
	if (result != DS_OK)
		return false;

	this->lastPlayedEvent = event;
	return true;
}

/*virtual*/ bool ChessSound_DirectSound::SoundStillPlaying()
{
	SoundEffectMap::iterator iter = this->sfxMap->find(this->lastPlayedEvent);
	if (iter == this->sfxMap->end())
		return false;

	SoundEffect* sfx = iter->second;
	if (!sfx->buffer)
		return false;

	DWORD status = 0;
	HRESULT result = sfx->buffer->GetStatus(&status);
	if (result != DS_OK)
		return false;

	return (status == DSBSTATUS_PLAYING);
}

#endif //__WXMSW__