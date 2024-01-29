// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include "../include/audio_startup.h"
#include "../include/audio_test.h"
#include "../include/Logger.h"
#include <fstream>
#include "temp_utils.hpp"

using namespace Logger;
static Audio_Engine* AEngine = NULL;
static bool visitedPauseMenu = false;
static bool visitedLowHealth = false;
static int repeatChannel = -1;
static bool isMute = false; //used for global mute
static bool melodyMute = false;
static std::map<std::string, unsigned> soundCount;

unsigned getSoundCount( const string& soundName )
{
  return soundCount[soundName];
}
void setSoundCount( const string& soundName, unsigned count )
{
  soundCount[soundName] = count;
}

void setMelodyMute(bool muted)
{
  melodyMute = muted;
  int nChannelId = GetAudioEngine()->FindSoundChannel("menumelody_repeat.wav");
  int nChannelId2 = GetAudioEngine()->FindSoundChannel("startup.wav");
  // We don't need to set the mute if it's already in place.
  if (!isMute)
  {
    GetAudioEngine()->SetChannelMute(nChannelId, melodyMute);
    GetAudioEngine()->SetChannelMute(nChannelId2, melodyMute);
  }
}
bool getMelodyMute()
{
  return melodyMute;
}

void setVolumeMute(bool muted)
{
  isMute = muted;
}

bool getVolumeMute()
{
  return isMute;
}

void setRepeatChannel(int nChannelId)
{
  repeatChannel = nChannelId;
}

int getRepeatChannel()
{
  return repeatChannel;
}

bool isVisitedLowHealth()
{
  return visitedLowHealth;
}

void setVisitedLowHealth( bool result )
{
  visitedLowHealth = result;
}

bool isVisitedPause()
{
  return visitedPauseMenu;
}

void setVisitedPause( bool visitedPause )
{
  visitedPauseMenu = visitedPause;
}

Audio_Engine* GetAudioEngine()
{
  return AEngine;
}

/*! \brief
 * Initializes the audio system and reads in all sounds in the List.
 */
void StartGameAudioSystem()
{
  std::string theList = "audio/List.txt";
  AEngine = new Audio_Engine;
  AEngine->Init();
  GatherSounds( theList, *AEngine);
}

/*! \brief
 * Reads a list of sounds from a text file and loads them into the engine.
 * \param strFileName
 * Path of the list.
 */
void GatherSounds( const string& strFileName, Audio_Engine& aEngine )
{
  std::ifstream fIn;
  fIn.open( strFileName.c_str() );
  if ( !fIn.is_open() )
  {
    Log<Error>("Couldn't open the Sound List");
    return;
  }
  //container
  std::string prefix = "audio/";
  std::string strSoundName;
  //iterator
  while ( fIn.good() )
  {
    // Grab a name from the file.
    strSoundName = "5debug5";
    std::getline( fIn, strSoundName );
    char chrFirstLetter = strSoundName[0];
    // Issue a warning if the input is bad.
    if ( !( 'A' <= chrFirstLetter && chrFirstLetter >= 'Z' ) &&
	 !( 'a' <= chrFirstLetter && chrFirstLetter >= 'z' ) )
    {
      Log<Error>("Sound named '%s  does not start with a letter.'", strSoundName.c_str());
      return;
    }
    // Sets audio to 2D by default.
    FMOD_MODE eMode = FMOD_DEFAULT;
    // If the first letter is an 'm', loop it.
    if ( strSoundName[0] == 'm' )
    {
      eMode |= FMOD_LOOP_NORMAL;
    }
    std::string finished_name = prefix + strSoundName;
    // Send the results of reading our naming scheme to the Audio Engine.
    aEngine.LoadSound( finished_name, (eMode & FMOD_3D) != 0, (eMode & FMOD_LOOP_NORMAL) != 0, (eMode & FMOD_CREATESTREAM) != 0 );
  }
}
