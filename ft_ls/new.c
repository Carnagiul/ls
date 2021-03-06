#include "../libft.h"
#include <errno.h>

t_ls_ppt		*create_ppt(t_dir *file, t_ls *ls, char *path, t_ls_app *app)
{
	t_ls_ppt	*ret;
	int			len;

	len = 0;
	if (ls->cmd[0] == 0 && file->d_name[0] == '.')
		return (NULL);
	ret = ft_malloc(sizeof(*ret));
	if (ls->cmd[1] || ls->cmd[4])
	{
		ret->filepath = ft_joinpath(path, file->d_name);
		lstat(ret->filepath, &(ret->stat));
	}
	if ((len = ft_strlen(file->d_name)) > app->max_name)
		app->max_name = len;
	ret->name = ft_strdup(file->d_name);
	ret->type = file->d_type;
	if (ls->cmd[1] == 1)
	{
		if (ft_pow(ret->stat.st_size) > app->pow_size)
			app->pow_size = ft_pow(ret->stat.st_size);
		ret->mod = ft_display_file_chmod(ret->stat);
		ret->pwd = ft_get_file_user(ret->stat);
		ret->grp = ft_get_file_group(ret->stat);
		if (ft_strlen(ret->grp) > app->max_grp)
			app->max_grp = ft_strlen(ret->grp);
		if (ft_strlen(ret->pwd) > app->max_pwd)
			app->max_pwd = ft_strlen(ret->pwd);
		app->max_size += ret->stat.st_blocks;
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

void				kkkft_display_timefile(time_t timestamp)
{
	time_t			t;
	char			*dt;

	dt = ctime(&timestamp);
	t = time(NULL);
	if (t - timestamp <= 15811200)
		printf("%2.2s %3.3s %5.5s ", &(dt[8]), &(dt[4]), &(dt[11]));
	else
		printf("%2.2s %3.3s %5.4s ", &(dt[8]), &(dt[4]), &(dt[20]));
}

void			ft_display(t_ls_app *data, t_ls *ls)
{
	t_ls_ppt		*list;
	char			buf[256];
	int				len;

	list = *(&(data->files));
	while (list)
	{
		if (ls->cmd[8] == 1)
			printf("%llu ", list->stat.st_ino);
		if (ls->cmd[1])
		{
			printf("%c", ft_display_file_type(list->stat));
			printf("%s ", list->mod);
			printf("USER=%-*s ", data->max_pwd, list->pwd);
			printf("GROP=%-*s ", data->max_grp, list->grp);
			printf("SIZE=%*lld ", data->pow_size, list->stat.st_size);
			kkkft_display_timefile(list->stat.st_mtime);
		}
		if (ls->cmd[1] || ls->cmd[6])
		{
			if (ft_display_file_type(list->stat) == 'l')
			{
				len = (int)readlink(list->filepath, buf, sizeof(buf));
				buf[len] = '\0';
				printf("%s -> %s\n", list->name, buf);
			}
			else
				printf("%s\n", list->name);
		}
		else if (ls->cmd[5])
			printf("%-*s ", data->max_name, list->name);
		else
			printf("%s\t", list->name);
		list = list->next;
	}
}

t_ls_app 			*ft_readdir(char *path, t_ls *ls)
{
	t_ls_app		*old_ret;
	t_ls_ppt		*mem;
	t_ls_ppt		*list;
	t_ls_ppt		*old;
	t_dir			*files;
	DIR				*dir;
	t_ls_app 		*ret;

	if (ls->cmd[2] == 1)
		printf("%s:\n", path);
	dir = opendir(path);
	if (dir == NULL)
	{
		free(path);
		printf("Cannot open the directory\n");
		return (NULL);
	}
	ret = ft_malloc(sizeof(*ret));
	ret->files = NULL;
	ret->count = 0;
	ret->max_name = 0;
	ret->max_grp = 0;
	ret->max_pwd = 0;
	ret->max_size = 0;
	ret->pow_size = 0;
	ret->next = NULL;
	while ((files = readdir(dir)) != NULL)
		ft_ppt_push_front(path, files, ls, ret);
	free(files);
	closedir(dir);
	ft_display(ret, ls);
	printf("\n");
	list = *(&(ret->files));
	while (list && ls->cmd[2] == 1)
	{
		if (list->type == 4 &&
					ft_strcmp(list->name, ".") != 0 &&
					ft_strcmp(list->name, "..") != 0)
		{
			old_ret = *(&ret);
			if (old_ret)
			{
				while (old_ret->next != NULL)
					old_ret = old_ret->next;
				printf("\n");
				old_ret->next = ft_readdir(ft_joinpath(path, list->name), ls);
			}
		}
		list = list->next;
	}
	free(path);
	return (ret);
}
