#ifndef OMXPLAYER_H
#define OMXPLAYER_H

void SetSpeed(int iSpeed);
extern int playspeeds[];
extern int playspeed_current;
extern const int playspeed_slow_min;
extern const int playspeed_normal;
extern bool m_is_sync_verbose;
extern double stamp;
#endif // #ifndef OMXPLAYER_H
