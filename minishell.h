/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mresch <mresch@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 17:34:54 by ischmutz          #+#    #+#             */
/*   Updated: 2024/05/13 14:21:50 by mresch           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
//#include <cstddef>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <errno.h>
# include "minilib/libft.h"
# include <sys/types.h>
# include <sys/wait.h>
# include <signal.h>

//# include "src/parser/parse.h"

# ifndef BUFFER
#  define BUFFER 50
# endif

# ifndef BUFFER_INCREMENT
#  define BUFFER_INCREMENT 50
# endif

typedef enum type
{
	CMD = 0,
	ARG,
	PIPE,
	IN,
	OUT,
	APP,
	HEREDOC,
}	t_type;

typedef struct s_token
{
	char			*str;
	int				type;
	char			*dir;
	int				connected;
	int				distanced;
	int				quoted;
	struct s_token	*next;
	struct s_token	*prev;
}	t_token;

typedef struct s_pipe
{
	int		read;
	int		write;
}	t_pipe;

typedef struct s_command
{
	t_token				*cmd;
	t_token				*args;
	t_token				*input;
	t_token				*output;
	int					arg_num;
	int					pid;
	int					heredoc_fd;
	char				*tmpfile;
	char				**args_exec;
	struct s_command	*next;
	struct s_command	*prev;
}	t_command;

typedef struct s_env
{
	char			*var;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct exec
{
	char	*path;
	char	**paths;
}	t_exec;

typedef struct s_bigshell
{
	int			num_cmd;
	int			simple_error;
	int			pipe_fd[2];
	int			pipe_fd2[2];
	int			var_i;
	int			reference_i;
	int			std_in;
	int			std_out;
	int			fd_in;
	int			fd_out;
	int			heredoc_fd;
	t_exec		*exec;
	size_t		buffer_size;
	char		*cwd;
	char		*mod_cwd;
	char		**mod_env;
	char		**built_ins; //FREED
	t_pipe		*pipe;
	t_token		*heredoc;	//token with delimiter and str;
	t_command	*commands;
	t_env		*env; //FREED
	t_env		*s_env; //FREED
}	t_bigshell;

typedef struct s_signal
{
	int			sigint;
	int			sigquit;
	int			pid;
	t_bigshell	*data;
}	t_sig;

extern int	g_sig;
void	free_null(void **ptr);
int		ft_strcmp(const char *s1, const char *s2);

void	restore_fork(t_bigshell *data, int mode);
void	store_restore_fds(t_bigshell *data, int mode);
int		check_file(t_bigshell *data, const char *file, int mode);
int		redir(t_command *command, t_bigshell *data);

//PATH CONSTRUCTION FUNCTIONS:

char	**find_and_split_path(char **env);
char	*check_if_correct_path(char **paths, t_bigshell *data, char *str);

//BUILT-IN CHECKER FUNCTIONS:

char	*put_str(char *s);
void	put_built_in(int index, t_bigshell *data);
void	built_in_list(t_bigshell *data);
void	builtin_exec(t_bigshell *data, int builtin_index, t_command *cmd);
int		builtin_check_exec(t_bigshell *data, char *cmd, t_command *command);

int		builtin_allrounder(t_bigshell *data, t_command *current_command);

//REDIRECTION FUNCTIONS:

void	redir_error(t_bigshell *data, int exit_code, char *str);
void	error1(t_bigshell *data, char *str, int exit_code);
void	simple_error(t_bigshell *data, int exit_code);
void	exit_child(t_bigshell *data, int exit_stat);
void	critical_failure(t_bigshell *data, char *str);

//SINGLE CMD EXECUTION FUNCTIONS:

void	simple_exec(t_bigshell *data);

//PIPELINE FUNCTIONS:

void	pipe_init(t_bigshell *data);
void	restore_output(t_bigshell *data);
void	error_exit(t_bigshell *data, int status, char *msg);
void	close_first_pipe(t_bigshell *data, t_command *cmd);
void	wait_for_children(t_bigshell *data);

int		close_read(t_bigshell *data);
int		close_write(t_bigshell *data);
int		close_pipe(t_bigshell *data, int mode);
void	close_redir_fds(t_bigshell *data);
void	close_std_fds(t_bigshell *data);
void	close_redir_fds_in_child(t_bigshell *data);

void	execute_command(t_bigshell *data, t_command *cmd);
void	first_executor(t_bigshell *data, t_command *cmd, int out_fd);
void	middle_executor(t_bigshell *data, t_command *cmd, \
		int out_fd, int in_fd);
void	last_executor(t_bigshell *data, t_command *cmd, int in_fd);

void	complex_exec(t_bigshell *data);

//ECHO FUNCTIONS:

void	ft_echo(t_bigshell *data, t_token *args);

//CD FUNCTIONS:

void	overwrite_s_env(t_bigshell *data, char *env_var, char *str);
void	overwrite_oldpwd(t_bigshell *data, char *oldpwd);
void	overwrite_pwd(t_bigshell *data, char *new_path);
void	home_dir(t_bigshell *data, char *oldpwd);
char	*get_cwd(t_bigshell *data);
void	ft_cd(t_bigshell *data);

//PWD FUNCTIONS:

void	ft_pwd(t_bigshell *data, t_command *cmd);

//EXPORT FUNCTIONS:

void	switch_values(t_bigshell *data, t_env *node, \
		char *new_value, int len);
int		var_exists(t_bigshell *data, char *str);
int		check_var(t_bigshell *data, char *key);
void	make_copy(t_bigshell *data);
void	print_env(t_env *head);
int		check_if_sorted(t_env *current);
void	switch_nodes(t_env *current);
void	sort_env(t_bigshell *data);
void	ft_export(t_bigshell *data);

//UNSET FUNCTIONS:

void	ft_unset(t_bigshell *data, t_command *cmd);
void	unset_var(t_bigshell *data, t_env **current, t_env **prev);
void	find_node_to_delete(t_bigshell *data, t_env **current, \
		t_env **prev, t_token *arg);

//ENV FUNCTIONS:

void	ft_env(t_bigshell *data);
//void	add_env_variable(t_bigshell *data);

//EXIT FUNCTIONS:

int		get_exitcode(t_bigshell *data);
void	update_exit_stat(t_bigshell *data, int exit_code);
void	ft_exit(t_bigshell *data, t_command *cmd);
t_env	*create_node(t_bigshell *data, char *str);
void	store_env(t_bigshell *data, char **env);
void	convert_env(t_bigshell *data);
void	free_single_node(t_bigshell *data, t_env **node);
void	free_env(t_bigshell *data);
void	free_builtin_list(t_bigshell *data);
void	free_tokens(t_token *data);
void	s_array_free(char **s_array);
void	double_free_array(char **array1, char **array2);
void	close_unused_fds(t_bigshell *data);
void	free_tmpfile(t_bigshell *data);
void	free_struct(t_bigshell *data);

//HEREDOC FUNCTIONS:

int		tmpfile_cleanup(t_bigshell *data);
char	*create_unique_name(t_bigshell *data, t_command *cmd, char *eof);
int		open_heredoc_fd(t_bigshell *data, t_command *cmd);
void	close_heredoc_fd(int fd);
char	*free_set_null(char *mod_eof);
void	ft_heredoc(t_bigshell *data);
char	*delimiter_finder(t_bigshell *data);
char	*check_for_quotes(t_bigshell *data, char *eof);
int		tmpfile_cleanup(t_bigshell *data);

//MAIN EXTRAS
int		exitcode_and_freeshell(t_bigshell *data);
int		remove_cmd_list_from_data(t_bigshell *data);
void	exec_init(t_bigshell *data);
void	fd_init(t_bigshell *data);

int		just_do_the_thing(t_bigshell *data);
int		execute(t_bigshell *data);
void	init_all(t_bigshell *data, char **env);
int		get_input(t_bigshell *data, char *lineread);

#endif
