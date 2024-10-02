#include <stdio.h>
#include <criterion/criterion.h>
#include "../server/server.h"

/**
 * @brief Test to check if the handle_client_move function exists
 */
Test(server, handle_client_move_exists){
    cr_assert_not_null(handle_client_move, "handle_client_move does not exist");
}

/**
 * @brief Test if close_sockets exists
 */

Test(server, close_sockets_exists){
    cr_assert_not_null(close_sockets, "close_sockets does not exist");
}

/**
 * @brief Test if close_sockets closes the sockets
 */
Test(server, close_sockets_closes_sockets){
    extern int server_fd;
    extern int new_socket;
    close_sockets();
    cr_assert_eq(new_socket, -1, "new_socket is not closed");
    cr_assert_eq(server_fd, -1, "server_fd is not closed");
}

/**
 * @brief Test if handle_sigint exists
 */
Test(server, handle_sigint_exists){
    cr_assert_not_null(handle_sigint, "handle_sigint does not exist");
}

/**
 * @brief Test if handle_sigint closes the sockets
 */
Test(server, handle_sigint_closes_sockets){
    extern int server_fd;
    extern int new_socket;
    handle_sigint(0);
    cr_assert_eq(new_socket, -1, "new_socket is not closed");
    cr_assert_eq(server_fd, -1, "server_fd is not closed");
}

/**
 * @brief Test if play_network_server exists
 */
Test(server, play_network_server_exists){
    cr_assert_not_null(play_network_server, "play_network_server does not exist");
}

/**
 * @brief Test if launch_server_thread exists
 */
Test(server, launch_server_thread_exists){
    cr_assert_not_null(launch_server_thread, "launch_server_thread does not exist");
}

/**
 * @brief Test if start_server_from_gui exists
 */
Test(server, start_server_from_gui_exists){
    cr_assert_not_null(start_server_from_gui, "start_server_from_gui does not exist");
}

