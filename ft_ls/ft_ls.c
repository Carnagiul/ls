#include "../libft.h"
#include <sys/types.h>
#include <sys/stat.h>

typedef struct			s_ls
{
	int					cmd[6];
	char				*dir;
}						t_ls;

typedef struct			s_file_opt
{
	char				*name;
	int					type;
	struct stat			stat;
}						t_file_opt;

typedef struct			s_file_ls
{
	struct s_file_opt	*files;
	int					max;
}						t_file_ls;

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

t_file_ls	ft_create_file_ls(char *path, t_ls *ls)
{
	t_file_ls	content;

	content.files = ft_malloc(sizeof(t_file_opt) * ft_files_count(path));
	content.max = 0;
	return (content);
	
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
		temp = file[i];
		file[i] = file[count];
		file[count] = temp;
		i++;
	}
	return (file);
}

int		ls_is_last(t_ls *ls, int nb, t_file *file, int id)
{
	if (id < nb && ls->cmd[0] == 1)
		return (0);
	while (id < nb)
	{
		if (file[id].name[0] == '.')
			id++;
		else
			return (0);
	}
	return (1);
}

t_file	*trier_tableau(t_file *file, int count)
{
	int	i;
	t_file temp;
	int	ii;

	ii = 0;
	while (ii < count)
	{
		i = 0;
		while (i + 1 < count)
		{
			if (ft_strcmp(file[i].name, file[i + 1].name) > 0)
			{
				temp = file[i];
				file[i] = file[i + 1];
				file[i + 1] = temp;
			}
			i++;
		}
		ii++;
	}
	return (file);
}

int		ft_filename_len(int count, t_file *file)
{
	int i;
	int	len;

	len = 0;
	i = -1;
	while (++i < count)
		len = (len < ft_strlen(file[i].name)) ? ft_strlen(file[i].name) : len;
	while (len % 4 != 0)
		len++;
	return (len);
}

t_file		*do_ls_first(t_ls *ls)
{
	t_file	*file;
	int		nb;

	nb = ft_files_count(ls->dir);
	if (nb == 0)
		return (NULL);
	file = ft_create_array_files(ls->dir);
	if (nb && file)
		file = trier_tableau(file, nb);
	if (ls->cmd[2] == 1)
		ft_printf("%s:\n", ls->dir);
	if (ls->cmd[3] == 1)
		file = do_reverse_file(file, nb);
	return (file);
}

void	do_ls(t_ls *ls)
{
	int		nb;
	char	*temp_dir;
	int		i;
	t_file	*file;
	int		maxlen;

	temp_dir = ft_strdup(ls->dir);
	nb = ft_files_count(temp_dir);
	file = do_ls_first(ls);
	i = 0;
	if (nb == 0 || file == NULL)
		return ;
	maxlen = ft_filename_len(nb, file);
	while (i < nb)
	{
		if (ls->cmd[0] == 0 && file[i].name[0] == '.')
		{

		}
		else
		{
			if (ls_is_last(ls, nb, file, i + 1) == 0)
			{
				ft_printf("%s", file[i].name);
				if (ls->cmd[5] == 1)
				{
					for (int r = ft_strlen(file[i].name); r < maxlen; r++)
						ft_printf(" ");
				}
				else
					ft_printf("\t");
			}
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
				if (ls->dir[ft_strlen(ls->dir) - 1] != '/')
						ls->dir = ft_free_join1(ls->dir, "/");
				if (ls->cmd[0] == 0 && file[i].name[0] != '.')
				{
					ls->dir = ft_free_join1(ls->dir, file[i].name);
					ft_printf("\n");
					do_ls(ls);
					free(ls->dir);
					ls->dir = ft_strdup(temp_dir);
				}
				if (ls->cmd[0] == 1)
				{
					ls->dir = ft_free_join1(ls->dir, file[i].name);
					ft_printf("\n");
					do_ls(ls);
					free(ls->dir);
					ls->dir = ft_strdup(temp_dir);
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
		if (cmd[i] == 'C' || cmd[i] == 'l')
			ls->cmd[5] = 1;
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
	ls->cmd[5] = 0;
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
		if (ft_strcmp(temp, "--version") == 0)
			exit(ft_printf("Version : @G%s@@\n", VERSION));
		if (ft_strcmp(temp, "--usage") == 0)
			exit(ft_printf("Usage : \n@R%s@@ %s\n", argv[0], USAGE));
		if (temp[0] == '-')
			addCmd(temp, ls);
		else if (ls->dir == NULL)
			ls->dir = ft_strdup(argv[i]);
		free(temp);
	}
	if (ls->dir == NULL)
		ls->dir = ft_strdup(".");
	do_ls(ls);
}
