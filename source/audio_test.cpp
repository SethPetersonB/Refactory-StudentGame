// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <luabind/luabind.hpp>
#include "../include/Logger.h"
#include "../include/audio_test.h"
#include "../include/audio_startup.h"

#define DEBUG 1

using std::vector;
using std::string;
using namespace Logger;

static float volume_ = 0.0f;

/*! \brief
 * Creates the containers for the system, initializes the system.
 */
Implementation::Implementation()
{
    mpStudioSystem = NULL;
    Audio_Engine::ErrorCheck( FMOD::Studio::System::create( &mpStudioSystem ) );
    Audio_Engine::ErrorCheck( mpStudioSystem->initialize( 200, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL ) );

    mpSystem = NULL;
    Audio_Engine::ErrorCheck( mpStudioSystem->getLowLevelSystem( &mpSystem ) );
}

/*! \brief
 * Cleans up the information being used/stored and shuts down FMOD.
 */
Implementation::~Implementation() 
{
    Audio_Engine::ErrorCheck( mpStudioSystem->unloadAll() );
    Audio_Engine::ErrorCheck( mpStudioSystem->release() );
}

/*! \brief
 * Loops through channels to find and delete stopped ones.
 */
void Implementation::Update()
{
  Audio_Engine::ErrorCheck( mpStudioSystem->update() );
}


/**********************************************************************/
// Global pointer to our implementation system.
Implementation* sgpImplementation = NULL;
/**********************************************************************/

/*! \brief
 * Creates the audio engine.
 */
void Audio_Engine::Init()
{
  sgpImplementation = new Implementation;
}

/*! \brief
 * Necessary cleanup every game loop!
 */
void Audio_Engine::Update()
{
  sgpImplementation->Update();
}

/*! \brief
 * Loads a sound into the engine.
 * \param strSoundName
 * Unique identifier to search for desired sound.
 * \param b3d
 * Loads a sound with 3D position properties. (Channel setPan won't work here)
 * \param bLooping
 * Loads the sounds as a loop.
 * \param bStream
 * Open sound file/URL so it decompresses/reads at runtime.
 */
void Audio_Engine::LoadSound( const std::string& strSoundName, bool b3d, bool bLooping, bool bStream )
{
  Implementation::SoundMap::iterator tFoundIt = sgpImplementation->mSounds.find( strSoundName );
  // Say NO to duplicate sound loading!
  if ( tFoundIt != sgpImplementation->mSounds.end() )
  {
    Log<Info>("Already created sound: %s", strSoundName.c_str());
    return;
  }

  FMOD_MODE eMode = FMOD_DEFAULT;
  eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
  eMode |= b3d ? FMOD_3D : FMOD_2D;
  eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

  FMOD::Sound* pSound = NULL;
  std::string prefix = "audio/";
  std::string finished_name = prefix + strSoundName;
  // Searches in directory for a sound and gives it the desired playmodes.
  Audio_Engine::ErrorCheck( sgpImplementation->mpSystem->createSound( finished_name.c_str(), eMode, NULL, &pSound ) );
  Log<Info>("'%s' ping", strSoundName.c_str());

  if ( pSound )
  {
    sgpImplementation->mSounds[ strSoundName ] = pSound;
  }
}

/*! \brief
 * Unloads a sound from the audio engine.
 * \param strSoundName
 * Sound to be removed.
 */
void Audio_Engine::UnLoadSound( const std::string& strSoundName )
{
  Implementation::SoundMap::iterator tFoundIt = sgpImplementation->mSounds.find( strSoundName );
  // Stop if you can't find it.
  if ( tFoundIt == sgpImplementation->mSounds.end() )
  {

    Log<Warning>("Couldn't find sound: %s", strSoundName.c_str());

    return;
  }
  Audio_Engine::ErrorCheck( tFoundIt->second->release() );
  sgpImplementation->mSounds.erase( tFoundIt );
}

/*! \brief
 * Puts a sound into a channel and starts it.
 * \param strSoundName
 * Search the Sound map for this.
 * \param vPosition
 * Location of the sound in 3D space.
 * \param fVolumedB
 * How loud the volume is, in decibels.
 * \return
 * The channel containing the sound.
 */
