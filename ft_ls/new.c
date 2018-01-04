#include "../libft.h"

typedef struct		s_ls_ppt
{
	char			*name;
	int				type;
	struct  stat	stat;
	struct s_ls_ppt	*next;
}					t_ls_ppt;

typedef struct		s_ls_app
{
	struct s_ls_ppt	**files;
	int				count;
}					t_ls_app;

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

	list = app->files;
	if (*list)
	{
		temp = create_ppt(file, ls, path, app);
		temp->next = *list;
		*list = temp;
	}
	else
		*list = create_ppt(file, ls, path, app);
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
	while ((files = readdir(dir)) != NULL)
		ft_ppt_push_front(path, files, ls, ret);
	free(files);
	closedir(dir);
	return (ret);
}