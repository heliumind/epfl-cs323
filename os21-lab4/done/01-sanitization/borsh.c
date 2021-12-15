#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> /* for ssize_t */
#include <stdlib.h>

#define MAX_CMD_LENGTH 128U
#define MAX_CMD_HISTORY 8U
#define MAX_VARNAME_LENGTH 8U
#define MAX_VARVALUE_LENGTH 32U

typedef char command_t[MAX_CMD_LENGTH], *pcommand_t;

#ifdef ASAN_FIX_1
typedef struct {
    command_t wizard_command;
    unsigned int length;
    command_t commands[MAX_CMD_HISTORY];
} history_t, *phistory_t;
#else
typedef struct {
    command_t commands[MAX_CMD_HISTORY];
    command_t wizard_command;
    unsigned int length;
} history_t, *phistory_t;
#endif

typedef struct varnode {
    char name[MAX_VARNAME_LENGTH];
    char value[MAX_VARVALUE_LENGTH];
    struct varnode *next;
} variable_t, *pvariable_t;

void init_history(phistory_t hist)
{
    bzero(hist->commands, sizeof(hist->commands));
    strcpy(hist->wizard_command, "Who Dares Disturb My Slumber!");
    hist->length = 0;
}

int prepend_history(phistory_t hist, const pcommand_t cmd)
{
    if (hist == NULL || cmd == NULL) {
        return -1;
    }
    if (strnlen(cmd, MAX_CMD_LENGTH) == MAX_CMD_LENGTH) {
        /* command is too long */
        return -2;
    }
    if (hist->length > 0) {
        if (strcmp(hist->commands[0], cmd) == 0) {
            /* new command is the same as the last command, return success */
            return 0;
        }

        /* move all commands up by one */
        #ifdef PATCH_1
        if (hist->length == MAX_CMD_HISTORY) {
            for (size_t i = hist->length - 1; i > 0; i--)
                strncpy(hist->commands[i], hist->commands[i - 1], MAX_CMD_LENGTH);
        } else {
            for (unsigned int i = hist->length; i > 0; --i) 
                strncpy(hist->commands[i], hist->commands[i - 1], MAX_CMD_LENGTH);
        }
        #else
        for (unsigned int i = hist->length; i > 0; --i) {
            strncpy(hist->commands[i], hist->commands[i - 1], MAX_CMD_LENGTH);
        }
        #endif
    }

    /* prepend new command */
    strcpy(hist->commands[0], cmd);
    hist->length += (unsigned int)(hist->length < MAX_CMD_HISTORY);

    return 0;
}

void init_variables(pvariable_t *head)
{
    *head = NULL;
}

int put_variable(pvariable_t *head, char *name, char *value)
{
    if (head == NULL || name == NULL || value == NULL) {
        return -1;
    }
    if (strnlen(name, MAX_VARNAME_LENGTH) == MAX_VARNAME_LENGTH ||
        strnlen(value, MAX_VARVALUE_LENGTH) == MAX_VARVALUE_LENGTH) {
        /* variable name or value is too long */
        return -2;
    }

    /* Check if variable exists */
    pvariable_t node;
    for (node = *head; node != NULL; node = node->next) {
        if (strcmp(name, node->name) == 0) {
            break;
        }
    }

    if (node == NULL) {
        /* Create new variable node */
        node = calloc(1, sizeof(variable_t));
        strcpy(node->name, name);
        node->next = NULL;

        /* Insert new node into list */
        if (*head == NULL) {
            *head = node;
        } else {
            #ifdef PATCH_3
            /* Find last variable in list */
            pvariable_t last;
            for (pvariable_t cur = *head; cur != NULL; cur = cur->next)
                last = cur;
            last->next = node;
            #else
            (*head)->next = node;
            #endif
        }
    }

    strcpy(node->value, value);

    return 0;
}

int get_variable(const pvariable_t head, const char *name, const char **value)
{
    if (head == NULL || name == NULL || value == NULL) {
        return -1;
    }
    if (strnlen(name, MAX_VARNAME_LENGTH) == MAX_VARNAME_LENGTH) {
        /* variable name is too long */
        return -2;
    }
    for (pvariable_t node = head; node != NULL; node = node->next) {
        if (strcmp(name, node->name) == 0) {
            *value = node->value;
            return 0;
        }
    }
    /* variable does not exist */
    return -3;
}