int Audio_Engine::PlaySounds(const string& strSoundName, const Vector3& vPosition, float fVolumedB)
{ 
  int nChannelId = 0;

  unsigned count = getSoundCount(strSoundName);
  if ( count > 5 )
  {
    //Do nothing if there are too many instances of the sound.
    return -1;
  }
  else
  {
    setSoundCount(strSoundName, ++count); //increment the count
  }
  // Increment the channelId variable to keep getting unique IDs
  nChannelId = sgpImplementation->mnNextChannelId++;
  Implementation::SoundMap::iterator tFoundIt = sgpImplementation->mSounds.find( strSoundName );
  // If we can't find the sound in the map, try to find it by name.
  if ( tFoundIt == sgpImplementation->mSounds.end() )
  {
    LoadSound( strSoundName );
    tFoundIt = sgpImplementation->mSounds.find( strSoundName );
    if ( tFoundIt == sgpImplementation->mSounds.end() )
    {
      return nChannelId;
    }
  }

  FMOD::Channel* pChannel = NULL;
  // This plays the sound PAUSED.
  Audio_Engine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, NULL, true, &pChannel));
  FMOD_MODE loop_var = 0;
  tFoundIt->second->getMode(&loop_var);
  if (loop_var == FMOD_LOOP_NORMAL)
  {
    pChannel->setMode(FMOD_LOOP_NORMAL);
  }
  // Adds the sound to the playing map.
  sgpImplementation->mPlaying[nChannelId] = strSoundName;
  sgpImplementation->mVolume[nChannelId] = fVolumedB;
  if ( pChannel )
  {
    FMOD_MODE currMode;
    tFoundIt->second->getMode(&currMode);
    if ( currMode & FMOD_3D )
    {
      FMOD_VECTOR position = VectorToFmod(vPosition);
      Audio_Engine::ErrorCheck(pChannel->set3DAttributes(&position, NULL));
    }
    // If we're alt-tabbed, silence the sound and add it to the list!
    if ( getVolumeMute() )
    {
      Audio_Engine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB + volume_)));
      sgpImplementation->mPaused[nChannelId] = pChannel;
    }
    else
    {
      Audio_Engine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB + volume_)));
    }
    Audio_Engine::ErrorCheck( pChannel->setPaused(false) );
    sgpImplementation->mChannels[nChannelId] = pChannel;
    pChannel->setCallback(Audio_Engine::EndOfSound);
  }
  sgpImplementation->Update();

  //DEBUG text! Please let it sit there if it is needed in the future.
  /*
  char name[20];
  for ( int i = 0; i < 20; ++i )
  {
    if ( i < strSoundName.size() )
    {
      name[i] = strSoundName[i];
    }
    else
    {
      name[i] = '*';
    }
  }
  Log<Warning>("Playing sound '%s' ", name);
  Log<Warning>("Playing Channel count: %d", static_cast<int>(sgpImplementation->mPlaying.size()));
  int channelsplaying = 0, realchan = 0;
  unsigned vers = 0;
  Log<Warning>("FMOD Channel count: %d", channelsplaying );
  Log<Warning>("Playing Channel count: %d", static_cast<int>(sgpImplementation->mPlaying.size()));
  Log<Warning>("Channel count: %d", static_cast<int>(sgpImplementation->mChannels.size()));
  if ( sgpImplementation->mPlaying.size() > 4 )
  {
    char soundname[20];
    for ( Implementation::ChannelPlayingMap::iterator j = sgpImplementation->mPlaying.begin(); j != sgpImplementation->mPlaying.end(); ++j )
    {
      // Get the name of each sound in the playing channel.
      for (int i = 0; i < 20; ++i)
      {
        if (i < (j->second).size())
        {
          soundname[i] = (j->second)[i];
        }
        else
        {
          soundname[i] = '*';
        }
      }
      Log<Warning>("Channel %d sound '%s' ", j->first, soundname);
    }
  }
  */

  // If we've got paused sounds, play new sounds paused as well.
  if ( !sgpImplementation->mPaused.empty() )
  {
    SetChannelMute(nChannelId, true);
    sgpImplementation->mPaused[nChannelId] = pChannel;
  }
  return nChannelId;
}

