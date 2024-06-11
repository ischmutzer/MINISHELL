/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ischmutz <ischmutz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 11:00:47 by ischmutz          #+#    #+#             */
/*   Updated: 2024/05/13 11:48:39 by ischmutz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void	unset_var(t_bigshell *data, t_env **current, t_env **prev)
{
	if (*current == data->env || *current == data->s_env)
	{
		if (*current == data->env)
			data->env = (*current)->next;
		else
			data->s_env = (*current)->next;
		free_single_node(data, current);
		return ;
	}
	if (!(*current)->next)
	{
		free_single_node(data, current);
		(*prev)->next = NULL;
		return ;
	}
	else
	{
		(*prev)->next = (*current)->next;
		free_single_node(data, current);
		return ;
	}
}

void	find_node_to_delete(t_bigshell *data, t_env **current,
	t_env **prev, t_token *arg)
{
	int		len;

	len = ft_strlen((*current)->var);
	while (*current)
	{
		if (ft_strncmp((*current)->var, arg->str, len) == 0)
		{
			unset_var(data, current, prev);
			return ;
		}
		else
		{
			*prev = *current;
			*current = (*current)->next;
		}
	}
	return ;
}

static void	check_banned_arg(t_bigshell *data, t_command *cmd)
{
	t_token	*arg;

	if (!cmd)
		return ;
	arg = cmd->args;
	while (arg)
	{
		if (!ft_strcmp(arg->str, "?") || !ft_strcmp(arg->str, "_"))
			update_exit_stat(data, 1);
		else
			update_exit_stat(data, 0);
		arg = arg->next;
	}
}

void	ft_unset(t_bigshell *data, t_command *cmd)
{
	t_env	*current;
	t_env	*prev;
	t_env	*current_env;
	t_env	*prev_env;
	t_token	*arg;

	if (!cmd->args)
		return (update_exit_stat(data, 0));
	current = data->s_env;
	prev = NULL;
	current_env = data->env;
	prev_env = NULL;
	arg = cmd->args;
	while (arg)
	{
		if (current && ft_strcmp(arg->str, "_") != 0)
			find_node_to_delete(data, &current, &prev, arg);
		if (current_env && (ft_strcmp(arg->str, "?") && \
			ft_strcmp(arg->str, "_")))
			find_node_to_delete(data, &current_env, &prev_env, arg);
		arg = arg->next;
		current = data->s_env;
		current_env = data->env;
	}
	return (check_banned_arg(data, cmd));
}
