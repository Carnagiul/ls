#include "../libft.h"
#include <sys/types.h> 
#include <sys/stat.h> 

typedef struct	s_ls
{
	int			cmd[5];
	char		*dir;
}				t_ls;


int		ft_files_count(char *path)
{
	struct dirent	*files;
	int				count;
	DIR				*dir;

	count = 0;
	dir = opendir(path);
	if (!dir)
		return (0);
	while ((files = readdir(dir)) != NULL)
	{
		count++;
	}
	free(files);
	closedir(dir);
	return (count);
}

t_file		*ft_create_array_files(char *path)
{
	int				nb;
	t_file			*files;
	struct dirent	*f;
	DIR				*dir;
	int				i;

	nb = ft_files_count(path);
	files = (t_file *)ft_malloc(sizeof(t_file) * nb);
	i = 0;
	dir = opendir(path);
	while ((f = readdir(dir)) != NULL)
	{
		files[i].name = ft_strdup(f->d_name);
		files[i++].type = f->d_type;
	}
	closedir(dir);
	return (files);
}

void	do_lstat_2(t_file *file, int id)
{
	t_file		tmp;

	tmp = file[id];
	file[id] = file[id + 1];
	file[id + 1] = tmp;
}

t_file		*do_lstat(t_file *file, char *path, int count, t_ls *ls)
{
	int	bl;
	int blbl;
 	struct stat st[count];
 	struct stat tmp;
 	char	*data;

	bl = -1;
	while (++bl < count)
	{
		data = ft_strjoin(path, "/");
		data = ft_free_join1(data, file[bl].name);
		lstat(data, &(st[bl]));
		free(data);
	}
	bl = -1;
	while (++bl < count)
	{
		blbl = -1;
		while (++blbl < count - 1)
		{
			if (st[blbl].st_atime < st[blbl + 1].st_atime)
			{
				do_lstat_2(file, blbl);
				tmp = st[blbl];
				st[blbl] = st[blbl + 1];
				st[blbl + 1] = tmp;
			}
		}
	}
	return (file);
}

t_file 	*do_reverse_file(t_file *file, int count)
{
	int	i;
	t_file temp;

	i = 0;
	while (i < count)
	{
		--count;
		temp = file[0];
		file[0] = file[count];
		file[count] = temp;
		i++;
	}
	return (file);
}

int		ls_is_last(t_ls *ls, int nb, t_file *file, int id)
{
	while (id < nb)
	{
		if (!(ls->cmd[0] == 0 && file[i].name[0] == '.'))
			return (0);
		id++;
	}
	return (1);
}

void	do_ls(t_ls *ls)
{
	int		nb;
	char	*temp_dir;
	int		i;
	t_file	*file;
	DIR				*dir;

	temp_dir = ls->dir;
	dir = opendir(temp_dir);
	if (!dir)
		return ;
	closedir(dir);
	nb = ft_files_count(temp_dir);
	file = ft_create_array_files(temp_dir);
	if (ls->cmd[2] == 1)
		ft_printf("%s:\n", temp_dir);
	if (ls->cmd[3] == 1)
		file = do_reverse_file(file, nb);
	i = 0;
	while (i < nb)
	{
		if (ls->cmd[0] == 0 && file[i].name[0] == '.')
		{

		}
		else
		{
			if (ls_is_last(ls, nb, file, i) == 0)
				ft_printf("%s\t", file[i].name);
			else
				ft_printf("%s\n", file[i].name);
		}
		i++;
	}
	if (ls->cmd[2] == 1)
	{
		i = 0;
		while (i < nb)
		{
			if (file[i].type == 4 &&
				ft_strcmp(file[i].name, ".") != 0 &&
				ft_strcmp(file[i].name, "..") != 0)
			{
				if (ls->cmd[0] == 0 && file[i].name[0] != '.')
				{
					ls->dir = ft_free_join1(ls->dir, "/");
					ls->dir = ft_free_join1(ls->dir, file[i].name);
					ft_printf("\n");
					do_ls(ls);
				}	
				if (ls->cmd[1] == 1) 
				{
					ls->dir = ft_free_join1(ls->dir, "/");
					ls->dir = ft_free_join1(ls->dir, file[i].name);
					ft_printf("\n");
					do_ls(ls);
				}
			}
			i++;
		}
	}
}

void	addCmd(char *cmd, t_ls *ls)
{
	int		i;

	i = 0;
	while (++i < ft_strlen(cmd))
	{
		if (cmd[i] == 'a')
			ls->cmd[0] = 1;
		if (cmd[i] == 'l')
			ls->cmd[1] = 1;
		if (cmd[i] == 'R')
			ls->cmd[2] = 1;
		if (cmd[i] == 'r')
			ls->cmd[3] = 1;
		if (cmd[i] == 't')
			ls->cmd[4] = 1;
	}
}

void ft_ls(int argc, char **argv)
{
	char	*temp;
	int		i;
	t_ls	*ls;

	i = 0;
	ls = (t_ls *)ft_malloc(sizeof(t_ls));
	ls->cmd[0] = 0;
	ls->cmd[4] = 0;
	ls->cmd[3] = 0;
	ls->cmd[2] = 0;
	ls->cmd[1] = 0;
	ls->dir = NULL;
	if (argc == 1)
		ls->dir = ft_strdup(".");
	while (++i < argc)
	{
		temp = ft_strdup(argv[i]);
		if (temp[0] == '-')
			addCmd(temp, ls);
		else if (ls->dir == NULL)
			ls->dir = ft_strdup(argv[i]);
		free(temp);
	}
	do_ls(ls);
}