// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#ifndef AUDIO_STARTUP_H
#define AUDIO_STARTUP_H
#include "audio_test.h"

unsigned getSoundCount( const string& soundName );
void setSoundCount( const string& soundName, unsigned count );
void setMelodyMute(bool muted);
bool getMelodyMute();
void setVolumeMute(bool muted);
bool getVolumeMute();
void setRepeatChannel(int nChannelId);
int getRepeatChannel();
bool isVisitedPause();
void setVisitedPause(bool visitedPause);
void GatherSounds( const string& strFileName, Audio_Engine& AEngine );
Audio_Engine* GetAudioEngine(); //TODO: Implement this!
void StartGameAudioSystem();

#endif