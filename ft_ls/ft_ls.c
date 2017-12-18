#include "../libft.h"
#include <sys/types.h> 
#include <sys/stat.h> 

typedef struct	s_ls
{
	int			cmd[5];
	char		*dir;
}				t_ls;

t_file		*ft_create_array_files_wa(char *path)
{
	int				nb;
	t_file			*files;
	struct dirent	*f;
	DIR				*dir;
	int				i;

	nb = ft_files_count_files_wa(path);
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

void	do_lstat(t_file *file, char *path, int count, t_ls *ls)
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
}

void	do_reverse_file(t_file *file, int count)
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
}

void	do_ls(t_ls *ls)
{
	int		nb;
	char	*temp_dir;
	int		i;
	t_file	*file;

	temp_dir = ls->dir;
	nb = ft_files_count_files_wa(temp_dir);
	file = ft_create_array_files_wa(temp_dir);
	if (ls->cmd[2])
		ft_printf("%s:\n", temp_dir);
	if (ls->cmd[3])
		do_reverse_file(file, nb);
	while (i < nb)
	{
		if (ls->cmd[0] == 0 && file[i].name[0] == '.')
			continue ;
		if (i != nb - 1)
			ft_printf("%s\t", file[i].name);
		else
			ft_printf("%s\n", file[i].name);
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
	char	*dir;

	ls = (t_ls *)ft_malloc(sizeof(t_ls));
	if (argc > 1)
		dir = argv[argc - 1];
	else
		dir = ft_strdup(".");
	i = 0;
	if (argc == 1)
		do_ls(ls);
	while (++i < argc - 1)
	{
		temp = argv[i];
		if (temp[0] == '-')
			addCmd(temp, ls);
		free(temp);
	}
	do_ls(ls);
}