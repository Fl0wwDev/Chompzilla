#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "main_menu_utils.h"
#include "../game_logic/game_logic.h"
#include "../server/server.h"
#include "../client/client.h"
#include "../gui/gui_gamemode.h"

/**
 * @brief Function to display the help menu
 *
 * @return void
 */
void help_menu()
{
    printf("Welcome to Chompzilla!!!\n");
    printf("Usage: ./game [OPTIONS]\n");
    printf("\033[1mOptions:\033[0m\n");
    printf("-h, --help: Display the help message\n");
    printf("-g, --gui: Launch the GUI\n");
    printf("-l, --local: Launch the local game in CLI mode\n");
    printf("-s, --server: Launch the server in CLI mode\n");
    printf("-ia, --ia: Launch the IA in CLI mode\n");
    printf("-c, --client: Launch the client in CLI mode(with the server IP address after the argument)\n");
}

/**
 * @brief Function to ask the user to enter the server IP address and the port number
 *
 * @return void
 */
IpPort ask_server_ip_port()
{
    // We ask the user for the IP and port
    char server_ip[16];
    unsigned short server_port;

    printf("Enter the server IP address: ");
    scanf("%15s", server_ip);

    // We verify that the IP is correct
    struct sockaddr_in sa;
    while (inet_pton(AF_INET, server_ip, &(sa.sin_addr)) != 1)
    {
        printf("Invalid IP address. Please enter a valid IP address: ");
        scanf("%15s", server_ip); // Limit input to avoid buffer overflow
    }

    printf("Enter the server port: ");
    scanf("%hu", &server_port);
    // We verify if the port is correct
    while (server_port < 1024 || server_port > 65535)
    {
        printf("Invalid port number. Please enter a valid port number: ");
        scanf("%hu", &server_port);
    }

    // We assign the values to the struct
    IpPort ip_port;
    strcpy(ip_port.ip, server_ip);
    ip_port.port = server_port;

    return ip_port;
}

/**
 * @brief Function to verify if the port is correct
 *
 * @param port The port number to verify
 * @return bool True if the port is correct, false otherwise
 */
bool verify_port(char *port)
{
    // We verify that the port number is not longer than the max value of unsigned short
    if (strlen(port) > 5)
    {
        return false;
    }

    int port_verify = atoi(port);
    if (port_verify < 0 || port_verify > USHRT_MAX)
    {
        return false;
    }

    unsigned short port_num = (unsigned short)port_verify;

    // We verify if the port is correct
    if (port_num < 1024 || port_num > 65535)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * @brief Function to verify if the IP address and the port are correct. Format: hhh.hhh.hhh.hhh:pppp
 *
 * @param ip The IP address to verify
 * @return bool True if the IP address is correct, false otherwise
 */
bool verify_ip_port(char *server_ip)
{
    if (server_ip == NULL)
    {
        return false;
    }

    char *server_ip_copy = strdup(server_ip);

    if (server_ip_copy == NULL)
    {
        return false;
    }

    if (strchr(server_ip, ':') == NULL)
    {
        return false;
    }

    // We cut the IP address and the port
    char *saveptr;
    char *ip = strtok_r(server_ip_copy, ":", &saveptr);
    char *port = strtok_r(NULL, ":", &saveptr);

    // We verify that the IP or port exists
    if (ip == NULL || port == NULL)
    {
        free(server_ip_copy);
        return false;
    }

    // We verify that the IP is a valid IP address
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, ip, &(sa.sin_addr)) != 1)
    {
        free(server_ip_copy);
        return false;
    }

    // We verify that the port is correct
    char *endptr;
    long port_num = strtol(port, &endptr, 10);
    if (*endptr != '\0' || port_num < 0 || port_num > 65535)
    {
        free(server_ip_copy);
        return false;
    }

    if (!verify_port((char *)port))
    {
        free(server_ip_copy);
        return false;
    }

    free(server_ip_copy);
    return true;
}

/**
 * @brief Function to display the game mode menu
 *
 * @param type The type of interface selected (1 for CLI, 2 for GUI)
 * @return void
 */
void gamemode_menu(int interface_type)
{
    int choice_game;

    if (interface_type == 1)
    {
        // Interface is CLI
        printf("Choose a game mode:\n");
        printf("------ Local Game Modes ------\n");
        printf("1. Local Multiplayer\n");
        printf("2. Local Multiplayer with IA \n");
        printf("------ Network Game Modes ------\n");
        printf("3. Network Multiplayer (Server)\n");
        printf("4. Network Multiplayer (Client)\n");
        printf("5. Network Multiplayer (IA Client)\n");
        printf("6. Network Multiplayer (IA Server)\n");
        printf("-----------------------------\n");
        printf("7. Exit\n");
        printf("-----------------------------\n");
        printf("Enter your choice: ");
        printf("\n");
        scanf("%d", &choice_game);
        getchar();

        if (choice_game == 1)
        {
            // Play local multiplayer
            play_local_game();
        }
        if (choice_game == 2)
        {
            // Play local multiplayer vs IA
            play_local_game_ia();
        }
        else if (choice_game == 3)
        {
            // Network multiplayer mode (server)
            // We get the port number
            unsigned short port;

            printf("Enter the port number: ");
            scanf("%hu", &port);

            // We verify if the port is correct
            while (port < 1024 || port > 65535)
            {
                printf("Invalid port number. Please enter a valid port number: ");
                scanf("%hu", &port);
            }
            play_network_server(port);
        }
        else if (choice_game == 4)
        {
            // Network multiplayer mode (client)

            IpPort ip_port = ask_server_ip_port();

            play_network_client(ip_port.ip, ip_port.port);
        }
        else if (choice_game == 5)
        {
            // Network multiplayer mode (IA client)

            IpPort ip_port = ask_server_ip_port();

            play_network_ia_client(ip_port.ip, ip_port.port);
        }
        else if (choice_game == 6)
        {
            // Network multiplayer mode (IA server)
            // We get the port number
            unsigned short port;

            printf("Enter the port number: ");
            scanf("%hu", &port);

            // We verify if the port is correct
            while (port < 1024 || port > 65535)
            {
                printf("Invalid port number. Please enter a valid port number: ");
                scanf("%hu", &port);
            }
            play_network_server_ia(port);
        }
        else if (choice_game == 7)
        {
            printf("Exiting the game.\n");
        }
        else
        {
            printf("Invalid choice. Exiting the game.\n");
        }
    }
    else
    {
        // Interface is GUI
        launch_gui_gamemode(0, NULL);
    }
}

/**
 * @brief Function to display the main menu
 *
 * @return void
 */
void game_menu()
{
    int choice_interface;

    printf("Welcome to Chompzilla!!!\n");
    printf("Choose your interface:\n");
    printf("1. Terminal (CLI)\n");
    printf("2. Graphical Interface (GUI)\n");
    printf("Enter your choice: ");
    printf("\n");
    scanf("%d", &choice_interface);

    if (choice_interface == 1)
    {
        // Terminal interface
        printf("Terminal interface selected.\n");
        gamemode_menu(1);
    }
    else if (choice_interface == 2)
    {
        // Graphical interface
        printf("Graphical interface selected.\n");
        gamemode_menu(2);
    }
    else
    {
        printf("Invalid choice. Exiting the game.\n");
    }
}