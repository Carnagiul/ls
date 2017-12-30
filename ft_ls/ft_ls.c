#include "../libft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

typedef struct			s_ls
{
	int					cmd[9];
	char				*dir;
	int					max;
}						t_ls;

typedef struct			s_file_opt
{
	char				*name;
	char				*mod;
	int					block;
	char				*owner;
	char				*group;
	int					type;
	struct stat			stat;
}						t_file_opt;

typedef struct			s_file_ls
{
	struct s_file_opt	*files;
	int					max;
}						t_file_ls;

void				ft_create_file_ls(char *path, t_ls *ls, int id);

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

int		ft_filename_len_opt(int count, t_file_opt *file)
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

int				is_rotated_file(t_file_ls content, t_ls *ls)
{
	int			i;

	i = -1;
	while (++i < content.max)
	{
		if (i + 1 < content.max)
		{
			if (ls->cmd[4])
			{
				if (ls->cmd[3])
				{
					if (content.files[i].stat.st_atime > content.files[i + 1].stat.st_atime)
						return (1);
				}
				else
				{
					if (content.files[i].stat.st_atime < content.files[i + 1].stat.st_atime)
						return (1);
				}
			}
			else
			{
				if (ls->cmd[3])
				{
					if (ft_strcmp(content.files[i].name, content.files[i + 1].name) > 0)
						return (1);
				}
				else
				{
					if (ft_strcmp(content.files[i].name, content.files[i + 1].name) < 0)
						return (1);
				}
			}
		}
	}
	return (0);
}

t_file_ls	rotate_file(t_file_ls content, t_ls *ls)
{
	int		j;
	int		i;
	t_file_opt	temp;

	j = -1;
	if (is_rotated_file(content,ls) == 0)
		return (content);
	while (++j < content.max)
	{
		i = -1;
		while (++i < content.max)
		{
			if (ls->cmd[4] == 1 && i + 1 < content.max)
			{
				if (ls->cmd[3] == 1)
				{
					if (content.files[i].stat.st_atime > content.files[i + 1].stat.st_atime)
					{
						temp = content.files[i];
						content.files[i] = content.files[i + 1];
						content.files[i + 1] = temp;
					}
					if (content.files[i].stat.st_atime == content.files[i + 1].stat.st_atime)
					{
						if (ft_strcmp(content.files[i].name, content.files[i + 1].name) > 0)
						{
							temp = content.files[i];
							content.files[i] = content.files[i + 1];
							content.files[i + 1] = temp;
						}
					}
				}
				else
				{
					if (content.files[i].stat.st_atime < content.files[i + 1].stat.st_atime)
					{
						temp = content.files[i];
						content.files[i] = content.files[i + 1];
						content.files[i + 1] = temp;
					}
					if (content.files[i].stat.st_atime == content.files[i + 1].stat.st_atime)
					{
						if (ft_strcmp(content.files[i].name, content.files[i + 1].name) < 0)
						{
							temp = content.files[i];
							content.files[i] = content.files[i + 1];
							content.files[i + 1] = temp;
						}
					}
				}
			}
			else
			{
				if (ls->cmd[3] == 1 && i + 1 < content.max)
				{
					if (ft_strcmp(content.files[i].name, content.files[i + 1].name) < 0)
					{
						temp = content.files[i];
						content.files[i] = content.files[i + 1];
						content.files[i + 1] = temp;
					}
				}
				else
				{
					if (i + 1 < content.max)
					{
						if (ft_strcmp(content.files[i].name, content.files[i + 1].name) > 0)
						{
							temp = content.files[i];
							content.files[i] = content.files[i + 1];
							content.files[i + 1] = temp;
						}
					}
				}
			}
		}
	}
	return (content);
}

char				*ft_joinpath(char *s1, char *s2)
{
	char	*str;
	int		i;
	int		j;

	str = ft_malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 2));
	for (i = 0; i < ft_strlen(s1); i++)
		str[i] = s1[i];
	if (str[i - 1] != '/')
		str[i++] = '/';
	for (j = 0; j < ft_strlen(s2); j++)
		str[i + j] = s2[j];
	str[i + j] = '\0';
	return (str);
}

t_file_ls			ft_get_files(char *path, t_ls *ls)
{
	t_file_ls		content;
	struct dirent	*files;
	DIR				*dir;
	int				ok;

	content.max = 0;
	content.files = ft_malloc(sizeof(t_file_opt) * ft_files_count(path));
	dir = opendir(path);
	if (ls->cmd[2] == 1)
		ft_printf("%s:\n", path);
	while ((files = readdir(dir)) != NULL)
	{
		ok = 1;
		if (files->d_name[0] == '.' && ls->cmd[0] == 0)
			ok = 0;
		if (ok == 1)
		{
			content.files[content.max].name = ft_strdup(files->d_name);
			content.files[content.max].type = files->d_type;
			if (ls->cmd[1] == 1 || ls->cmd[8] == 1 || ls->cmd[4] == 1)
				lstat(files->d_name, &(content.files[content.max].stat));
			content.max++;
			content = rotate_file(content, ls);
		}
	}
	closedir(dir);
	return (content);
}

char				ft_display_file_type(struct stat stat)
{
	if ((stat.st_mode & S_IFMT) == S_IFBLK)
		return ('b');
	if ((stat.st_mode & S_IFMT) == S_IFCHR)
		return ('c');
	if ((stat.st_mode & S_IFMT) == S_IFDIR)
		return ('d');
	if ((stat.st_mode & S_IFMT) == S_IFIFO)
		return ('p');
	if ((stat.st_mode & S_IFMT) == S_IFSOCK)
		return ('s');
	if ((stat.st_mode & S_IFMT) == S_IFLNK)
		return ('l');
	return ('-');
}

