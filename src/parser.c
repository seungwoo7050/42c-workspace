#include "shell.h"

#include <stdlib.h>

static void free_commands(t_command *cmd);

static void set_error(char **error, const char *message)
{
    if (error != NULL && *error == NULL)
        *error = sh_strdup(message);
}

static t_command *new_command(void)
{
    return (t_command *)sh_calloc(1, sizeof(t_command));
}

static t_pipeline *new_pipeline(void)
{
    t_pipeline *pipeline;

    pipeline = (t_pipeline *)sh_calloc(1, sizeof(t_pipeline));
    if (pipeline != NULL)
        pipeline->next_op = CONN_NONE;
    return pipeline;
}

static size_t word_count(char **argv)
{
    size_t n;

    n = 0;
    while (argv != NULL && argv[n] != NULL)
        n++;
    return n;
}

static int add_arg(t_command *cmd, const char *text)
{
    size_t  n;
    char    **next;
    char    *copy;
    size_t  i;

    n = word_count(cmd->argv);
    next = (char **)sh_calloc(n + 2, sizeof(char *));
    if (next == NULL)
        return 1;
    copy = sh_strdup(text);
    if (copy == NULL) {
        free(next);
        return 1;
    }
    i = 0;
    while (i < n) {
        next[i] = cmd->argv[i];
        i++;
    }
    next[n] = copy;
    free(cmd->argv);
    cmd->argv = next;
    cmd->argc = n + 1;
    return 0;
}

static int add_redir(t_command *cmd, t_redir_type type, const char *target,
        int target_quoted)
{
    t_redir *node;
    t_redir *tail;

    node = (t_redir *)sh_calloc(1, sizeof(t_redir));
    if (node == NULL)
        return 1;
    node->target = sh_strdup(target);
    if (node->target == NULL) {
        free(node);
        return 1;
    }
    node->type = type;
    node->heredoc_quoted = (type == REDIR_HEREDOC && target_quoted);
    if (cmd->redirs == NULL) {
        cmd->redirs = node;
        return 0;
    }
    tail = cmd->redirs;
    while (tail->next != NULL)
        tail = tail->next;
    tail->next = node;
    return 0;
}

static int command_empty(t_command *cmd)
{
    return (cmd == NULL || (cmd->argc == 0 && cmd->redirs == NULL
            && cmd->subshell == NULL));
}

static void append_command(t_pipeline *pipeline, t_command *cmd)
{
    t_command *tail;

    if (pipeline->commands == NULL) {
        pipeline->commands = cmd;
        pipeline->command_count++;
        return;
    }
    tail = pipeline->commands;
    while (tail->next != NULL)
        tail = tail->next;
    tail->next = cmd;
    pipeline->command_count++;
}

static void append_pipeline(t_pipeline **head, t_pipeline **tail,
        t_pipeline *node)
{
    if (*head == NULL)
        *head = node;
    else
        (*tail)->next = node;
    *tail = node;
}

static int token_is_redir(t_token_type type)
{
    return (type == TOK_REDIR_IN || type == TOK_REDIR_OUT
        || type == TOK_REDIR_APPEND || type == TOK_HEREDOC);
}

static t_redir_type redir_type(t_token_type type)
{
    if (type == TOK_REDIR_OUT)
        return REDIR_OUT;
    if (type == TOK_REDIR_APPEND)
        return REDIR_APPEND;
    if (type == TOK_HEREDOC)
        return REDIR_HEREDOC;
    return REDIR_IN;
}

static t_connector connector_type(t_token_type type)
{
    if (type == TOK_AND)
        return CONN_AND;
    if (type == TOK_OR)
        return CONN_OR;
    return CONN_SEQ;
}

#define MAX_GROUP_DEPTH 64

typedef struct s_parse {
    t_token *cur;
    char    **error;
    int     depth;
}   t_parse;

static t_pipeline *parse_list(t_parse *p, int in_group);

static void *parse_error(t_parse *p, const char *message)
{
    set_error(p->error, message);
    return NULL;
}

static int token_is(const t_parse *p, t_token_type type)
{
    return (p->cur != NULL && p->cur->type == type);
}

