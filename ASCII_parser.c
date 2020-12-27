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
 * 27/12/2020	Ben P		1.0	File created. Added functions to parse ASCII text to double.
 * 					Supports SI suffuxes and base 10 exponents.
 *
 ************************************************************************************************ */

#include "ASCII_parser.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

int ASCII_char_to_int(const char c)
{
    if (c > 47 && c < 58) // if input char is '0'-'9'
    {
        return (int) c - 48; // (int) '0' = 48;
    }
    else
    {
        return -1;
    }
}

double get_sign(const char * string, int * index)
{
    int length = strlen(string);
    double sign = 1.0;
    while (*index < length && ASCII_char_to_int(string[*index]) < 0 && string[*index] != ',' && string[*index] != '.')
    {
	if(string[*index] == '-')
	{
	    sign = -1.0;
	}
	else if (string[*index] != ' ')
	{   
	    sign = 1.0;
	}
	(*index)++;
    }

    return sign;
}

double get_integer(const char * string, int * index)
{
    int length = strlen(string);
    double integer = 0.0;
    while (*index < length)
    {
	int test = ASCII_char_to_int(string[*index]);
	if (test >= 0)
	{
		integer *= 10;
		integer += test;
	}
	else if (string[*index] != ' ')
	{
		return integer;
	}

	(*index)++;
    }
    return integer;
}

double get_fraction(const char * string, int * index)
{
    int length = strlen(string);
    double fraction = 0.0;
    double power = 0.1;
    while (*index < length)
    {
	int test = ASCII_char_to_int(string[*index]);
	if (test >= 0)
	{
		fraction += test * power;
		power /= 10.0;
	}
	else if (string[*index] != ' ')
	{
		return fraction;
	}
	
	(*index)++;
    }

    return fraction;
}

double ASCII_string_to_double(const char * input)
{
    int length = strlen(input);
    int index = 0;
    double sign = 1.0;
    double base = 0.0;
    double exponent = 1.0;
    
    sign = get_sign(input, &index);
    base = get_integer(input, &index);
    if(index < length && (input[index] == ',' || input[index] == '.'))
    {
	index++;
	base += get_fraction(input, &index);
    }
    base *= sign;

    if(index < length && (input[index] == 'e' || input[index] == 'E'))
    {
	index++;
	double exponent_sign = get_sign(input, &index);
	exponent = get_integer(input, &index);
	exponent *= exponent_sign;
	base *= pow(10.0, exponent);
	index = length;
    }

    while (index < length)
    {
	// check for SI suffix
	switch(input[index])
	{
	    case 'Y' :
	    case 'Z' :
	    case 'E' :
	    case 'P' :
	    case 'T' :
	    case 'G' :
	    case 'M' :
	    case 'k' :
	    case 'm' :
	    case 'u' :
	    case 'n' :
	    case 'p' :
	    case 'f' :
	    case 'a' :
	    case 'z' :
	    case 'y' :
		base *= SI_suffixes[(int)input[index] - 65];
		index = length;
	    default :
		break;
	}
	index++;
    }
    
    return base;
}

void print_scientific(double number)
{
    int exponent = 0;
    if (number == 0.0)
    {
	printf("0.0e0\n");
	return;
    }
    while (number < 1 && number > -1)
    {
	number *= 10;
	exponent--;
    }
    while (number >= 10 || number <= -10)
    {
	number /= 10;
	exponent++;
    }
    printf("%lfe%d\n", number, exponent);
}

int main(int argc, char ** argv)
{
    if(argc > 1)
    {
	double number = ASCII_string_to_double(argv[1]);
	printf("%lf\n", number);
	print_scientific(number);

    }
    return 0;
}
