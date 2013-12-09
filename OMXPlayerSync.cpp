#include "OMXPlayerSync.h"

OMXPlayerSync::OMXPlayerSync( SyncType _syncType, int _numNodes, int _port )
{
   jitter = 0;
   syncType = _syncType;
   syncTimeState = ONTRACK;
   numNodes = _numNodes;
   port = _port;
   reply = strdup ( "0" ); // Can be anything at start
   updateTicker = 2 * UPDATE_THRESHOLD; // want to force an update on the first try
}

OMXPlayerSync::OMXPlayerSync( SyncType _syncType, int _port, const string &_serverAddress )
{
   jitter = 0;
   syncType = _syncType;
   syncTimeState = ONTRACK;
   port = _port;
   serverAddress = _serverAddress;
   question = strdup ( "1" ); // Can be anything
   updateTicker = 2 * UPDATE_THRESHOLD; // want to force an update on the first try
}

OMXPlayerSync::~OMXPlayerSync()
{
   closeConnection ();
}

void OMXPlayerSync::update ()
{
   if ( updateTicker >= UPDATE_THRESHOLD )
   {
      syncWithServer ();
      
      if ( syncType == SYNC_CLIENT )
      {
         updateJitter ();
         updateSpeed ();
      }
      updateTicker = 0;
      displayVerbose ();
   }
   else
   {
      updateTicker++;
   }
}

void OMXPlayerSync::updateJitter ()
{
   if ( clientTime > serverTime + JITTER_TOLERANCE )
   {
      syncTimeState = AHEAD;
      jitter -= JITTER_DELTA;
   }
   else if ( clientTime < serverTime - JITTER_TOLERANCE )
   {
      syncTimeState = BEHIND;
      jitter += JITTER_DELTA;
   }
   else
   {
      syncTimeState = ONTRACK;
      jitter = 0;
   }
}

void OMXPlayerSync::setPort ( int _port )
{
   port = _port;
}

void OMXPlayerSync::updateSpeed ()
{
   if ( syncTimeState != ONTRACK )
   {
      SetSpeed(playspeeds[playspeed_current] + jitter);
   }
}

float OMXPlayerSync::getJitter ()
{
   return jitter;
}

int OMXPlayerSync::syncWithServer ()
{
   int i;
   
   if ( syncType == SYNC_SERVER )
   {
      serverTime = stamp;
      
      //First receive the tcp messages
      for ( i = 0; i < numNodes && i < MAX_NUM_CLIENT_SOCKETS; i++)
      {
         bytesreceived = recv(clientSocket [ i ], buff, BUFFLEN, 0);
         if (-1 == bytesreceived) {
            perror("0:recv");
            return 1;
         }
      }
      
      sprintf ( reply, "%f", serverTime );
    
      //then rapidly send a message back
      for ( i = 0; i < numNodes && i < MAX_NUM_CLIENT_SOCKETS; i++)
      {
         if (-1 == send(clientSocket [ i ], reply, (size_t) strlen(reply) + 1, 0))
         {
            perror("0:send parent");
            return 1;
         }
      }
   }
   else
   {
      clientTime = stamp;
      
      if (-1 == send(sockfd, question, (size_t) strlen(question) + 1, 0)) {
         perror("1:send");
         return 1;
      }
      
      bytesreceived = recv(sockfd, buff, BUFFLEN, 0);
      
      if (-1 == bytesreceived) {
         perror("1:recv");
         return 1;
      }
      
      serverTime = atof ( buff );
   }
   
   return 0;
}