static int parse_group(t_parse *p, t_command *cmd)
{
    if (cmd->argc != 0 || cmd->redirs != NULL || cmd->subshell != NULL) {
        parse_error(p, "syntax error: unexpected '('");
        return 1;
    }
    if (p->depth >= MAX_GROUP_DEPTH) {
        parse_error(p, "syntax error: group nesting too deep");
        return 1;
    }
    p->cur = p->cur->next;
    p->depth++;
    cmd->subshell = parse_list(p, 1);
    p->depth--;
    if (cmd->subshell == NULL)
        return 1;
    if (!token_is(p, TOK_RPAREN)) {
        parse_error(p, "syntax error: missing ')'");
        return 1;
    }
    p->cur = p->cur->next;
    return 0;
}

static int parse_redirection(t_parse *p, t_command *cmd)
{
    if (p->cur->next == NULL || p->cur->next->type != TOK_WORD) {
        parse_error(p, "syntax error: redirection target missing");
        return 1;
    }
    if (add_redir(cmd, redir_type(p->cur->type), p->cur->next->text,
            p->cur->next->quoted) != 0) {
        parse_error(p, "allocation failure");
        return 1;
    }
    p->cur = p->cur->next->next;
    return 0;
}

/* command := '(' list ')' redirs* | word-or-redir+ ; '(' 에서 parse_list로 재귀한다. */
static t_command *parse_command(t_parse *p)
{
    t_command   *cmd;
    int         failed;

    cmd = new_command();
    if (cmd == NULL)
        return parse_error(p, "allocation failure");
    failed = 0;
    while (p->cur != NULL && !failed) {
        if (p->cur->type == TOK_WORD) {
            if (cmd->subshell != NULL) {
                parse_error(p, "syntax error: unexpected word after group");
                failed = 1;
            } else if (add_arg(cmd, p->cur->text) != 0) {
                parse_error(p, "allocation failure");
                failed = 1;
            } else
                p->cur = p->cur->next;
        } else if (token_is_redir(p->cur->type))
            failed = parse_redirection(p, cmd);
        else if (p->cur->type == TOK_LPAREN)
            failed = parse_group(p, cmd);
        else
            break;
    }
    if (failed) {
        free_commands(cmd);
        return NULL;
    }
    return cmd;
}

static const char *empty_command_message(const t_parse *p, int after_pipe)
{
    if (token_is(p, TOK_PIPE))
        return "syntax error: empty command before pipe";
    if (after_pipe)
        return "syntax error: expected command after pipe";
    if (token_is(p, TOK_RPAREN) && p->depth == 0)
        return "syntax error: unexpected ')'";
    return "syntax error: empty command before connector";
}

/* pipeline := command ( '|' command )* */
static t_pipeline *parse_pipeline(t_parse *p)
{
    t_pipeline  *pipeline;
    t_command   *cmd;
    int         after_pipe;

    pipeline = new_pipeline();
    if (pipeline == NULL)
        return parse_error(p, "allocation failure");
    after_pipe = 0;
    for (;;) {
        cmd = parse_command(p);
        if (cmd == NULL) {
            free_pipeline(pipeline);
            return NULL;
        }
        if (command_empty(cmd)) {
            free_commands(cmd);
            free_pipeline(pipeline);
            return parse_error(p, empty_command_message(p, after_pipe));
        }
        append_command(pipeline, cmd);
        if (!token_is(p, TOK_PIPE))
            return pipeline;
        p->cur = p->cur->next;
        after_pipe = 1;
    }
}

/* list := pipeline ( ('&&' | '||' | ';') pipeline )* ';'? */
static t_pipeline *parse_list(t_parse *p, int in_group)
{
    t_pipeline      *head;
    t_pipeline      *tail;
    t_pipeline      *pipeline;
    t_token_type    connector;

    head = NULL;
    tail = NULL;
    if (p->cur == NULL)
        return in_group ? parse_error(p, "syntax error: missing ')'") : NULL;
    if (in_group && token_is(p, TOK_RPAREN))
        return parse_error(p, "syntax error: empty group");
    for (;;) {
        pipeline = parse_pipeline(p);
        if (pipeline == NULL) {
            free_pipeline(head);
            return NULL;
        }
        append_pipeline(&head, &tail, pipeline);
        if (p->cur == NULL || (in_group && token_is(p, TOK_RPAREN)))
            return head;
        if (token_is(p, TOK_RPAREN)) {
            free_pipeline(head);
            return parse_error(p, "syntax error: unexpected ')'");
        }
        connector = p->cur->type;
        pipeline->next_op = connector_type(connector);
        p->cur = p->cur->next;
        if (p->cur == NULL || (in_group && token_is(p, TOK_RPAREN))) {
            if (connector == TOK_SEQ) {
                pipeline->next_op = CONN_NONE;
                return head;
            }
            free_pipeline(head);
            return parse_error(p, "syntax error: conditional operator needs "
                "a following pipeline");
        }
    }
}

