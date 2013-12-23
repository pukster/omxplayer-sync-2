#ifndef OMXPLAYER_PLAYLIST_H
#define OMXPLAYER_PLAYLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>

using namespace std;

#define PLAYLIST_DELIMITER ","

class OMXPlayerPlaylistEntry
{
public:
   OMXPlayerPlaylistEntry ();
   double startPos;
   double finishPos;
   string path;
};

class OMXPlayerPlaylist
{
public:
   OMXPlayerPlaylist ();
   OMXPlayerPlaylist ( string a_filename );
   OMXPlayerPlaylist ( string a_filename, int nothing );
   void createPlaylist ();
   void createPlaylistFromFile ( string a_filename );
   void usage ();
   char* getCurrentFilename ();
   double getCurrentStartPos ();
   double getCurrentFinishPos ();
   void next ();
   bool hasNext ();
   bool isFinished ();
private:
   unsigned int countLines();
   void createPlaylistEntry ( string line, int index );
   bool isValidPlaylist ();
   string playlistPath;
   OMXPlayerPlaylistEntry *playlistEntries;
   int playlistLength;
   int current;
};
#endif // #ifndef OMXPLAYER_PLAYLIST_H

