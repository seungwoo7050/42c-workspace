#include "shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int check_line(const char *name, const char *line, int expect_failure,
        int request_error)
{
    t_sequence  sequence;
    char        *error;
    int         result;

    error = NULL;
    result = shell_parse_line(line, &sequence,
            request_error ? &error : NULL);
    if ((result != 0) != expect_failure) {
        fprintf(stderr, "not ok - %s: result=%d\n", name, result);
        free(error);
        if (result == 0)
            shell_sequence_free(&sequence);
        return 1;
    }
    if (request_error && ((error != NULL) != expect_failure)) {
        fprintf(stderr, "not ok - %s: unexpected error state\n", name);
        free(error);
        if (result == 0)
            shell_sequence_free(&sequence);
        return 1;
    }
    free(error);
    if (result == 0)
        shell_sequence_free(&sequence);
    return 0;
}

int main(void)
{
    int failed;

    failed = 0;
    failed |= check_line("valid command", "echo ok", 0, 1);
    failed |= check_line("empty input", "", 0, 0);
    failed |= check_line("pipe without error output", "echo |", 1, 0);
    failed |= check_line("quote without error output", "echo 'open", 1, 0);
    failed |= check_line("operator with error output", "echo &", 1, 1);
    failed |= check_line("group in pipeline", "(echo a && echo b) | cat", 0, 1);
    failed |= check_line("nested group", "((echo a); echo b) || echo c", 0, 1);
    failed |= check_line("group with redirection", "(cat) > out < in", 0, 1);
    failed |= check_line("empty group", "()", 1, 1);
    failed |= check_line("missing close paren", "(echo a", 1, 1);
    failed |= check_line("stray close paren", "echo a)", 1, 1);
    failed |= check_line("word after group", "(echo a) echo b", 1, 1);
    {
        char deep[200];
        int  i;

        for (i = 0; i < 70; i++) {
            deep[i] = '(';
            deep[70 + 6 + i] = ')';
        }
        memcpy(deep + 70, "echo a", 6);
        deep[146] = '\0';
        failed |= check_line("group nesting limit", deep, 1, 1);
    }
    if (shell_parse_line("echo ok", NULL, NULL) == 0) {
        fprintf(stderr, "not ok - null parse output accepted\n");
        failed = 1;
    }
    if (failed)
        return 1;
    puts("ok - parser api");
    return 0;
}