t_pipeline *parse_tokens(t_token *tokens, char **error)
{
    t_parse p;

    p.cur = tokens;
    p.error = error;
    p.depth = 0;
    if (error != NULL)
        *error = NULL;
    return parse_list(&p, 0);
}

static void free_redirs(t_redir *redir)
{
    t_redir *next;

    while (redir != NULL) {
        next = redir->next;
        free(redir->target);
        free(redir);
        redir = next;
    }
}

static void free_commands(t_command *cmd)
{
    t_command *next;

    while (cmd != NULL) {
        next = cmd->next;
        sh_free_words(cmd->argv);
        free_redirs(cmd->redirs);
        free_pipeline(cmd->subshell);
        free(cmd);
        cmd = next;
    }
}

void free_pipeline(t_pipeline *pipeline)
{
    t_pipeline *next;

    while (pipeline != NULL) {
        next = pipeline->next;
        free_commands(pipeline->commands);
        free(pipeline);
        pipeline = next;
    }
}

static size_t count_pipelines(t_pipeline *pipeline)
{
    size_t count;

    count = 0;
    while (pipeline != NULL) {
        count++;
        pipeline = pipeline->next;
    }
    return count;
}

void shell_sequence_init(t_sequence *sequence)
{
    if (sequence == NULL)
        return;
    sequence->pipelines = NULL;
    sequence->pipeline_count = 0;
}

void shell_sequence_free(t_sequence *sequence)
{
    if (sequence == NULL)
        return;
    free_pipeline(sequence->pipelines);
    sequence->pipelines = NULL;
    sequence->pipeline_count = 0;
}

int shell_parse_line(const char *line, t_sequence *sequence, char **error)
{
    t_token  *tokens;
    char     *internal_error;
    char     **error_slot;

    internal_error = NULL;
    error_slot = error != NULL ? error : &internal_error;
    if (sequence == NULL) {
        set_error(error_slot, "parse output is null");
        free(internal_error);
        return 1;
    }
    shell_sequence_init(sequence);
    tokens = tokenize_line(line, error_slot);
    if (*error_slot != NULL) {
        free(internal_error);
        return 1;
    }
    sequence->pipelines = parse_tokens(tokens, error_slot);
    free_tokens(tokens);
    if (*error_slot != NULL) {
        shell_sequence_free(sequence);
        free(internal_error);
        return 1;
    }
    sequence->pipeline_count = count_pipelines(sequence->pipelines);
    free(internal_error);
    return 0;
}

int shell_execute_sequence(const t_sequence *sequence, t_env *env,
        int *last_status, const t_executor_hooks *hooks, void *ctx)
{
    const t_pipeline *pipeline;
    t_connector      gate;
    int              status;
    int              should_run;

    if (hooks == NULL || hooks->run_pipeline == NULL) {
        if (hooks != NULL && hooks->on_error != NULL)
            hooks->on_error("missing executor pipeline hook", ctx);
        if (last_status != NULL)
            *last_status = 1;
        return 1;
    }
    status = last_status != NULL ? *last_status : 0;
    gate = CONN_NONE;
    pipeline = sequence != NULL ? sequence->pipelines : NULL;
    while (pipeline != NULL) {
        should_run = 1;
        if (gate == CONN_AND && status != 0)
            should_run = 0;
        if (gate == CONN_OR && status == 0)
            should_run = 0;
        if (should_run)
            status = hooks->run_pipeline(pipeline, env, ctx);
        gate = pipeline->next_op;
        pipeline = pipeline->next;
    }
    if (last_status != NULL)
        *last_status = status;
    return status;
}