/*! \brief
 * Sets the location of the sound.
 * \param nChannelID
 * Specifies which channel is having its location set.
 * \param vPosition
 * Specifies the position to set the channel to.
 */
void Audio_Engine::SetChannel3dPosition( int nChannelId, const Vector3& vPosition )
{
  Implementation::ChannelMap::iterator tFoundIt = sgpImplementation->mChannels.find( nChannelId );
  // If you can't find the channel, stop.
  if ( tFoundIt == sgpImplementation->mChannels.end() )
  {

    Log<Warning>("Couldn't find channel: %d", nChannelId);

    return;
  }

  FMOD_VECTOR position = VectorToFmod( vPosition );
  Audio_Engine::ErrorCheck( tFoundIt->second->set3DAttributes( &position, NULL) );
}

/*! \brief
 * Set the volume of sounds.
 * \param nChannelId
 * Specify the channel.
 * \param fVolumedB
 * Specify the volume to set for that channel.
 */
void Audio_Engine::SetChannelVolume( int nChannelId, float fVolumedB )
{
  float volume = fVolumedB + GetGlobalVolumedB();
  Implementation::ChannelMap::iterator tFoundIt = sgpImplementation->mChannels.find( nChannelId );
  if ( tFoundIt == sgpImplementation->mChannels.end() )
  {
    Log<Warning>("Couldn't find channel: %d", nChannelId);

    return;
  }

  if ( getMelodyMute() )
  {
    if ( tFoundIt->first == FindSoundChannel("menumelody_repeat.wav") || tFoundIt->first == FindSoundChannel("startup.wav"))
    {
      volume = -96.0f;
    }
  }
  // Don't record the volume if we're just muting it.
  if ( !getVolumeMute() )
  {
    if ( !getMelodyMute() )
    {
      sgpImplementation->mVolume[nChannelId] = fVolumedB;
    }
    else
    {
      if ( tFoundIt->first != FindSoundChannel("menumelody_repeat.wav") || tFoundIt->first == FindSoundChannel("startup.wav"))
      {
        sgpImplementation->mVolume[nChannelId] = fVolumedB;
      }
      //else it's a melody and is mute so the volume should not be recorded.
    }
  }
  Audio_Engine::ErrorCheck( tFoundIt->second->setVolume( dbToVolume( volume ) ) );
}

/*! \brief
 * Get the volume of sounds.
 * \param nChannelId
 * Specify the channel.
 * \return
 * The volume level of the channel (dB)
 */
float Audio_Engine::GetChannelVolume(int nChannelId)
{
  return sgpImplementation->mVolume[nChannelId];
}

/*! \brief
 * Loads a bank from the map into the audio engine.
 * \param strBankName
 * Unique identifier for the bank.
 * \flags
 * Check FMOD help for info on these bank flags.
 */
void Audio_Engine::LoadBank( const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags )
{
  Implementation::BankMap::iterator tFoundIt = sgpImplementation->mBanks.find( strBankName );
  // Stop if the identifier string is already in use.
  if ( tFoundIt != sgpImplementation->mBanks.end() )
  {
    Log<Info>("Already created bank: ", strBankName.c_str());
    return;
  }
  FMOD::Studio::Bank* pBank;
  // The part that puts it int
  Audio_Engine::ErrorCheck( sgpImplementation->mpStudioSystem->loadBankFile( strBankName.c_str(), flags, &pBank ) );
  if ( pBank )
  {
    sgpImplementation->mBanks[strBankName] = pBank;
  }
}

/*! \brief
 * Loads Event audio
 * \param strEventName
 * Unique identifier for the Event.
 */
void Audio_Engine::LoadEvent( const std::string& strEventName ) {
  Implementation::EventMap::iterator tFoundit = sgpImplementation->mEvents.find( strEventName );
  // Stop if you found an event using the same name.
  if ( tFoundit != sgpImplementation->mEvents.end() )
  {
    return;
  }
  FMOD::Studio::EventDescription* pEventDescription = NULL;
  Audio_Engine::ErrorCheck( sgpImplementation->mpStudioSystem->getEvent( strEventName.c_str(), &pEventDescription ) );
  if ( pEventDescription )
  {
    FMOD::Studio::EventInstance* pEventInstance = NULL;
    Audio_Engine::ErrorCheck(pEventDescription->createInstance( &pEventInstance ) );
    if ( pEventInstance )
    {
      sgpImplementation->mEvents[strEventName] = pEventInstance;
    }
  }   
}

