#include "../libft.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

typedef struct  passwd  t_pswd;
typedef struct  group   t_group;

typedef struct			s_ls
{
	int					cmd[9];
	char				*dir;
	int					max;
	int					len_user;
	int					len_byte;
	int					len_group;
}						t_ls;

typedef struct			s_file_opt
{
	char				*name;
	char				*mod;
	int					owner;
	int					group;
	int					type;
	struct stat			stat;
	t_pswd				*pswd;
	t_group				*grp;
}						t_file_opt;

typedef struct			s_file_ls
{
	struct s_file_opt	*files;
	int					max;
}						t_file_ls;

void				ft_create_file_ls(char *path, t_ls *ls, int id);

int			ft_get_pow(int nb)
{
	int				pow;

	pow = 0;
	while (nb != 0)
	{
		pow++;
		nb /= 10;
	}
	return (pow);
}

char				*ft_display_file_chmod(struct stat stat)
{
	char			*ret;

	ret = ft_malloc(sizeof(char) * 10);
	ret[0] = (stat.st_mode & S_IRUSR) ? 'r' : '-';
	ret[1] = (stat.st_mode & S_IWUSR) ? 'w' : '-';
	ret[2] = (stat.st_mode & S_IXUSR) ? 'x' : '-';
	if (stat.st_mode & S_ISUID)
		ret[2] = (stat.st_mode & S_IXUSR) ? 's' : 'S';
	ret[3] = (stat.st_mode & S_IRGRP) ? 'r' : '-';
	ret[4] = (stat.st_mode & S_IWGRP) ? 'w' : '-';
	ret[5] = (stat.st_mode & S_IXGRP) ? 'x' : '-';
	if (stat.st_mode & S_ISGID)
		ret[5] = (stat.st_mode & S_IXGRP) ? 's' : 'S';
	ret[6] = (stat.st_mode & S_IROTH) ? 'r' : '-';
	ret[7] = (stat.st_mode & S_IWOTH) ? 'w' : '-';
	ret[8] = (stat.st_mode & S_IXOTH) ? 'x' : '-';
	if (stat.st_mode & S_ISVTX)
		ret[8] = (stat.st_mode & S_IXOTH) ? 't' : 'T';
	ret[9] = '\0';
	return (ret);
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

int				is_sorted_file(t_file_opt file, t_file_opt filee, t_ls *ls)
{
	if (ls->cmd[4])
	{
		if (file.stat.st_mtime == filee.stat.st_mtime)
		{
			if ((ls->cmd[3]) ? file.stat.st_mtimespec.tv_nsec > filee.stat.st_mtimespec.tv_nsec : file.stat.st_mtimespec.tv_nsec < filee.stat.st_mtimespec.tv_nsec)
				return (1);
		}
		if ((ls->cmd[3]) ? file.stat.st_mtime > filee.stat.st_mtime : file.stat.st_mtime < filee.stat.st_mtime)
			return (1);
	}
	else
	{
		if ((ls->cmd[3] == 0) ? ft_strcmp(file.name, filee.name) > 0 : ft_strcmp(file.name, filee.name) < 0)
			return (1);
	}
	return (0);
}

int				is_rotated_file(t_file_ls content, t_ls *ls)
{
	int			i;

	i = content.max - 2;
	if (content.max < 2)
		return (1);
	return (is_sorted_file(content.files[i], content.files[i + 1], ls));
}

int			get_file_id(t_file_ls content, t_ls *ls)
{
	int		i;

	i = -1;
	while (++i < content.max)
	{
		if (is_sorted_file(content.files[i], content.files[content.max - 1], ls) == 1)
			return (i);
	}
	return (0);
}

t_file_ls	rotate_file(t_file_ls content, t_ls *ls)
{
	int		j;
	int		i;
	t_file_opt	temp;
	int 	not_sorted;


	if (is_rotated_file(content,ls) == 0)
		return (content);
	not_sorted = get_file_id(content, ls);
	j = --not_sorted;
	while (++j < content.max)
	{
		i = not_sorted;
		while (++i < content.max)
		{
			if (i + 1 < content.max)
			{
				if (is_sorted_file(content.files[i], content.files[i + 1], ls) == 1)
				{
					temp = content.files[i];
					content.files[i] = content.files[i + 1];
					content.files[i + 1] = temp;
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

	content.max = -1;
	content.files = ft_malloc(sizeof(t_file_opt) * ft_files_count(path));
	dir = opendir(path);
	if (!dir)
		return (content);
	ls->len_group = 0;
	ls->len_byte = 0;
	ls->len_user = 0;
	content.max = 0;
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
			{
				lstat(files->d_name, &(content.files[content.max].stat));
				if (ls->cmd[1] == 1)
				{
					if (ft_get_pow(content.files[content.max].stat.st_size) > ls->len_byte)
						ls->len_byte = ft_get_pow(content.files[content.max].stat.st_size);
					content.files[content.max].pswd = getpwuid(content.files[content.max].stat.st_uid);
					content.files[content.max].grp = getgrgid(content.files[content.max].stat.st_gid);
					content.files[content.max].mod = ft_display_file_chmod(content.files[content.max].stat);
					if (content.files[content.max].pswd != NULL)
						content.files[content.max].owner = ft_strlen(content.files[content.max].pswd->pw_name);
					else
						content.files[content.max].owner = ft_get_pow(content.files[content.max].stat.st_uid) + 1;
					if (content.files[content.max].grp != NULL)
						content.files[content.max].group = ft_strlen(content.files[content.max].grp->gr_name);
					else
						content.files[content.max].group = ft_get_pow(content.files[content.max].stat.st_gid) + 1;
					if (content.files[content.max].owner > ls->len_user)
						ls->len_user = content.files[content.max].owner;
					if (content.files[content.max].group > ls->len_group)
						ls->len_group = content.files[content.max].group;
				}
			}	
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

void				ft_display_ls_file(t_ls *ls, t_file_opt content)
{
	if (ls->cmd[1] == 1)
	{
		ft_printf("%c", ft_display_file_type(content.stat));
		ft_printf("%s ", content.mod);
		ft_printf("%5ld ", content.stat.st_nlink);
		ft_printf("%-*s  ", ls->len_user, (content.pswd != NULL) ? content.pswd->pw_name : ft_itoa(content.stat.st_uid));
		ft_printf("%-*s ", ls->len_group, (content.grp != NULL) ? content.grp->gr_name : ft_itoa(content.stat.st_gid));
		ft_printf("%*lld ", ls->len_byte + 1,  content.stat.st_size);
		//if (content.pswd)
		//	free(content.pswd);
		//if (content.grp)
		//	free(content.grp);
		free(content.mod);
	}
	if (ls->cmd[7] == 1)
	{
		if (content.type == 4)
			ft_printf("@C%-*s@@", ls->max, content.name);
		else
			ft_printf("%-*s", ls->max, content.name);
	}
	else
		ft_printf("%-*s", ls->max, content.name);
	if (ls->cmd[7] == 1 || ls->cmd[1] == 1)
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
			else if (ls->cmd[1] == 0)
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
	if (content.max <= 0)
	{
		free(content.files);
		free(path);
		return ;
	}	
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
	ls->len_group = 0;
	ls->len_user = 0;
	ls->len_byte = 0;
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
