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

void		ft_ppt_trier(t_ls_app *app, t_ls_ppt *temp, t_ls *ls)
{
	t_ls_ppt	**mem;
	t_ls_ppt	*mem_ppt;
	t_ls_ppt	*swap;
	t_ls_ppt	*old;

	mem = &(app->files);
	mem_ppt = *mem;
	old = NULL;
	if (ft_strcmp(mem_ppt->name, temp->name) > 0)
	{
		temp->next = app->files;
		app->files = temp;
		return ;
	}
	while (mem_ppt->next != NULL)
	{
		if (ft_strcmp(mem_ppt->name, temp->name) > 0)
			break ;
		old = mem_ppt;
		mem_ppt = mem_ppt->next;
	}
	if (old->next != NULL)
	{
		/*
			swap = mem_ppt->next;
			mem_ppt->next = temp;
			temp->next = swap;		
		*/
		ft_printf("%s replace by %s\n", old->name, temp->name);
		swap = old->next;
		old->next = temp;
		temp->next = swap;
	}
	else
		mem_ppt->next = temp;
	(void)ls;
	return ;

}

void		ft_ppt_push_front(char *path, t_dir *file, t_ls *ls, t_ls_app *app)
{
	t_ls_ppt	*temp;

	temp = create_ppt(file, ls, path, app);
	if (temp)
	{
		if (app->files != NULL)
		{
			ft_ppt_trier(app, temp, ls);
			//temp->next = app->files;
			//app->files = temp;
		}
		else
			app->files = temp;
	}
}

t_ls_app			*ft_readdir(char *path, t_ls *ls)
{
	t_ls_app		*ret;
	t_ls_ppt		*get;
	t_dir			*files;
	DIR				*dir;

	dir = opendir(path);
	if (!dir)
		return (NULL);
	ret = ft_malloc(sizeof(*ret));
	ret->files = NULL;
	ret->count = 0;
	ret->max_name = 0;
	while ((files = readdir(dir)) != NULL)
		ft_ppt_push_front(path, files, ls, ret);
	free(files);
	closedir(dir);
	return (ret);
}