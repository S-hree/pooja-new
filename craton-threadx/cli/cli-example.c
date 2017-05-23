/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include <libcli.h>

#include <craton/cli.h>

/* 
  CRATON ThreadX CLI Example 
  
  This example demonstrates basic usage of CRATON CLI API and libcli's API
  for code running on top of a CRATON processor with ThreadX RTOS.

  A CLI instance is retrieved via CRATON CLI API. The instance is used to
  register a basic CLI command which prints the message "Example command
  executed successfully" to the console when invoked.
*/

/** Example command to register */
int cmd_example(cli_instance_t *cli, const char *command,
                char *argv[], int argc)
{
  /* Unused parameters */
  (void)command;
  (void)argv;
  (void)argc;

  cli_print(cli, "%s", "Example command executed successfully.");

  return CLI_OK;
}

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* CLI instance */
  cli_instance_t *cli = NULL;
  /* CLI command handle */
  struct cli_command *command = NULL;
  /* CLI instance type */
  cli_instance_type_t type;

  /* Register command on all CLI instances */
  for (type = CLI_INSTANCE_TYPE_MIN; type <= CLI_INSTANCE_TYPE_MAX; ++type) {
    /* Get CLI instance */
    rc = cli_instance_get(&cli, type);
    if (atlk_error(rc)) {
      fprintf(stderr, "cli_instance_get failed: %d\n", rc);
      continue;
    }

    /* Register example command */
    command = cli_register_command(cli, NULL, "example", cmd_example,
                                   PRIVILEGE_UNPRIVILEGED, MODE_ANY,
                                   "Example command");
    if (command == NULL) {
      fprintf(stderr, "cli_register_command returned NULL\n");
    }
  }

  return;
}
