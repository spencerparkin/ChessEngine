#include "ChessSound.h"
#include "ChessApp.h"
#include "ChessFrame.h"

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

const char* ChessSound::GetAudioFileForEvent(Event event)
{
	switch (event)
	{
		case Event::PIECE_MOVED:				return "Sounds\\plop.wav";
		case Event::HUMAN_PIECE_CAPTURED:		return "Sounds\\the_pain.wav";
		case Event::HUMAN_PIECE_PROMOTED:		return "Sounds\\oh_yeah.wav";
		case Event::HUMAN_QUEEN_THREATENED:		return "Sounds\\sac_the_queen_brugh.wav";
		case Event::HUMAN_KING_CHECKED:			return "Sounds\\get_to_the_choppa.wav";
		case Event::HUMAN_KING_CHECKMATED:		return "";
		case Event::COMPUTER_PIECE_CAPTURED:	return "";
		case Event::COMPUTER_PIECE_PROMOTED:	return "";
		case Event::COMPUTER_QUEEN_THREATENED:	return "";
		case Event::COMPUTER_KING_CHECKED:		return "";
		case Event::COMPUTER_KING_CHECKMATED:	return "Sounds\\he_is_the_quizzats_haderact.wav";
	}

	return nullptr;
}

bool ChessSound::PlaySoundEventForGameState(ChessEngine::ChessGame* game)
{
	/*ChessEngine::ChessMoveArray moveArray;

	ChessEngine::GameResult whiteResult = game->GenerateAllLegalMovesForColor(ChessEngine::ChessColor::White, moveArray);
	ChessEngine::DeleteMoveArray(moveArray);

	ChessEngine::GameResult blackResult = game->GenerateAllLegalMovesForColor(ChessEngine::ChessColor::Black, moveArray);
	ChessEngine::DeleteMoveArray(moveArray);*/

	return false;
}

//------------------------------- ChessSound_DirectSound -------------------------------

#if defined __WXMSW__

ChessSound_DirectSound::ChessSound_DirectSound()
{
	this->device = nullptr;
	this->sfxMap = new SoundEffectMap;
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

	return true;
}

#endif //__WXMSW__