/*! \brief
 * Starts playing an event.
 * \param strEventName
 * Specifies which event to play.
 */
void Audio_Engine::PlayEvent( const string &strEventName )
{
  Implementation::EventMap::iterator tFoundit = sgpImplementation->mEvents.find( strEventName );
  // If we can't find it, try to load the event.
  if ( tFoundit == sgpImplementation->mEvents.end() )
  {
    LoadEvent( strEventName );
    tFoundit = sgpImplementation->mEvents.find( strEventName );
    // If it can't be loaded in, stop.
    if ( tFoundit == sgpImplementation->mEvents.end() )
    {
      return;
    }
  }
  tFoundit->second->start();
}

void Audio_Engine::SetChannelPause( int nChannelId, bool pause )
{
	//for ( ChannelMap::iterator it = mChannels.begin(), itEnd = mChannels.end() ; it != itEnd ; ++it )
	Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.find(nChannelId);
	if ( it != sgpImplementation->mChannels.end() )
	{
		Audio_Engine::ErrorCheck( it->second->setPaused( pause ) );
	}
}

void Audio_Engine::GetChannelSounds( int nChannelId, vector<std::string>& vstrSoundsFound )
{
  for (Implementation::ChannelPlayingMap::iterator it = sgpImplementation->mPlaying.begin(), itEnd = sgpImplementation->mPlaying.end() ; it != itEnd ; ++it)
  {
    if ( it->first == nChannelId )
    {
      vstrSoundsFound.push_back( it->second );
    }
  }
}

void Audio_Engine::GetSoundChannels( const string& strSoundName, vector<int>& vintChannelsFound )
{
  for ( Implementation::ChannelPlayingMap::iterator it = sgpImplementation->mPlaying.begin(), itEnd = sgpImplementation->mPlaying.end() ; it != itEnd ; ++it )
  {
    if ( it->second == strSoundName )
    {
      vintChannelsFound.push_back( it->first );
    }
  }
}

//Currently, this only works to add functions into an overall mute. DO NOT PASS FALSE!
void Audio_Engine::SetChannelMute( int nChannelId, bool mute )
{
	//for ( ChannelMap::iterator it = mChannels.begin(), itEnd = mChannels.end() ; it != itEnd ; ++it )
	Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.find(nChannelId);
	if ( it != sgpImplementation->mChannels.end() )
	{
    float volume = -96.0f;
    if (!mute)
    {
      volume = GetChannelVolume(it->first);
    }
    SetChannelVolume(it->first, volume);
	}
}

//This function will mute all channels.
void Audio_Engine::SetAllChannelsMute( bool isMute )
{
  setVolumeMute(isMute); //This function indicates a blanked mute setting.
  if ( !sgpImplementation->mChannels.empty() )
  {
    for (Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.begin(); it != sgpImplementation->mChannels.end(); ++it)
    {
      float volume = -96.0f;
      // Set volume to normal levels.
      if ( !isMute )
      {
        // Should we unpause the melodies?
        if ( getMelodyMute() )
        {
          if ( it->first == FindSoundChannel("menumelody_repeat.wav"))
          {
            continue;
          }
          if ( it->first == FindSoundChannel("startup.wav"))
          {
            continue;
          }
        }

        volume = GetChannelVolume(it->first);
      }
      SetChannelVolume(it->first, volume);
    }
  }
  // Empty out the Paused sound list once we've set the volumes properly.
  if (!isMute)
  {
    if (!sgpImplementation->mPaused.empty())
    {
      if ( !getMelodyMute() )
      {
        sgpImplementation->mPaused.clear();
      }
      else //empty out all other sounds.
      {
        int nChannelId = FindSoundChannel("menumelody.wav");
        int nChannelId2 = FindSoundChannel("startup.wav");
        //This deletion causes problems
        Implementation::ChannelMap unmute;
        for (Implementation::ChannelMap::iterator it = sgpImplementation->mPaused.begin(); it != sgpImplementation->mPaused.end(); ++it)
        {
          // If it's not either of the melody sounds, remove it.
          if ( it->first != nChannelId && it->first != nChannelId2 )
          {
            unmute.insert(*it);
          }
        }
        for (Implementation::ChannelMap::iterator it = unmute.begin(); it != unmute.end(); ++it)
        {
          Implementation::ChannelMap::iterator condemned = sgpImplementation->mPaused.find(it->first);
          sgpImplementation->mPaused.erase(condemned);
        }
      }
    }
  }

  /* THIS FUNCTIONALITY IS BROKEN because the cleanup service removes information. */
#if 0
  /* Are we looking for sounds to pause? */
  if ( isPaused )
  {
    for (Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.begin(), itEnd = sgpImplementation->mChannels.end(); it != itEnd; ++it)
    {
      // Only set currently playing sounds to be paused.
      bool playing = false;
      it->second->isPlaying( &playing );
      if ( playing )
      {
        Audio_Engine::ErrorCheck(it->second->setPaused(isPaused));
        sgpImplementation->mPaused.insert(*it);
      }
    }
  }
  else /* Are we unpausing our paused sounds? */
  {
    Implementation::ChannelMap::iterator it = sgpImplementation->mPaused.begin();
    while (it != sgpImplementation->mPaused.end() )
    {
      Audio_Engine::ErrorCheck(it->second->setPaused(isPaused));
      it = sgpImplementation->mPaused.erase(it);
    }
  }
#endif
}

