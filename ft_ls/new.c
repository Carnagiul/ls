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
	filepath = ft_joinpath(path, file->d_name);
	lstat(filepath, &(ret->stat));
	free(filepath);
	if ((len = ft_strlen(file->d_name)) > app->max_name)
		app->max_name = len;
	ret->name = file->d_name;
	ret->type = file->d_type;
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

void			ft_readdir(char *path, t_ls *ls)
{
	t_ls_app		*ret;
	t_ls_ppt		**mem;
	t_ls_ppt		*list;
	t_ls_ppt		*old;
	t_dir			*files;
	DIR				*dir;

	dir = opendir(path);
	if (!dir)
		return ;
	ret = ft_malloc(sizeof(*ret));
	ret->files = NULL;
	ret->count = 0;
	ret->max_name = 0;
	while ((files = readdir(dir)) != NULL)
		ft_ppt_push_front(path, files, ls, ret);
	free(files);
	closedir(dir);
	mem = &(ret->files);
	list = *mem;
	if (list->type == 4 && ls->cmd[2] == 1)
		ft_printf("%s:\n", path);
	while (list)
	{
		ft_printf("%-*s", ret->max_name, list->name);
		list = list->next;
	}
	ft_printf("\n");
	list = *mem;
	old = NULL;
	while (list)
	{
		old = list;
		if (list->type == 4 && ls->cmd[2] == 1)
			ft_readdir(ft_joinpath(path, list->name), ls);
		list = list->next;
	}
}