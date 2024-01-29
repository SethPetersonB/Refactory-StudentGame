// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <string> // Used as container.
#include <map>    // Higher level container.
#include <vector>
#include <math.h>

//#pragma warning (push)
//#pragma warning (disable : 4201)
#include "../include/fmod/fmod.hpp"
#include "../include/fmod/fmod_dsp.h"
#include "../include/fmod/fmod_common.h"
#include "../include/fmod/fmod_errors.h"
#include "../include/fmod/fmod_studio.hpp"
//#pragma warning (pop)

//#pragma comment (lib, "fmodL_vc.lib")

using std::map;
using std::string;

struct Vector3
{
  float x;
  float y;
  float z;
  Vector3() : x(0), y(0), z(0) {};
  Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
};

/**********************************************************************/
/**********************************************************************/
// This keeps the FMOD API calls separate from the engine class.
//      It also holds a map of played sounds and triggered events.
struct Implementation
{
  Implementation();
  ~Implementation();

  void Update();

  FMOD::Studio::System* mpStudioSystem;
  FMOD::System* mpSystem;

  int mnNextChannelId;

  typedef map<string, FMOD::Sound*> SoundMap;
  typedef map<int, FMOD::Channel*> ChannelMap;
  typedef map<int, string> ChannelPlayingMap;
  typedef map<string, FMOD::Studio::EventInstance*> EventMap;
  typedef map<string, FMOD::Studio::Bank*> BankMap;
  typedef map<int, float> VolumeMap;

  //These two aren't used right now.
  BankMap mBanks;
  EventMap mEvents;
  /////////////////////
  SoundMap mSounds;
  ChannelMap mChannels;
  ChannelPlayingMap mPlaying;
  VolumeMap mVolume;

  //Stores list of sounds paused by the "pause all" function.
  //   This lets you unpause only those sounds later.
  ChannelMap mPaused;
};

/**********************************************************************/
/**********************************************************************/

namespace luabind
{
  struct scope;
}

class Audio_Engine
{
  public:
    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck( FMOD_RESULT result );

    void LoadBank( const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags );
    void LoadEvent( const string& strEventName );
    void LoadSound( const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false );
    void UnLoadSound( const string& strSoundName );
    void Set3dListenerAndOrientation( const Vector3& vPos = Vector3(), float fVolumedB = 0.0f );
    int PlaySounds( const string& strSoundName, const Vector3& vPos = Vector3(), float fVolumedB = 0.0f );
    void PlayEvent( const string& strEventName );
    void SetChannelPause( int nChannelId, bool pause );
    int FindSoundChannel(const string& strSoundName);
    void GetChannelSounds( int nChannelId, std::vector<string>& vstrSoundsFound );
    void GetSoundChannels( const string& strSoundName, std::vector<int>& vintChannelsFound );
    void StopEvent( const string& strEventName, bool bImmediate = false );
    void GetEventParameter( const string& strEventName, const string& strEventParameter, float* parameter );
    void SetEventParameter( const string& strEventName, const string& strParameterName, float fValue );
    void SetChannelMute(int nChannelId, bool mute);
    void SetAllChannelsMute( bool isPaused );
    void StopChannelPtr(FMOD::Channel* target);
    void StopChannel( int nChannelId );
    float GetChannelVolume( int nCHannelId );
    void SetChannel3dPosition( int nChannelId, const Vector3& vPosition );
    void SetChannelVolume( int nChannelId, float fVolumedB );
    bool IsPlaying( int nChannelId ) const;
    //bool IsEventPlaying( const string& strEventName ) const;
    void SetGlobalVolumedB(float volume);
    void SetChannelPriority(int nChannelId, int priority);
    float GetGlobalVolumedB();
    static FMOD_RESULT F_CALLBACK EndOfSound(FMOD_CHANNELCONTROL *chanControl,
                                         FMOD_CHANNELCONTROL_TYPE controlType,
                                         FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                                         void *commandData1,
                                         void *commandData2);

    // lua stuff
    static luabind::scope GetLuaRegisters();

};

float dbToVolume(float db);
float VolumeTodb(float volume);
FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
#endif
