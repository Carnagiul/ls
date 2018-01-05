#include "../libft.h"

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

char				*ft_get_file_user(struct stat stat)
{
	char			*ret;
	t_pswd			*pswd;

	pswd = getpwuid(stat.st_uid);
	if (pswd)
		ret = ft_strdup(pswd->pw_name);
	else
		ret = ft_itoa(stat.st_uid);
	return (ret);
}

char				*ft_get_file_group(struct stat stat)
{
	char			*ret;
	t_group			*grp;

	grp = getgrgid(stat.st_gid);
	if (grp)
		ret = ft_strdup(grp->gr_name);
	else
		ret = ft_itoa(stat.st_gid);
	return (ret);
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

void				ft_display_timefile(time_t timestamp)
{
	time_t			t;
	char			*dt;

	dt = ctime(&timestamp);
	t = time(NULL);
	if (t - timestamp <= 15811200)
		ft_printf("%2.2s %3.3s %5.5s ", &(dt[8]), &(dt[4]), &(dt[11]));
	else
		ft_printf("%2.2s %3.3s %5.4s ", &(dt[8]), &(dt[4]), &(dt[20]));
}