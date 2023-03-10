/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "test_shared.h"


volatile lp_core_test_commands_t main_cpu_command = LP_CORE_NO_COMMAND;
volatile lp_core_test_command_reply_t main_cpu_reply = LP_CORE_COMMAND_INVALID;
volatile lp_core_test_commands_t command_resp = LP_CORE_NO_COMMAND;
volatile uint32_t test_data_in = 0;
volatile uint32_t test_data_out = 0;
volatile uint32_t counter = 0;

volatile uint32_t incrementer = 0;

void handle_commands(lp_core_test_commands_t cmd)
{
    counter++;

    switch (cmd) {
        case LP_CORE_READ_WRITE_TEST:
            /* Echo the command ID back to the main CPU */
            command_resp = LP_CORE_READ_WRITE_TEST;

            /* Process test data */
            test_data_out = test_data_in ^ XOR_MASK;

            /* Set the command reply status */
            main_cpu_reply = LP_CORE_COMMAND_OK;
            break;

        case LP_CORE_NO_COMMAND:
            main_cpu_reply = LP_CORE_COMMAND_OK;
            break;

        default:
            main_cpu_reply = LP_CORE_COMMAND_NOK;
            break;
    }
}

int main (void)
{
    while (1) {
        handle_commands(main_cpu_command);
    }

    return 0;
}
