/*
** Made by Florian Lécot
** Email : <contact.florianlecot@gmail.com>
** 
** Started on  Tue Nov  8 16:03:38 2016 
** Last update Sun Nov 13 17:40:51 2016 
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>	// include open
#include <sys/stat.h>	// include open (à supprimer si on ne fait pas d'appel à open dans le fichier)
#include <fcntl.h>	// include open

# define SIZE_BUFFER (128)

static char		*ret_line(char *line, char *buffer, ssize_t *compt, ssize_t size_line) // Alloue/Réalloue de la mémoire à line afin d'y copier le contenu du READ stocké dans buffer
{
  register const char	*temp;
  ssize_t		len_line;
  ssize_t		i;

  errno = 0;
  if (line == NULL) // Première allocation de mémoire de line, correspond aux "SIZE_BUFFER" premiers caractères lus depuis l'appel de GNL
    {
      if ((line = malloc((size_t)size_line + 1)) == NULL)
	{
	  fprintf(stderr, "Error : Couldn't allocate memory (%s).\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}
      len_line = 0;
    }
  else // Réalloue de la mémoire à line afin de copier le contenu de buffer à la fin de line
    {
      for (temp = line; *temp; ++temp); // Strlen non typé
      len_line = temp - line;           //
      if ((line = realloc(line, (size_t)len_line + (size_t)size_line + 1)) == NULL)
	{
	  fprintf(stderr, "Error : Couldn't reallocate memory (%s).\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}
    }
  for (i = 0; i < size_line; i++)              //
    line[len_line + i] = buffer[(*compt) + i]; // Concatène notre buffer (la dernière lecture) derrière line (ce qui a été lu depuis le début) ou copie au début si c'est la première lecture
  line[len_line + i] = '\0';                   //
  *compt += size_line + 1;
  return (line);
}

char			*get_next_line(const int fd)
{
  static char		buffer[SIZE_BUFFER + 1];
  static ssize_t	compt = 0;
  static ssize_t	size_read = 0;
  ssize_t		size_line;
  char			*line; // Variable qui stockera le retour du GNL

  for (size_line = 0, line = NULL, errno = 0 ;; size_line++) // Boucle infinie volontaire, s'il y a une erreur de lecture ou qu'il n'y a plus rien à lire elle sera interrompue avec un return.
    {
      if (compt >= size_read)
	{
	  compt = 0;
	  size_line = 0;
	  if ((size_read = read(fd, buffer, SIZE_BUFFER)) < 0)
	    fprintf(stderr, "Error : Couldn't read (%s).\n", strerror(errno));
	  if (size_read <= 0) // Si on est dans l'incapacité de lire ou qu'il n'y a plus rien à lire -> Renvoie line qui peut valoir NULL (ex : si READ à renvoyé -1)
	    return (line);
	}
      if (buffer[compt + size_line] == '\n') // Principale condition d'arrêt du GNL -> Renvoie tout ce qui a été lu depuis l'appel de la fonction lorsqu'on rencontre un "\n"
	return (ret_line(line, buffer, &compt, size_line));
      if (compt + size_line == (size_read - 1))
	line = ret_line(line, buffer, &compt, (size_line + 1));
    }
}

/*
** La fonction get_next_line est une fonction qui peut lire sur un fd défini en paramètre (Elle peut aussi bien lire l'entrée standard qu'un fichier préalablement ouvert avec les droits de lecture).
** Elle renvoie dans une chaîne de caractère ce qui a été lue lorsqu'elle rencontre un \n ou qu'il n'y a plus rien à lire.
** Un buffer est défini (SIZE_BUFFER), il désigne le nombre de caractères lus au maximum à chaque appel de READ.
** Pour un gain de temps, ne pas hésiter à augmenter la taille du buffer si on sait que les lignes lues seront bien plus grandes que celui-ci.
** (On peut aussi supprimer le define et ajouter en paramètre de get_next_line et ret_line un const size_t afin de choisir une taille adaptée à nos différents appel du GNL).
** get_next_line fait appel à read autant de fois que nécessaire (la lecture n'est pas limitée par la taille du buffer).
** On vérifie la valeur de retour du read afin d'interrompre la boucle (et on affiche un message d'erreur si nécessaire).
** ret_line utilise la valeur renvoyée par read pour allouer/réallouer de la mémoire afin de stocker la lecture dans line.
** Il copie ensuite le contenu de la dernière lecture effectuée à la fin de line.
** NB : Ne pas oublier de free les chaînes retournées par get_next_line lorsque vous n'en avez plus l'utilité.
*/

int		main(int ac, char **av)
{
  char		*str;
  int		fd;
  
  str = NULL;
  errno = 0;
  if (ac == 2)
    {
      if ((fd = open(av[1], O_RDONLY)) == -1)
	{
	  fprintf(stderr, "Error : Couldn't open \"%s\" (%s).\n", av[1], strerror(errno));
	  return (1);
	}
    }
  else
    fd = 0;
  for (;;)
    {
      if ((str = get_next_line(fd)) != NULL)
	{
	  printf("%s\n", str);
	  free(str);
	}
      else
	return (0);
    }
}

/*
** Exemple d'utilisation du get_next_line :
** Si on donne un fichier en paramètre, ouvre le paramètre en lecture seule et affiche son contenu ligne par ligne.
** Sinon, lire indéfiniment sur l'entrée standard et renvoyer ce qui a été lu.
*/
