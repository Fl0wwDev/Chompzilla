#ifndef SERVER_H
#define SERVER_H

void handle_client_move(const char* coordinates);
void close_sockets();
void handle_sigint(int sig);
void play_network_server(unsigned short port);
void play_network_server_ia(unsigned short port);
void* launch_server_thread(void* arg);
void start_server_from_gui(unsigned short port);

#endif 
