#include "../libft.h"

t_ls_ppt		*create_ppt(t_dir *file, t_ls *ls, char *path, t_ls_app *app)
{
	t_ls_ppt	*ret;
	char		*filepath;


	if (ls->cmd[0] == 0 && file->d_name[0] == '.')
		return (NULL);
	ret = ft_malloc(sizeof(*ret));
	filepath = ft_joinpath(path, file->d_name);
	lstat(filepath, &(ret->stat));
	free(filepath);
	ret->name = ft_strdup(file->d_name);
	ret->type = file->d_type;
	ret->next = NULL;
	app->count++;
	return (ret);
}

void		ft_ppt_push_front(char *path, t_dir *file, t_ls *ls, t_ls_app *app)
{
	t_list	*temp;
	t_list	**list;

	if (app->files != NULL)
	{
		temp = create_ppt(file, ls, path, app);
		temp->next = app->files;
		app->files = temp;
	}
	else
		app->files = create_ppt(file, ls, path, app);
	ft_printf("test free\n");
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
	ft_printf("ici?\n");
	while ((files = readdir(dir)) != NULL)
		ft_ppt_push_front(path, files, ls, ret);
	ft_printf("test\n");
	free(files);
	closedir(dir);
	return (ret);
}