# get_next_line

La fonction *get_next_line* est une fonction qui peut lire sur un fd défini en paramètre (Elle peut aussi bien lire l'entrée standard qu'un fichier préalablement ouvert avec les droits de lecture). Elle renvoie dans une chaîne de caractère ce qui a été lue lorsqu'elle rencontre un \n ou qu'il n'y a plus rien à lire.

Un buffer est défini (SIZE_BUFFER), il désigne le nombre de caractères lus au maximum à chaque appel de READ. Pour un gain de temps, ne pas hésiter à augmenter la taille du buffer si on sait que les lignes lues seront bien plus grandes que celui-ci (On peut aussi supprimer le define et ajouter en paramètre de *get_next_line* et *ret_line* un const size_t afin de choisir une taille adaptée à nos différents appel du *GNL*).

*get_next_line* fait appel à *read* autant de fois que nécessaire (**la lecture n'est pas limitée par la taille du buffer**). On vérifie la valeur de retour du *read* afin d'interrompre la boucle (et on affiche un message d'erreur si nécessaire).

*ret_line* utilise la valeur renvoyée par *read* pour allouer/réallouer de la mémoire afin de stocker la lecture dans line. Il copie ensuite le contenu de la dernière lecture effectuée à la fin de line.

NB : Ne pas oublier de *free* les chaînes retournées par *get_next_line* lorsque vous n'en avez plus l'utilité.

Ce projet était à l'origine un exercice donné lors de mon cursus au sein d'EPITECH.
Si vous êtes étudiant à EPITECH ou 42 et que vous souhaitez réutiliser ce code, pensez à remplacer certaines fonctions et de remplacer la structure de contrôle "for" par un "while" afin de respecter le sujet et la norme de votre école (fonctions interdites : strerror, exit, realloc et fprintf).
