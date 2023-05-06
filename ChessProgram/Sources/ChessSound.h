#pragma once

#include "ChessGame.h"
#include "AudioFile/AudioFile.h"
#include <map>
#if defined __WXMSW__
#	include <dsound.h>
#endif //__WXMSW__

class ChessSound
{
public:
	ChessSound();
	virtual ~ChessSound();

	enum class Event
	{
		PIECE_MOVED,
		HUMAN_PIECE_CAPTURED,
		HUMAN_PIECE_PROMOTED,
		HUMAN_QUEEN_THREATENED,
		HUMAN_KING_CHECKED,
		HUMAN_KING_CHECKMATED,
		COMPUTER_PIECE_CAPTURED,
		COMPUTER_PIECE_PROMOTED,
		COMPUTER_QUEEN_THREATENED,
		COMPUTER_KING_CHECKED,
		COMPUTER_KING_CHECKMATED,
		NUM_EVENTS
	};

	virtual bool Initialize();
	virtual bool Shutdown();
	virtual bool PlaySoundEvent(Event event);
	virtual bool SoundStillPlaying();

	bool PlaySoundEventForGameState(ChessEngine::ChessGame* game);

	bool enabled;

protected:

	const char* GetAudioFileForEvent(Event event);
};

#if defined __WXMSW__

class ChessSound_DirectSound : public ChessSound
{
public:
	ChessSound_DirectSound();
	virtual ~ChessSound_DirectSound();

	virtual bool Initialize() override;
	virtual bool Shutdown() override;
	virtual bool PlaySoundEvent(Event event) override;
	virtual bool SoundStillPlaying() override;

private:
	IDirectSound8* device;

	struct SoundEffect
	{
		IDirectSoundBuffer* buffer;
	};

	typedef std::map<Event, SoundEffect*> SoundEffectMap;
	SoundEffectMap* sfxMap;

	Event lastPlayedEvent;
};

#endif //__WXMSW__