void deinit_variables(pvariable_t *head)
{
    pvariable_t tmp;
    for (pvariable_t node = *head; node != NULL; node = tmp) {
        tmp = node->next;
        free(node);
    }
    *head = NULL;
}

ssize_t remove_extra_spaces(char *str)
{
    size_t offset = 0;
    char *tmp = strdup(str);
    char *tok = strtok(tmp, " ");
    if (tok == NULL) {
        free(tmp);
        return 0;
    }
    do {
        size_t toklen = strlen(tok);
        memcpy(&str[offset], tok, toklen);
        offset += toklen;

        /* append space after token */
        str[offset++] = ' ';
    } while ((tok = strtok(NULL, " ")) != NULL);
    str[--offset] = '\0';
    free(tmp);
    return offset;
}

ssize_t get_command(const pvariable_t vars, pcommand_t *cmd, size_t *cmd_size)
{
    fputs("# ", stdout);
    ssize_t cmd_len = getline(cmd, cmd_size, stdin);
    if (cmd_len != -1) {
        /* remove newline from end of cmd */
        ssize_t terminator = cmd_len - 1;
        if ((*cmd)[terminator] == '\n') {
            (*cmd)[terminator] =  '\0';
        }

        /**** Handle variable expansion ****
         * In order to save space, we will perform variable expansion in-place.
         * Whenever a variable is encountered in the middle of a command, we
         * replace it by its stored value (if found).
         * To avoid corruption of the rest of the command, we relocate the part
         * after the variable token depending on the length of the token and the
         * length of the variable's value.
         * NOTE: If the expansion of _any_ variable fails, the original command
         *  must be returned AS IS (without any of the variables being expanded)
         */

        /* remove duplicate spaces from command */
        cmd_len = remove_extra_spaces(*cmd);

        if (cmd_len == 0) {
            return 0;
        }

        /* copy the cmd for tokenization */
        char *tmp = strdup(*cmd);

        /* split the cmd by the space delimiter */
        char *tok = strtok(tmp, " ");

        /* This variable stores the offset of the *next* token in the modified
         * command. We use it to keep track of where the next token is, in case
         * the size of the command changes after expanding a variable.
         */
        size_t offset = 0;

        /* The first token in a command must not be expanded. Otherwise,
         * assignment statements would be mis-interpreted.
         * For instance, if a command is `$foo=bar`, expanding $foo into its
         * value would not yield expected behavior.
         */
        int has_first_token = 0;

        do {
            if (tok[0] != '$' || has_first_token == 0) {
                /* This is a non-variable token, or the first token. */
                offset += strlen(tok) + 1;
                has_first_token = 1;
            } else {
                /* We need to expand the variable in the command.
                 * First, store the value of the variable (if it exists).
                 */
                const char *value;
                if (get_variable(vars, &tok[1], &value) == 0) {
                    /* To expand the variable, we need to move everything after
                     * the variable token in the command.
                     * So first, we calculate the offset where to start the move
                     * at.
                     * This is the offset of the current token (variable), added
                     * to the token's length. Notice that this offset *includes*
                     * the space delimiter, since we need to move the space as
                     * well.
                     */
                    size_t move_src = offset + strlen(tok);

                    /* The length of the moved part is just the length of the
                     * string from the move source until the end of the command.
                     */
                    size_t move_length = strlen(&(*cmd)[move_src]) + 1;

                    /* The destination of the move is the offset of the variable
                     * token added to its length.
                     */
                    size_t move_dst = offset + strlen(value);

                    
                    #ifdef PATCH_2
                    if (move_dst > *cmd_size) {
                        size_t add = cmd_len - strlen(tok) + strlen(value) - *cmd_size + 1; 
                        *cmd = realloc(*cmd, *cmd_size + add);
                    }
                    memmove(&(*cmd)[move_dst], &(*cmd)[move_src], move_length);
                    #else
                    /* Perform the move operation in `cmd`. */
                    memmove(&(*cmd)[move_dst], &(*cmd)[move_src], move_length);
                    #endif

                    /* Now we insert the value of the variable in its place. */
                    memcpy(&(*cmd)[offset], value, strlen(value));

                    /* The offset of the *next* token is the offset of the
                     * destination, skipping one character for the delimiter.
                     */
                    offset = move_dst + 1;

                    /* The new length of the command is the re-adjusted.*/
                    cmd_len = cmd_len - strlen(tok) + strlen(value);
                } else {
                    /* The variable is not found; do not expand it. */
                    offset += strlen(tok) + 1;
                }
            }
        } while((tok = strtok(NULL, " ")) != NULL);
        free(tmp);
    }
    return cmd_len;
}

