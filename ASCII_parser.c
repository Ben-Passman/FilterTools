/************************************************************************************************
 * FilterTools.ASCII_parser.c
 *
 * Author	: Ben Passman
 * Created	: 27/12/2020
 *
 * Description	: Converts ASCII text into numerical values with tolerance for small typos.
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 27/12/2020	Ben P	1.0	File created. Added functions to parse ASCII text to double.
 * 								Supports SI suffuxes and base 10 exponents.
 *
 ************************************************************************************************ */

#include "ASCII_parser.h"
#include <stdio.h>
//#include <string.h>
#include <math.h>
#include <regex.h>

#define ASCII_CHAR_IS_INT(c) c >= '0' && c <= '9'
#define INT_CHAR_TO_INT(c) (int) (c - '0')
#define INT_CHAR_TO_DOUBLE(c) (double) (c - '0')

#define SI_PREFIX_OFFSET 8
char SI_prefixes[17] = { 'y', 'z', 'a', 'f', 'p', 'n', 'u', 'm', ' ', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' };

#define char_to_SI_value(char) SI_prefix_values[char-'A']
double SI_prefix_values[58] = {
	1.0, 1.0, 1.0, 1.0, 1E18, 1.0, 1E9, 1.0, 1.0, 1.0, 1.0, 1.0, 1E6, 1.0, 
	1.0, 1E15, 1.0, 1.0, 1.0, 1E12, 1.0, 1.0, 1.0, 1.0, 1E24, 1E21,	1.0, 1.0, 
	1.0, 1.0, 1.0, 1.0, 1E-18, 1.0, 1.0, 1.0, 1.0, 1E-15, 1.0, 1.0, 1.0, 1.0, 1E3, 
	1.0, 1E-3, 1E-9, 1.0, 1E-12, 1.0, 1.0, 1.0, 1.0, 1E-6, 1.0, 1.0, 1.0, 1E-24, 1E-21
};

// Assumes new_string has sufficient capacity
void strip_whitespace(const char *string, char *new_string)
{
	int i = 0;
	int count = 0;
	while (*(string + i) != '\0')
	{
		if(*(string + i) != ' ')
		{
			*(new_string + count) = *(string + i);
			count++;
		}
		i++;
	}
	*(new_string + count) = '\0';
}

// Assumes result has sufficient capacity
int format_number_string(const char *string, const char *expression, char *result)
{
	regex_t regex;
	if (regcomp(&regex, expression, REG_EXTENDED) != 0)
	{
		printf("Regular expression failed to compile\n");
		regfree(&regex);
		return 0;
	}

	regmatch_t match;
	int status = regexec(&regex, string, 1, &match, 0);

	int index = 0;
	if (status == 0)
	{
		for (int i = match.rm_so; i < match.rm_eo; i++)
		{
			*(result + index++) = *(string + i);
		}
	}
	*(result + index) = '\0';

	regfree(&regex);
	return 1 - status;
}

// Parsing functions assume correct formatting and no whitespace
static double get_integer(const char *string, int *offset)
{
	int index = 0;
	char c = *string;
	double result = 0.0;
	
	while(ASCII_CHAR_IS_INT(c))
	{
		result *= 10.0;
		result += INT_CHAR_TO_DOUBLE(c);
		index++;
		c = *(string + index);
	}
	*offset = index;
	return result;
}

static double get_fraction(const char *string, int *offset)
{
	int index = 0;
	char c = *string;
	double power = 0.1;
	double result = 0.0;

	while(ASCII_CHAR_IS_INT(c))
	{
		result += INT_CHAR_TO_DOUBLE(c) * power;
		power *= 0.1;
		index++;
		c = *(string + index);
	}
	*offset = index;
	return result;
}

double ASCII_string_to_double(const char *string)
{
	int index = 0;
	int offset;
	double sign = 1.0;
	double result = 0.0;

	if (*string == '-')
	{
		sign = -1.0;
		index++;
	}

	result = get_integer((string + index), &offset);
	index += offset;

	char c = *(string + index);
	if(c == ',' || c == '.')
	{
		index++;
		result += get_fraction((string + index), &offset);
		index += offset;
	}
	result *= sign;

	c = *(string + index);
	if(c != '\0')
	{
		if(*(string + index + 1) == '\0')
		{
			result *= char_to_SI_value(c);
		}
		else if(c == 'e' || c == 'E')
		{
			double exponent_sign = 1.0;
			double exponent = 0.0;

			index++;
			c = *(string + index);
			if(c == '+')
			{
				index ++;
			}
			else if(c == '-')
			{
				exponent_sign = -1.0;
				index++;
			}
			exponent = get_integer((string + index), &offset);			
			result *= pow(10.0, exponent_sign * exponent); 
		}
	}

	return result;
}

void print_scientific(double number)
{
	int exponent = 0;
	if (number != 0.0)
	{
		while (number < 1 && number > -1)
		{
			number *= 10;
			exponent--;
		}
		while (number >= 10 || number <= -10)
		{
			number *= 0.1;
			exponent++;
		}
	}
	printf("%lfe%d\n", number, exponent);
}

void print_SI(double number)
{
	int index = SI_PREFIX_OFFSET;
	if(number != 0.0)
	{
		while (number < 1 && number > -1 && number != 0.0)
		{
			number *= 1000;
			index--;
		}
		while (number >= 1000 || number <= -1000)
		{
			number *= 0.001;
			index++;
		}
	}
	printf("%lf%c\n", number, SI_prefixes[index]);
}

/*int main(int argc, char ** argv)
{
	if(argc > 1)
	{
		double number = ASCII_string_to_double(argv[1]);
		printf("%lf\n", number);
		print_scientific(number);
		print_SI(number);
	}
	return 0;
}*/
