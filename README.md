# ChessGame_clientServer
This was done as homework for the Networks course and the explenations were written in Romanian.  
The way it works is:  
1) The server wait for 2 clients to connect to it.  
2) First client connects and waits
3) Second client connects and waits.
4) The server having 2 clients , makes a thread to server them, while on the main thread waiting for another pair of clients. (back to 1) )
5) The 2 connected clients play chess with each other by sending their moves to the server.
6) The server validates their move and give turns to each player.
7) When the game is over the server closes the comunication with the client successfully.

*) If one of the clients leave, the other one waits, or has to also leave.  
Page of the course: https://profs.info.uaic.ro/~computernetworks/cursullaboratorul.php
