#include "OMXPlayerClip.h"

OMXPlayerClip::OMXPlayerClip ( double p_startPos, double p_finishPos )
{
   finishPos = ( p_finishPos > 0 ) ? p_finishPos : 0;
   startPos = ( p_startPos > 0 ) ? p_startPos : 0;
   
   if ( finishPos > 0 && finishPos < startPos )
   {
      perror ( "Clip can not finish before it starts\n" );
   }
   
   disabled = finishPos == 0;
   
   duration = finishPos - startPos;
   clipTicker = 0;
}

bool OMXPlayerClip::checkComplete ()
{
   if ( disabled )
   {
      return false;
   }
   else
   {
      if ( clipTicker >= CLIP_CHECK_THRESHOLD )
      {
         clipTicker = 0;
         
         if ( stamp / MICRO_PER_SECOND > finishPos )
         {
            display ();
            return true;
         }
      }
      else
      {
         clipTicker++;
      }
   }
   return false;
}

void OMXPlayerClip::display ()
{
   cout << "Start pos: "<<startPos <<" Finish pos: " <<finishPos <<" Time: " << stamp / MICRO_PER_SECOND <<"\n";
}