int OMXPlayerSync::setUpConnection ()
{
   int i;
   
   if ( syncType == SYNC_SERVER )
   {
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (-1 == sockfd) {
         perror("0:socket");
         return 1;
      }
      
      if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &forceRebind, sizeof(int))) {
         perror("0:setsockopt");
         return 1;
      }
      
      memset((void *) &server, 0, sizeof(struct sockaddr_in));
      server.sin_family = AF_INET;
      server.sin_port = htons( port );
      server.sin_addr.s_addr = INADDR_ANY;
      
      if (-1 == bind(sockfd, (const struct sockaddr *) &server,
            sizeof(struct sockaddr_in))) {
         perror("0:bind");
         return 1;
      }
      
      if (-1 == listen(sockfd, 0)) {
         perror("0:listen");
         return 1;
      }
      
      // Set up the client sockets
      for ( i = 0; i < numNodes && i < MAX_NUM_CLIENT_SOCKETS; i++)
      {
         clientSocket [ i ] = accept(sockfd, (struct sockaddr *) &client, &clientaddrlen);
         if (-1 == clientSocket [ i ]) {
            perror("0:accept");
            return 1;
         }
      }
      
      return 0;
   }
   else
   {
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (-1 == sockfd) {
         perror("1:socket");
         return 1;
      }
      
      if (0 != getaddrinfo(serverAddress.c_str (), NULL, NULL, &serverinfo)) {
         perror("getaddrinfo");
         return 1;
      }
      
      /*Copy size of sockaddr_in b/c res->ai_addr to big for this example*/
      memcpy(&server, serverinfo->ai_addr, sizeof(struct sockaddr_in));
      server.sin_family = AF_INET;
      server.sin_port = htons( port );
      freeaddrinfo(serverinfo);
      
#ifdef LOOP_UNTIL_CONNECT
      while ( true )
      {
         if (-1 != connect(sockfd, (const struct sockaddr *) &server,
               sizeof(struct sockaddr_in))) {
            break;
         }
         usleep ( CONNECT_SLEEP_PERIOD );
      }
#else
      if (-1 == connect(sockfd, (const struct sockaddr *) &server,
            sizeof(struct sockaddr_in))) {
         perror("1:connect");
         return 1;
      }
#endif // LOOP_UNTIL_CONNECT
      
      return 0;
   }
}

int OMXPlayerSync::closeConnection ()
{
   int i;
   
   if ( syncType == SYNC_SERVER )
   {
      // Close here to speed up previous stage
      for ( i = 0; i < numNodes && i < MAX_NUM_CLIENT_SOCKETS; i++)
      {
         if (-1 == close(clientSocket [ i ])) {
            perror("0:close(clientSocket)");
            return 1;
         }
      }
      
      if (-1 == close(sockfd)) {
         perror("0:close(sockfd)");
         return 1;
      }
      return 0;
   }
   else
   {
      close(sockfd);
      
      return 0;
   }
}

/* Forks a process to run in background and                            *
 * 1) Listen for server broadcasting its play time                     *
 * 2) Listen for clients indicating that they are ready to run         *
 */
void OMXPlayerSync::start ()
{
   // Pause movie, unpause later via tcp communication
   pauseMovie ();
   setUpConnection ();
   syncWithServer ();
   unpauseMovie ();
   displayVerbose ();
}

/*
 * Initally we pause the movie then resume when all nodes are ready.   *
 * Note, we never call this function more than once, hence why we      *
 * don't need to include the jitter yet.                               *
 */
void OMXPlayerSync::pauseMovie ()
{
   playspeed_current = playspeed_slow_min;
   SetSpeed(playspeeds[playspeed_current]);
}

/*
 * Server waits until all nodes are ready then broadcasts a start      *
 * signal. This is meant to only be used once.                         *
 */
void OMXPlayerSync::unpauseMovie ()
{
   playspeed_current = playspeed_normal;
   SetSpeed(playspeeds[playspeed_current] + jitter);
}

void OMXPlayerSync::display ()
{
   if ( m_is_sync_verbose )
   {
      if ( syncType == SYNC_SERVER )
      {
         printf ( "Sync Type: SYNC_SERVER\n" );
         printf ( "Jitter: %f\n", jitter );
         printf ( "Num Nodes: %d\n", numNodes );
         printf ( "Port: %d\n", port );
         cout << "Server Time: " << serverTime << "\n";
      }
      else
      {
         printf ( "Sync Type: SYNC_CLIENT\n" );
         printf ( "Jitter: %f\n", jitter );
         printf ( "Port: %d\n", port );
         cout << "Server Address: " << serverAddress << "\n";
         cout << "Server Time: " << serverTime << "\n";
      }
   }
}

void OMXPlayerSync::displayVerbose ()
{
   if ( m_is_sync_verbose )
   {
      cout.precision (15);
      if ( syncType == SYNC_SERVER )
      {
         cout << "[" << updateTicker << "] Server Time: " << serverTime << "\n";
      }
      else
      {
         cout << "[" << updateTicker << "] Server Time: " << serverTime << " local time: " << clientTime << " Jitter:" << jitter << "\n";
      }
   }
}
