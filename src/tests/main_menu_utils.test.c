#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <criterion/criterion.h>
#include "../inc/const.h"
#include "../utils/main_menu_utils.h"

/**
 * @brief Test the existence of the help_menu function
 */
Test(main_menu_utils, help_menu_exists)
{
    cr_assert_not_null(help_menu, "help_menu does not exist");
}

/**
 * @brief Test if the help_menu function output a string in the stdout
 */
Test(main_menu_utils, help_menu_prints)
{
    // We redirect the stdout to a file
    freopen("/tmp/help_menu_prints", "w+", stdout);
    help_menu();
    // We redirect the stdout to the console
    freopen("/dev/tty", "w", stdout);

    // We open the file
    FILE *f = fopen("/tmp/help_menu_prints", "r");
    char buffer[255];
    // We read
    fgets(buffer, 255, f);
    // We close the file
    fclose(f);

    // We check if the string is not empty
    cr_assert_str_not_empty(buffer, "help_menu does not print anything");
}

/**
 * @brief Test the existence of the ask_server_ip_port function
 */
// Test for function verify_port
Test(verify_port_tests, valid_ports) {
    cr_assert_eq(verify_port("1024"), true, "Port 1024 should be valid");
    cr_assert_eq(verify_port("65535"), true, "Port 65535 should be valid");
    cr_assert_eq(verify_port("2048"), true, "Port 2048 should be valid");
}

Test(verify_port_tests, invalid_ports) {
    cr_assert_eq(verify_port("1023"), false, "Port 1023 should be invalid");
    cr_assert_eq(verify_port("65536"), false, "Port 65536 should be invalid");
    cr_assert_eq(verify_port("abcd"), false, "Non-numeric port should be invalid");
    cr_assert_eq(verify_port(""), false, "Empty port should be invalid");
}

// Test for function verify_ip_port
Test(verify_ip_port_tests, valid_ip_ports) {
    cr_assert_eq(verify_ip_port("192.168.1.1:8080"), true, "IP and port 192.168.1.1:8080 should be valid");
    cr_assert_eq(verify_ip_port("127.0.0.1:1234"), true, "IP and port 127.0.0.1:1234 should be valid");
}
