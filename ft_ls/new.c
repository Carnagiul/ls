#include "../libft.h"

t_ls_ppt		*create_ppt(t_dir *file, t_ls *ls, char *path, t_ls_app *app)
{
	t_ls_ppt	*ret;
	char		*filepath;
	int			len;

	len = 0;
	if (ls->cmd[0] == 0 && file->d_name[0] == '.')
		return (NULL);
	ret = ft_malloc(sizeof(*ret));
	if (ls->cmd[1] || ls->cmd[4])
	{
		filepath = ft_joinpath(path, file->d_name);
		lstat(filepath, &(ret->stat));
		free(filepath);
	}
	if ((len = ft_strlen(file->d_name)) > app->max_name)
		app->max_name = len;
	ret->name = ft_strdup(file->d_name);
	ret->type = file->d_type;
	if (ls->cmd[1] == 1)
	{
		ret->mod = ft_display_file_chmod(ret->stat);
		ret->pwd = ft_get_file_user(ret->stat);
		ret->grp = ft_get_file_group(ret->stat);
		if (ft_strlen(ret->grp) > app->max_grp)
			app->max_grp = ft_strlen(ret->grp);
		if (ft_strlen(ret->pwd) > app->max_pwd)
			app->max_pwd = ft_strlen(ret->pwd);		
	}
	ret->next = NULL;
	app->count++;
	return (ret);
}

int			can_break(t_ls_ppt *temp, t_ls_ppt *actual, t_ls *ls)
{
	if (ls->cmd[4])
	{
		if (actual->stat.st_mtime == temp->stat.st_mtime)
		{
			if ((ls->cmd[3]) ? actual->stat.st_mtimespec.tv_nsec >
				temp->stat.st_mtimespec.tv_nsec :
				actual->stat.st_mtimespec.tv_nsec <
				temp->stat.st_mtimespec.tv_nsec)
				return (1);
		}
		if ((ls->cmd[3]) ? actual->stat.st_mtime > temp->stat.st_mtime
			: actual->stat.st_mtime < temp->stat.st_mtime)
			return (1);
	}
	else
	{
		if ((ls->cmd[3] == 0) ? ft_strcmp(actual->name, temp->name) > 0 :
			ft_strcmp(actual->name, temp->name) < 0)
			return (1);
	}
	return (0);
}

void		ft_ppt_trier(t_ls_app *app, t_ls_ppt *temp, t_ls *ls)
{
	t_ls_ppt	*mem_ppt;
	t_ls_ppt	*old;

	mem_ppt = *(&(app->files));
	if (can_break(temp, mem_ppt, ls) == 1)
	{
		temp->next = app->files;
		app->files = temp;
		return ;
	}
	old = mem_ppt;
	while (mem_ppt != NULL)
	{
		if (can_break(temp, mem_ppt, ls) == 1)
			break ;
		old = mem_ppt;
		mem_ppt = mem_ppt->next;
	}
	if (mem_ppt != NULL)
	{
		old->next = temp;
		temp->next = mem_ppt;
	}
	else
		old->next = temp;
}

void		ft_ppt_push_front(char *path, t_dir *file, t_ls *ls, t_ls_app *app)
{
	t_ls_ppt	*temp;

	temp = create_ppt(file, ls, path, app);
	if (temp)
	{
		if (app->files == NULL)
		{
			temp->next = app->files;
			app->files = temp;
			return ;
		}
		ft_ppt_trier(app, temp, ls);
	}
}

void			ft_display(t_ls_app *data, t_ls *ls)
{
	t_ls_ppt		*list;

	list = *(&(data->files));
	while (list)
	{
		if (ls->cmd[8] == 1)
			ft_printf("%ld ", list->stat.st_ino);
		if (ls->cmd[1])
			ft_printf("%c%s %-*s %-*s ", ft_display_file_type(list->stat), list->mod, data->max_pwd, list->pwd, data->max_grp, list->grp);
		if (ls->cmd[1] || ls->cmd[6])
			ft_printf("%s\n", list->name);
		else if (ls->cmd[5])
			ft_printf("%-*s ", data->max_name, list->name);
		else
			ft_printf("%s\t", list->name);
		list = list->next;
	}
}


void			ft_readdir(char *path, t_ls *ls, t_ls_app *ret)
{
	t_ls_app		*old_ret;
	t_ls_ppt		*mem;
	t_ls_ppt		*list;
	t_ls_ppt		*old;
	t_dir			*files;
	DIR				*dir;

	dir = opendir(path);
	if (!dir)
		return ;
	ret->files = NULL;
	ret->count = 0;
	ret->max_name = 0;
	ret->max_grp = 0;
	ret->max_pwd = 0;
	ret->next = NULL;
	while ((files = readdir(dir)) != NULL)
		ft_ppt_push_front(path, files, ls, ret);
	free(files);
	closedir(dir);
	list = *(&(ret->files));
	mem = NULL;
	if (ls->cmd[2] == 1)
		printf("%s:\n", path);
	while (list)
	{
		ft_display(list, ls);
		list = list->next;
	}
	list = *(&(ret->files));
	while (list)
	{
		if (list->type == 4 && ls->cmd[2] == 1 && ft_strcmp(list->name, ".") != 0 && ft_strcmp(list->name, "..") != 0)
		{
			old_ret = *(&ret);
			if (old_ret)
			{
				while (old_ret->next != NULL)
					old_ret = old_ret->next;
				old_ret->next = ft_malloc(sizeof(*ret));
				ft_readdir(ft_joinpath(path, list->name), ls, old_ret->next);
			}
			else
			{
				ret->next = ft_malloc(sizeof(*ret));
				ft_readdir(ft_joinpath(path, list->name), ls, ret->next);
			}
		}
		list = list->next;
	}
	free(path);
}