int main(int argc, char **argv)
{
    /* Print fabulous Bork Shell logo */
    fputs(
"                           ;\\               \n"
"                          |' \\              \n"
"       _                  ; : ;              \n"
"      / `-.              /: : |              \n"
"     |  ,-.`-.          ,': : |              \n"
"     \\  :  `. `.       ,'-. : |             \n"
"      \\ ;    ;  `-.__,'    `-.|             \n"
"       \\ ;   ;  :::  ,::'`:.  `.            \n"
"        \\ `-. :  `    :.    `.  \\          \n"
"         \\   \\    ,   ;   ,:    (\\        \n"
"          \\   :., :.    ,'o)): ` `-.        \n"
"         ,/,' ;' ,::\"'`.`---'   `.  `-._    \n"
"       ,/  :  ; '\"      `;'          ,--`.  \n"
"      ;/   :; ;             ,:'     (   ,:)  \n"
"        ,.,:.    ; ,:.,  ,-._ `.     \\\"\"'/\n"
"        '::'     `:'`  ,'(  \\`._____.-'\"'  \n"
"           ;,   ;  `.  `. `._`-.  \\\\       \n"
"           ;:.  ;:       `-._`-.\\  \\`.     \n"
"            '`:. :        |' `. `\\  ) \\    \n"
"    -bork-     ` ;:       |    `--\\__,'     \n"
"                 '`      ,'                  \n"
"                      ,-'                    \n"
"                                             \n"
"                                             \n"
"                                             \n"
"                                             \n"
"      Welcome to Borsh, the Bork Shell!      \n"
"                                             \n"
"                                             \n"
    , stdout);

    /* Initialize command history */
    history_t hist;
    init_history(&hist);

    /* Initialize variables */
    pvariable_t vars;
    init_variables(&vars);

    char *cmd = NULL;
    size_t cmd_size = 0;
    ssize_t cmd_len;

    while ((cmd_len = get_command(vars, &cmd, &cmd_size)) != -1) {
        if (cmd_len == 0) {
            continue;
        }
        if (prepend_history(&hist, cmd) != 0) {
            fprintf(stderr, "borsh: Command is too long\n");
            continue;
        }

        if (strcmp(cmd, "help") == 0) {
            fprintf(stdout, "List of available commands:\n");
            fprintf(stdout, "\tbark - expands variables and prints (echo)\n");
            fprintf(stdout, "\tfetch - lists the command history\n");
            fprintf(stdout, "\theck - summon the wizard of histories\n");
            fprintf(stdout, "\tpoop - prints the stored variables\n");
            fprintf(stdout, "\thelp - displays this help message\n");
            fprintf(stdout, "\texit - ends the borsh session\n");
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "fetch") == 0) {
            for (int i = hist.length - 1; i >= 0; --i) {
                fprintf(stdout, "%s\n", hist.commands[i]);
            }
        } else if (strcmp(cmd, "heck") == 0) {
            fprintf(stdout, "%s\n", hist.wizard_command);
        } else if (strcmp(cmd, "poop") == 0) {
            for (pvariable_t node = vars; node != NULL; node = node->next) {
                fprintf(stdout, "$%s=%s\n", node->name, node->value);
            }
        } else if (strncmp(cmd, "bark", 4) == 0) {
            fprintf(stdout, "%s\n", &cmd[5]);
        } else if (strncmp(cmd, "$", 1) == 0) {
            char *tmp = strdup(cmd);
            char *tok = strtok(tmp, "=");
            if (strlen(&tok[1]) == 0) {
                fprintf(stderr, "borsh: Variable name is invalid\n");
            } else if (strtok(NULL, "=") == NULL) {
                fprintf(stderr, "borsh: Variable value is invalid\n");
            } else {
                size_t offset = strlen(tok) + 1;
                if (put_variable(&vars, &tok[1], &cmd[offset]) != 0) {
                    fprintf(stderr, "borsh: Failed to assign variable\n");
                }
            }
            free(tmp);
        } else {
            fprintf(stderr, "borsh: Unknown command\n");
            continue;
        }
    }
    free(cmd);

    deinit_variables(&vars);
}
