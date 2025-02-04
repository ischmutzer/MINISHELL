/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ischmutz <ischmutz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 17:04:24 by ischmutz          #+#    #+#             */
/*   Updated: 2024/05/13 15:03:05 by ischmutz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../minishell.h"
#include "../../parser/parse.h"
#include <stdlib.h>

void	free_tokens(t_token *data)
{
	if (data->str)
		free(data->str);
	if (data->dir)
		free(data->dir);
}

void	free_paths(t_bigshell *data)
{
	if (data->exec->paths)
	{
		s_array_free(data->exec->paths);
		data->exec->paths = NULL;
	}
	if (data->exec->path)
	{
		free(data->exec->path);
		data->exec->path = NULL;
	}
	if (data->exec)
	{
		free(data->exec);
		data->exec = NULL;
	}
}

void	close_unused_fds(t_bigshell *data)
{
	if (data->std_in != -1)
	{
		if (close(data->std_in) == -1)
			perror("close std_in");
		data->std_in = -1;
	}
	if (data->std_out != -1)
	{
		if (close(data->std_out) == -1)
			perror("close std_out");
		data->std_out = -1;
	}
	if (data->fd_in != -1)
	{
		if (close(data->fd_in) == -1)
			perror("close fd_in");
		data->fd_in = -1;
	}
	if (data->fd_out != -1)
	{
		if (close(data->fd_out) == -1)
			perror("close fd_out");
		data->fd_out = -1;
	}
}

void	free_tmpfile(t_bigshell *data)
{
	t_command	*cmd;

	cmd = data->commands;
	while (cmd)
	{
		if (cmd->tmpfile)
			free(cmd->tmpfile);
		cmd = cmd->next;
	}
}

void	free_struct(t_bigshell *data)
{
	close_unused_fds(data);
	free_paths(data);
	free_tmpfile(data);
	if (data->commands)
	{
		delete_command_list(data->commands);
	}
	free_env(data);
	free_builtin_list(data);
	if (data->mod_env)
		s_array_free(data->mod_env);
	if (data->heredoc)
		delete_token_list(data->heredoc);
	if (data->pipe)
		free(data->pipe);
}
