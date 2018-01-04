#ifndef FT_LS_H
# define FT_LS_H

# define VERSION "1.0.0"
# define USAGE "[aRrtlC] [file]"

typedef struct  passwd  t_pswd;
typedef struct  group   t_group;
typedef struct  stat   	t_stat;
typedef struct	dirent	t_dir;
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

void					ft_ls(int argc, char **argv);
void					ft_create_file_ls(char *path, t_ls *ls, int id);
char					*ft_display_file_chmod(struct stat stat);
char					ft_display_file_type(struct stat stat);
void					ft_display_timefile(time_t timestamp);

t_ls_app				*ft_readdir(char *path, t_ls *ls);


#endif
