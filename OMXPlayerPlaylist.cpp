#include "OMXPlayerPlaylist.h"

OMXPlayerPlaylistEntry::OMXPlayerPlaylistEntry () {}

OMXPlayerPlaylist::OMXPlayerPlaylist ( string filename )
{
   current = 0;
   playlistPath = filename;
   createPlaylist ();
   if ( !isValidPlaylist () )
   {
      printf ( "Invalid Playlist \n" );
   }
}

/**
 * This is the wrong way to do it, but I don't care much at this       *
 * moment eventually, however, I will have to fix these constructors   *
 */
OMXPlayerPlaylist::OMXPlayerPlaylist ( string filename, int nothing )
{
   current = 0;
   createPlaylistFromFile ( filename );
   if ( !isValidPlaylist () )
   {
      printf ( "Invalid Playlist \n" );
   }
}

void OMXPlayerPlaylist::createPlaylist ()
{
   ifstream playlistFile;
   string line;
   int i;

   playlistLength = countLines ();

   playlistEntries = new OMXPlayerPlaylistEntry [ playlistLength ];

   playlistFile.open ( playlistPath.c_str () );

   if ( playlistFile.is_open() )
   {
      i = 0;
      while ( getline ( playlistFile, line ) )
      {
         // cout << line << '\n';
         // playlistEntries [ i ] = OMXPlayerPlaylistEntry ();
         createPlaylistEntry ( line, i );
         i++;
      }
      playlistFile.close();
   }
   else
   {
      cout << "Unable to open file";
   }
}

void OMXPlayerPlaylist::createPlaylistFromFile ( string a_filename )
{
   playlistLength = 1;
   playlistEntries = new OMXPlayerPlaylistEntry [ playlistLength ];

   playlistEntries [ 0 ].startPos = 0;
   playlistEntries [ 0 ].finishPos = 0;
   playlistEntries [ 0 ].path = a_filename.c_str ();
}

void OMXPlayerPlaylist::createPlaylistEntry ( string line, int index )
{
   const char *p;
   char *writable = new char[line.size() + 1];

   std::copy ( line.begin(), line.end(), writable );
   writable[line.size()] = '\0'; // don't forget the terminating 0

   p = strtok ( writable, PLAYLIST_DELIMITER );

   playlistEntries [ index ].startPos = atoi ( p );

   p = strtok ( NULL, PLAYLIST_DELIMITER );
   playlistEntries [ index ].finishPos = atoi ( p );

   p = strtok ( NULL, PLAYLIST_DELIMITER );
   playlistEntries [ index ].path = p;
}

void OMXPlayerPlaylist::usage ()
{
   cout << "File structure must be like so, with NONE of the entries missing:\n";
   cout << "[start position in seconds],[full file path],[finish position in seconds]\n";
}

unsigned int OMXPlayerPlaylist::countLines()
{
   int numLines = 0;

   ifstream in ( playlistPath.c_str () );
   std::string unused;

   while ( std::getline ( in, unused ) )
   {
      ++numLines;
   }

   return numLines;
}

bool OMXPlayerPlaylist::isValidPlaylist ()
{
   int i;
   bool valid = true;

   for ( i = 0; i < playlistLength; i++ )
   {
      valid = valid && playlistEntries [ i ].startPos >= 0;
      valid = valid && playlistEntries [ i ].finishPos >= 0;
      valid = valid && playlistEntries [ i ].finishPos >= playlistEntries [ i ].startPos ;
      valid = valid && access ( playlistEntries [ i ].path.c_str(), F_OK ) != -1;
   }
   return true;
}

char *OMXPlayerPlaylist::getCurrentFilename ()
{
   return ( char * ) playlistEntries [ current ].path.c_str ();
}

double OMXPlayerPlaylist::getCurrentStartPos ()
{
   return playlistEntries [ current ].startPos;
}

double OMXPlayerPlaylist::getCurrentFinishPos ()
{
   return playlistEntries [ current ].finishPos;
}

void OMXPlayerPlaylist::next ()
{
   current++;
}

bool OMXPlayerPlaylist::hasNext ()
{
   return current < playlistLength - 1;
}

bool OMXPlayerPlaylist::isFinished ()
{
   return current >= playlistLength;
}