/* This alters the priorty of a channel.
   0: Most important
   256: least important
   */
void Audio_Engine::SetChannelPriority(int nChannelId, int priority)
{
  FMOD::Channel* target = NULL;
  sgpImplementation->mpSystem->getChannel(nChannelId, &target );
  target->setPriority(priority);
}

/* UNTESTED */
void Audio_Engine::Set3dListenerAndOrientation(const Vector3& vPos, float fVolumedB)
{
  const FMOD_VECTOR * vFPos = &VectorToFmod(vPos);
  const FMOD_VECTOR * vFVel = &VectorToFmod( Vector3() );
  sgpImplementation->mpSystem->set3DListenerAttributes(0, vFPos, vFVel, vFVel, vFVel );
}

/* Finds the first channel which is playing a sound. */
int Audio_Engine::FindSoundChannel(const string& strSoundName)
{
  for (Implementation::ChannelPlayingMap::iterator soundIt = sgpImplementation->mPlaying.begin(), itEnd = sgpImplementation->mPlaying.end(); soundIt != itEnd ; ++soundIt)
  {
    if ( soundIt->second == strSoundName )
      return soundIt->first;
  }
  return -1; /* Magical number of doom that I hope never bites me. */
}

bool Audio_Engine::IsPlaying(int nChannelId) const
{
  Implementation::ChannelMap::iterator channelIt = sgpImplementation->mChannels.find(nChannelId);
  bool retval = false;
  if ( channelIt != sgpImplementation->mChannels.end() )
  {
    channelIt->second->isPlaying(&retval);
  }
  return retval;
}

// This is mainly used in the callback function. It's faster than StopChannel,
// and is more convenient for that function.
void Audio_Engine::StopChannelPtr(FMOD::Channel* target)
{
  if ( !sgpImplementation->mChannels.empty() )
  {
    for (Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.begin(); it != sgpImplementation->mChannels.end(); ++it)
    {
      //Have we found the channel?
      if (it->second == target)
      {
        int nChannelId = it->first;
        // Remove it from all containers.
        sgpImplementation->mChannels.erase(it);
        Implementation::ChannelPlayingMap::iterator pit = sgpImplementation->mPlaying.find(nChannelId);
        std::string name = pit->second;
        if (pit != sgpImplementation->mPlaying.end())
        {
          sgpImplementation->mPlaying.erase(pit);
        }
        Implementation::VolumeMap::iterator vit = sgpImplementation->mVolume.find(nChannelId);
        if (vit != sgpImplementation->mVolume.end())
        {
          sgpImplementation->mVolume.erase(vit);
        }
        // Make sure it can't hide in the paused channel list.
        Implementation::ChannelMap::iterator sit = sgpImplementation->mPaused.find(nChannelId);
        if (sit != sgpImplementation->mPaused.end())
        {
          //sgpImplementation->mPaused.erase(sit);
          target->setPaused(true);
        }
        else
        {
          target->stop();
        }

        unsigned count = getSoundCount(name); // these two lines decrement the count.
        setSoundCount(name, --count);
        break;
      }
    }
  }
  Update();
}

