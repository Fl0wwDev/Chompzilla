// Guards to prevent multiple inclusion
#ifndef MAIN_MENU_UTILS_H
#define MAIN_MENU_UTILS_H

#include <stdbool.h>

typedef struct {
    char ip[16];
    unsigned short port;
} IpPort;

void help_menu();
IpPort ask_server_ip_port();
bool verify_port(char* port);
bool verify_ip_port(char *server_ip);
void gamemode_menu(int interface_type);
void game_menu();

#endif