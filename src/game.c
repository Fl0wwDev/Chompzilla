#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <arpa/inet.h>
#include "utils/main_menu_utils.h"
#include "game_logic/game_logic.h"
#include "server/server.h"
#include "client/client.h"
#include "gui/gui_gamemode.h"
#include "gui/client_gui.h"
#include "gui/server_gui.h"

/**
 * @brief The main function
 *
 * @param argc The number of arguments
 * @param argv The arguments
 * @return int The return code
 */
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        /* List of arguments
        1. -h or --help: Display the help message
        2. -g or --gui: Launch the GUI
        3. -l or --local: Launch the local game
        4. -s or --server: Launch the server
        5. -ia or --ia: Launch the IA
        6. -c or --client: Launch the client (with the server IP address after the argument)
        */

        bool help = false;
        bool gui = false;
        bool local = false;
        bool server = false;
        char *server_port = NULL;
        bool ia = false;
        bool client = false;
        char *server_ip = NULL;

        // First we get the arguments
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                help = true;
            }
            else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gui") == 0)
            {
                gui = true;
            }
            else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--local") == 0)
            {
                local = true;
            }
            else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--server") == 0)
            {
                server = true;
                // We search in all the arguments if we have the port number
                for (int j = i + 1; j < argc; j++)
                {
                    if (verify_port(argv[j]))
                    {
                        server_port = argv[j];
                        break;
                    }
                }
                if (server_port == NULL)
                {
                    printf("No port number provided. Please provide a port number between 1024 and 65535. Exiting the game.\n");
                    return 1;
                }
            }
            else if (strcmp(argv[i], "-ia") == 0 || strcmp(argv[i], "--ia") == 0)
            {
                ia = true;
            }
            else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--client") == 0)
            {
                client = true;
                // We search in all the arguments if we have the server IP address
                for (int j = i + 1; j < argc; j++)
                {
                    if (verify_ip_port(argv[j]))
                    {
                        server_ip = argv[j];
                        break;
                    }
                }
                if (server_ip == NULL)
                {
                    printf("No server IP address or format is incorrect. Please use the format hhh.hhh.hhh.hhh:pppp. Exiting the game.\n");
                    return 1;
                }
            }
        }

        if (help)
        {
            help_menu();
            return 0;
        }
        else if (gui)
        {
            // local game with GUI
            if (local)
            {
                play_local_game_gui();
                return 0;
            }
            else if (server)
            {
                // We verify that we have a port number
                if (server_port != NULL)
                {
                    // We verify that the port number is correct;
                    if (!verify_port(server_port))
                    {
                        printf("Invalid port number. Please choose a number between 1024 and 65535. Exiting the game.\n");
                        return 1;
                    }

                    unsigned short port = atoi(server_port);

                    // Launch the server from the GUI
                        network_server_gui(port);
                    return 0;
                }
                else
                {
                    printf("No port number provided. Exiting the game.\n");
                    return 1;
                }
                return 0;
            }
            else if (client)
            {
                // Verify that we have the server IP address and the port
                if (server_ip != NULL)
                {
                    if (!verify_ip_port(server_ip))
                    {
                        printf("Invalid IP address. Please use the format hhh.hhh.hhh.hhh:pppp. Exiting the game.\n");
                        return 1;
                    }

                    // We cut the IP address and the port
                    char *ip = strtok(server_ip, ":");
                    char *port = strtok(NULL, ":");
                    unsigned short server_port = atoi(port);

                    // Launch the client from the GUI
                    network_client_gui(ip, server_port);
                    return 0;
                }
                else
                {
                    printf("No server IP address provided. Exiting the game.\n");
                    return 1;
                }
            }
            else
            {
                launch_gui_gamemode(0, NULL);
                return 0;
            }
        }
        else if (local)
        {
            if (ia)
            {
                play_local_game_ia();
                return 0;
            }
            else
            {
                play_local_game();
                return 0;
            }
        }
        else if (server)
        {
            // We verify that we have the port number
            if (server_port != NULL)
            {
                // We verify that the port number is correct;
                if (!verify_port(server_port))
                {
                    printf("Invalid port number. Please choose a number between 1024 and 65535. Exiting the game.\n");
                    return 1;
                }

                unsigned short port = atoi(server_port);

                if (ia)
                {
                    // Launch the server with IA
                    play_network_server_ia(port);
                }
                else
                {
                    // Launch the server
                    play_network_server(port);
                }
                return 0;
            }
            else
            {
                printf("No port number provided. Exiting the game.\n");
                return 1;
            }
        }
        else if (client)
        {
            // We verify that we have the server IP address
            if (server_ip != NULL)
            {
                if (!verify_ip_port(server_ip))
                {
                    printf("Invalid IP address. Please use the format hhh.hhh.hhh.hhh:pppp. Exiting the game.\n");
                    return 1;
                }

                // We cut the IP address and the port
                char *ip = strtok(server_ip, ":");
                char *port = strtok(NULL, ":");
                unsigned short server_port = atoi(port);

                if (ia)
                {
                    printf("Launching the IA client\n");
                    // Launch the IA client
                    play_network_ia_client(ip, server_port);
                }
                else
                {

                    // Launch the client
                    play_network_client(ip, server_port);
                }

                return 0;
            }
            else
            {
                printf("No server IP address provided. Exiting the game.\n");
                return 1;
            }
        }
        else
        {
            printf("Invalid arguments. Exiting the game.\n");
            return 1;
        }
    }
    else
    {
        game_menu(); // Launch the main menu
        return 0;
    }
}