char				*ft_display_file_chmod(struct stat stat)
{
	char			*ret;

	ret = ft_malloc(sizeof(char) * 10);
	ret[0] = (stat.st_mode & S_IRUSR) ? 'r' : '-';
	ret[1] = (stat.st_mode & S_IWUSR) ? 'w' : '-';
	ret[2] = (stat.st_mode & S_IXUSR) ? 'x' : '-';
	ret[3] = (stat.st_mode & S_IRGRP) ? 'r' : '-';
	ret[4] = (stat.st_mode & S_IWGRP) ? 'w' : '-';
	ret[5] = (stat.st_mode & S_IXGRP) ? 'x' : '-';
	ret[6] = (stat.st_mode & S_IROTH) ? 'r' : '-';
	ret[7] = (stat.st_mode & S_IWOTH) ? 'w' : '-';
	ret[8] = (stat.st_mode & S_IXOTH) ? 'x' : '-';
	ret[9] = '\0';
	return (ret);
}

void				ft_display_ls_file(t_ls *ls, t_file_opt content)
{
	char			*mod;

	if (ls->cmd[1] == 1)
	{
		mod = ft_display_file_chmod(content.stat);
		ft_printf("%c%s%5ld %s %s ", ft_display_file_type(content.stat), (long)mod, content.stat.st_nlink, getpwuid(content.stat.st_uid)->pw_name, getgrgid(content.stat.st_gid)->gr_name);
		free(mod);
	}
	if (ls->cmd[7] == 1)
	{
		if (content.type == 4)
			ft_printf("@C%*s@@", ls->max, content.name);
		else
			ft_printf("%*s", ls->max, content.name);
	}
	else
		ft_printf("%*s", ls->max, content.name);
	if (ls->cmd[7] == 1)
	{
		ft_printf("\n");
	/*
    printf("Numéro d'inœud :                   %ld\n", (long) content.stat.st_ino);
    printf("Nombre de liens :                  %ld\n", (long) content.stat.st_nlink);
    printf("Propriétaires :                    UID=%ld   GID=%ld\n", (long) content.stat.st_uid, (long) content.stat.st_gid);

    printf("Taille de bloc d’E/S :             %ld octets\n", (long) content.stat.st_blksize);
    printf("Taille du fichier :                %lld octets\n", (long long) content.stat.st_size);
    printf("Blocs alloués :                    %lld\n", (long long) content.stat.st_blocks);
    printf("Dernier changement d’état :        %s", ctime(&content.stat.st_ctime));
    printf("Dernier accès au fichier :         %s", ctime(&content.stat.st_atime));
    printf("Dernière modification du fichier:  %s", ctime(&content.stat.st_mtime));
	*/

		//ft_printf("\n");
	}

}

void				ft_ls_display(t_ls *ls, int id, t_file_ls content, char *path)
{
	int				ok;
	int				test;

	ok = -1;
	while (++ok < content.max)
	{
		if (ls->cmd[8] == 1)
			ft_printf("%ld ", content.files[ok].stat.st_ino);
		if (ls->cmd[6] == 1)
		{
			ft_display_ls_file(ls, content.files[ok]);
			ft_printf("\n");
		}
		else
		{
			ft_display_ls_file(ls, content.files[ok]);
			if (ok + 1 >= content.max)
				ft_printf("\n");
			else
				ft_printf(" ");
		}
	}
	if (ls->cmd[2] == 1)
	{
		ok = -1;
		while (++ok < content.max)
		{
			if (ft_strcmp(content.files[ok].name, ".") != 0 && ft_strcmp(content.files[ok].name, "..") != 0 && (content.files[ok].type == 4))
				ft_create_file_ls(ft_joinpath(path, content.files[ok].name), ls, 1);
		}
	}
}
/*
int					ft_count_stack(t_file_ls content)
{
	int				i;
	int				c;

	i = 0;
	c = 0;
	while (i < content.max)
	{
		c += content.files[i].stat.st_blocks;
		i++;
	}
	return (c);
}

void				ft_create_file_ls(char *path, t_ls *ls, int id)
{
	t_file_ls		content;
	struct dirent	*files;
	DIR				*dir;
	int				ok;

	if (id == 1 && ls->cmd[2] == 1)
		ft_printf("\n", path);
	content = ft_get_files(path, ls);
	if (content.max == 0)
		return ;
	if (ls->cmd[1] == 1)
		ft_printf("total %d\n", ft_count_stack(content));
	ls->max = ft_filename_len_opt(content.max, content.files);
	ft_ls_display(ls, id, content, path);
	ok = -1;
	while (++ok < content.max)
		free(content.files[ok].name);
	free(content.files);
	free(path);
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
*/
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
		if (cmd[i] == '1')
			ls->cmd[6] = 1;
		if (cmd[i] == 'p')
			ls->cmd[7] = 1;
		if (cmd[i] == 'i')
			ls->cmd[8] = 1;
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
	ls->cmd[6] = 0;
	ls->cmd[7] = 0;
	ls->cmd[8] = 0;
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
	ft_create_file_ls(ft_strdup(ls->dir), ls, 0);
	free(ls->dir);
	free(ls);
}
