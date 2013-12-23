#ifndef OMXPLAYER_CLIP_H
#define OMXPLAYER_CLIP_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <time.h>

// Not sure what I am doing here, just included all the below in order
// to get the extern working
#include "omxplayer.h"
#include "OMXStreamInfo.h"

#include "utils/log.h"

#include "DllAvUtil.h"
#include "DllAvFormat.h"
#include "DllAvFilter.h"
#include "DllAvCodec.h"
#include "linux/RBP.h"

#include "OMXVideo.h"
#include "OMXAudioCodecOMX.h"
#include "utils/PCMRemap.h"
#include "OMXClock.h"
#include "OMXAudio.h"
#include "OMXReader.h"
#include "OMXPlayerVideo.h"
#include "OMXPlayerAudio.h"
#include "OMXPlayerSubtitles.h"
// #include "OMXControl.h"
#include "DllOMX.h"
#include "Srt.h"
// #include "KeyConfig.h"
#include "utils/Strprintf.h"
// #include "Keyboard.h"
#include <utility>
#include "version.h"

using namespace std;

#define CLIP_CHECK_THRESHOLD 1
#define MICRO_PER_SECOND 1000000

extern OMXClock *m_av_clock;

class OMXPlayerClip
{
   public:
      OMXPlayerClip( double p_duration, double p_startPos );
      bool checkComplete ();
      void display ();
   private:
      double duration;
      double startPos;
      double finishPos;
      int clipTicker;
      bool disabled;
};
#endif // #ifndef OMXPLAYERSYNCH_H