//Both stop and pause a channel. The surest way to kill it.
// Also removes it from the Paused channel list.
void Audio_Engine::StopChannel(int nChannelId)
{
  FMOD::Channel* target = NULL;
  Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.find(nChannelId);
  if ( !sgpImplementation->mChannels.empty() && it != sgpImplementation->mChannels.end() )
  {
    target = it->second;
    // Use the callback function to clean up.
    target->stop();
  }
  /*
  if ( !sgpImplementation->mChannels.empty() )
  {
    for (Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.begin(); it != sgpImplementation->mChannels.end(); ++it)
    {
      //Have we found the channel?
      if (it->second == target)
      {
        nChannelId = it->first;
        // Remove it from all containers.
        sgpImplementation->mChannels.erase(it);
        Implementation::ChannelPlayingMap::iterator pit = sgpImplementation->mPlaying.find(nChannelId);
        if (pit != sgpImplementation->mPlaying.end())
        {
          sgpImplementation->mPlaying.erase(pit);
        }
        Implementation::VolumeMap::iterator vit = sgpImplementation->mVolume.find(nChannelId);
        if (vit != sgpImplementation->mVolume.end())
        {
          sgpImplementation->mVolume.erase(vit);
        }
        // Make sure it can't hide in the paused channel list.
        Implementation::ChannelMap::iterator sit = sgpImplementation->mPaused.find(nChannelId);
        if (sit != sgpImplementation->mPaused.end())
        {
          sgpImplementation->mPaused.erase(sit);
        }
        target->stop();
        break;
      }
    }
  }
  */
  Update();
}

/*! \brief
 * Try to stop a playing event.
 * \param strEventName
 * Specifies which event to stop.
 * \param bImmediate
 * Allows sound to (true): stop instantly, (false): fade out.
 */
void Audio_Engine::StopEvent( const string &strEventName, bool bImmediate )
{
  Implementation::EventMap::iterator tFoundIt = sgpImplementation->mEvents.find( strEventName );
  // Stop if we can't find the event.
  if ( tFoundIt == sgpImplementation->mEvents.end() )
  {
    return;
  }

  FMOD_STUDIO_STOP_MODE eMode;
  // Instant or fade?
  eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
  // Change the play mode.
  Audio_Engine::ErrorCheck( tFoundIt->second->stop( eMode ) );
}

#if 0
/*! \brief
 * Checks to see if an Event is currently playing.
 * \param strEventName
 * Specifies which event.
 * \return
 * True if playing, false otherwise.
 */
bool Audio_Engine::IsEventPlaying( const string& strEventName ) const
{
  bool bIsPlaying = false;
  Implementation::EventMap::iterator tFoundIt = sgpImplementation->mEvents.find( strEventName );
  // Stop if we can't find the event.
  if ( tFoundIt == sgpImplementation->mEvents.end() )
  {
    if ( DEBUG )
    {
      /* TODO: Make this debug later */
    }
  }
  return tFoundIt->second->isPlaying();
}
#endif

/*! \brief
 * Gets information about an Event.
 * \param strEventName
 * Specify the Event.
 * \param strParameterName
 * Specify the type of information being searched for.
 * \param parameter
 * Output - pointer to the value.
 */
void Audio_Engine::GetEventParameter( const string &strEventName, const string &strParameterName, float* parameter )
{
  Implementation::EventMap::iterator tFoundIt = sgpImplementation->mEvents.find( strEventName );
  // If you can't find it, stop.
  if ( tFoundIt == sgpImplementation->mEvents.end() )
  {
    if ( DEBUG )
    {
      //TODO: Debug statement!
    }
    return;
  }

  FMOD::Studio::ParameterInstance* pParameter = NULL;
  Audio_Engine::ErrorCheck( tFoundIt->second->getParameter( strParameterName.c_str(), &pParameter ) );
  Audio_Engine::ErrorCheck( pParameter->getValue( parameter ) );
}

