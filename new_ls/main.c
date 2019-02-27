#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

typedef struct						s_obj_param
{
	struct s_obj_param_ls			*ls;
	struct s_obj_param_ls_file		*path;
}									t_obj_param;

typedef struct						s_obj_param_ls_file
{
	char							*param;
	struct stat						stat;
	int								del;
	struct s_obj_param_ls_file		*next;
	struct s_obj_param_ls_file		*last;
}									t_obj_param_ls_file;

typedef struct						s_obj_param_ls
{
	int								a;
	int								rr;
	int								r;
	int								l;
	int								t;
}									t_obj_param_ls;

int ft_file_exist_int (char *filename)
{
  struct stat buffer;   
  return (stat (filename, &buffer) == 0);
}

t_obj_param_ls_file *ft_file_exist_obj(char *filename, t_obj_param_ls_file *old)
{
		struct stat 		buffer;
		t_obj_param_ls_file	*file;
		int					result;

		result = stat(filename, &buffer);
		if (result == 0)
		{

			file = (t_obj_param_ls_file *)malloc(sizeof(t_obj_param_ls_file));
			file->param = strdup(filename);
			file->stat = buffer;
			file->last = NULL;
			file->next = NULL;
			if (old != NULL)
			{
				if (old->last != NULL)
					old->last->next = file;
				if (old->next == NULL)
					old->next = file;
				old->last = file;				
			}
			else
			{
				old = file;
			}
			return (old);
		}
		else
			return (old);
}

t_obj_param		*ft_create_params(void)
{
	t_obj_param	*params;

	params = (t_obj_param *)malloc(sizeof(t_obj_param));
	params->ls = (t_obj_param_ls *)malloc(sizeof(t_obj_param_ls));
	params->path = NULL;
	params->ls->a = 0;
	params->ls->rr = 0;
	params->ls->r = 0;
	params->ls->l = 0;
	params->ls->t = 0;
	return (params);
}

void			ft_fnt_ls_display_param_file(t_obj_param_ls_file *file)
{
	if ((file->stat.st_mode & S_IFMT) == S_IFDIR)
		printf("\t Folder to check (%s)\n", file->param);
	else if ((file->stat.st_mode & S_IFMT) == S_IFBLK)
		printf("\t Block Device to check (%s)\n", file->param);
	else if ((file->stat.st_mode & S_IFMT) == S_IFCHR)
		printf("\t Caractere Device to check (%s)\n", file->param);
	else if ((file->stat.st_mode & S_IFMT) == S_IFIFO)
		printf("\t FIFO/Tube to check (%s)\n", file->param);
	else if ((file->stat.st_mode & S_IFMT) == S_IFLNK)
		printf("\t Symbolic Link to check (%s)\n", file->param);
	else if ((file->stat.st_mode & S_IFMT) == S_IFREG)
		printf("\t File to check (%s)\n", file->param);
	else if ((file->stat.st_mode & S_IFMT) == S_IFSOCK)
		printf("\t Socket to check (%s)\n", file->param);
	else
		printf("\t Unknow File to check (%s)\n", file->param);
}

void			ft_fnt_ls_display_param(t_obj_param *param)
{
	t_obj_param_ls_file *files;

	printf("parametre de ls : \n");
	printf("\t -a {%s}\n", (param->ls->a == 1) ? "Actif" : "Non Actif");
	printf("\t -l {%s}\n", (param->ls->l == 1) ? "Actif" : "Non Actif");
	printf("\t -r {%s}\n", (param->ls->r == 1) ? "Actif" : "Non Actif");
	printf("\t -t {%s}\n", (param->ls->t == 1) ? "Actif" : "Non Actif");
	printf("\t -R {%s}\n", (param->ls->rr == 1) ? "Actif" : "Non Actif");
	files = *(&(param->path));
	if (files != NULL)
	{
		while (files != NULL)
		{
			ft_fnt_ls_display_param_file(files);
			files = files->next;
		}
	}
}

int main(int argc, char **argv)
{
	t_obj_param *params;
	int			i;
	int			j;
	int			len;

	params = ft_create_params();
	i = 1;
	while (i < argc)
	{
		j = 0;
		len = strlen(argv[i]);
		while (j < len)
		{
			if (argv[i][j] == '-' && j == 0)
			{
				while (j < len)
				{
					if (argv[i][j] == 'a')
						params->ls->a = 1;
					else if (argv[i][j] == 't')
						params->ls->t = 1;
					else if (argv[i][j] == 'l')
						params->ls->l = 1;
					else if (argv[i][j] == 'r')
						params->ls->r = 1;
					else if (argv[i][j] == 'R')
						params->ls->rr = 1;
					else if (argv[i][j] == '-')
						;
					else
						printf("Le parametre %c dans l'argument %s n'es pas correct ou n'existe pas.\n", argv[i][j], argv[i]);
					j++;
				}
			}
			else
			{
				params->path = ft_file_exist_obj(argv[i], params->path);
				j = len;
				printf("l'argument %s n'es pas correct ou n'existe pas, on suppose qu'il s'agit d'un path ou d'un fichier.\n", argv[i]);
			}
			j = len + 1;
		}
		i++;
	}
	ft_fnt_ls_display_param(params);
}