#include "utils.h"
#include <stdlib.h>

// can replace this as faster sorting algorithm
// goal : n log n
// now : n^2
void sort_array(int *table, int size) {
	int temp;

	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			if (table[i] > table[j]) {
				temp = table[i];
				table[i] = table[j];
				table[j] = temp;
			}
		}
	}
}

///
/// ft_split.c
///

static char	**do_split(char **tab, char *temp, char c);

static int	ft_strlen5(char *str, char c)
{
	int	i;

	i = 0;
	while (str[i] != '\0' && str[i] != c)
		i++;
	return (i);
}

static size_t	tab_size(char *s, char c)
{
	size_t	i;
	size_t	k;

	i = 0;
	k = 0;
	if (s[k] == c)
		if (s[k + 1] != c)
			k++;
	while (s[k])
	{
		if (s[k] == c)
		{
			while (s[k + 1] == c)
				k++;
			if (s[k + 1] != '\0' && s[k + 1] != c)
				i++;
		}
		k++;
	}
	return (i + 1);
}

static char	*put_word(char *str, char set, size_t c, char **tab)
{
	size_t		i;
	size_t		size;
	char		*word;

	i = 0;
	size = ft_strlen5(str, set);
	word = (char *)malloc(sizeof(char) * (size + 1));
	if (!word)
	{
		while (i < c)
		{
			free(tab[i]);
			tab[i] = NULL;
			i++;
		}
		free(tab);
		return (NULL);
	}
	while (i < size)
	{
		word[i] = str[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;
	char	*temp;

	if (!s)
		return (NULL);
	temp = (char *)s;
	tab = (char **)malloc(sizeof(char *) * (tab_size(temp, c) + 1));
	if (!tab)
		return (NULL);
	tab = do_split(tab, temp, c);
	return (tab);
}

static char	**do_split(char **tab, char *temp, char c)
{
	size_t	i;

	i = 0;
	while (*temp != 0)
	{
		while (*temp && *temp == c)
			temp++;
		if (*temp != 0)
		{
			tab[i] = put_word(temp, c, i, tab);
			if (tab[i] == NULL)
				return (NULL);
			i++;
		}
		while (*temp && !(*temp == c))
			temp++;
	}
	tab[i] = NULL;
	return (tab);
}

///
///
///