/*! \brief
 * Gets the value of an Event parameter.
 * \param strEventName
 * Specifies the Event.
 * \param strParameterName
 * Specifies the parameter.
 * \param fValue
 * Parameter will be set to this value.
 */
void Audio_Engine::SetEventParameter( const string &strEventName, const string &strParameterName, float fValue )
{
  Implementation::EventMap::iterator tFoundIt = sgpImplementation->mEvents.find( strEventName );
  // If you can't find it, stop.
  if ( tFoundIt == sgpImplementation->mEvents.end() )
  {
    if ( DEBUG )
    {
      //TODO: Debug statement!
    }
    return;
  }

  FMOD::Studio::ParameterInstance* pParameter = NULL;
  Audio_Engine::ErrorCheck( tFoundIt->second->getParameter( strParameterName.c_str(), &pParameter ) );
  Audio_Engine::ErrorCheck( pParameter->setValue( fValue ) );
}

void Audio_Engine::SetGlobalVolumedB( float volume )
{
  volume_ = volume;

  for (Implementation::ChannelMap::iterator it = sgpImplementation->mChannels.begin(); it != sgpImplementation->mChannels.end(); ++it)
  {
    /* THIS ERASES PREVIOUS INFORMATION ON VOLUME! */
    SetChannelVolume( it->first, volume_ );
  }
}

float Audio_Engine::GetGlobalVolumedB()
{
  return volume_;
}

/*! \brief
 * Converts float vector into FMOD vector.
 * \param vPosition
 * Input values.
 * \return
 * FMOD vector.
 */
FMOD_VECTOR VectorToFmod( const Vector3& vPosition )
{
  FMOD_VECTOR fVec;
  fVec.x = vPosition.x;
  fVec.y = vPosition.y;
  fVec.z = vPosition.z;
  return fVec;
}

FMOD_RESULT F_CALLBACK Audio_Engine::EndOfSound(FMOD_CHANNELCONTROL *chanControl,
                                         FMOD_CHANNELCONTROL_TYPE controlType,
                                         FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                                         void *commandData1,
                                         void *commandData2)
{
  // Has the channel's sound ended?
  if (controlType == FMOD_CHANNELCONTROL_CALLBACK_END)
  {
    FMOD::Channel *channel = (FMOD::Channel *)chanControl;
    // Find the channel in our containers.

    // Only stop the channel if it's actually done playing.
    int nChannelId = -1;
    if (!sgpImplementation->mChannels.empty())
    {
      GetAudioEngine()->StopChannelPtr(channel);
    }
  }
  /*
  else
  {
    FMOD::ChannelGroup *group = (FMOD::ChannelGroup *)chanControl;
    // ChannelGroup specific functions here...
  }
  */

  // ChannelControl generic functions here...

    return FMOD_OK;
}

/*! \brief
 * Converts dB to volume.
 * \param dB
 * Input.
 * \return
 * Output volume value.
 */
float dbToVolume( float dB )
{
    return powf(10.0f, 0.05f * dB);
}

/*! \brief
 * Converts volume to dB
 * \param volume
 * Input.
 * \return
 * Output dB value.
 */
float VolumeTodb( float volume )
{
    return 20.0f * log10f(volume);
}

/*! \brief
 * Checks to see if FMOD encountered a problem.
 * \param result
 * Output from an FMOD function.
 * \return
 * Boolean, true is bad, false means everything is running smoothly.
 */
int Audio_Engine::ErrorCheck( FMOD_RESULT result )
{
  if ( result != FMOD_OK )
  {
    Log<Error>("FMOD ERROR %d", result);
    return 1;
  }

  return 0;
}

/*! \brief
 * Headshot the audio system with a silver bullet from a golden gun.
 * It's dead, Jim.
 * You deleted it.
 */
void Audio_Engine::Shutdown()
{
  delete sgpImplementation;
}

static int PlaySoundsBind(Audio_Engine& AE, std::string str, float volume)
{
  return AE.PlaySounds(str, Vector3(), volume);
}

luabind::scope Audio_Engine::GetLuaRegisters()
{
  using namespace luabind;
  return class_<Audio_Engine>("Audio_Engine")
    .scope[def("getSystem", &GetAudioEngine)]
    .def("PlaySounds",PlaySoundsBind);
    
}
