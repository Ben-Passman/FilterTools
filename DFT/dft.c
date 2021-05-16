#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <math.h>
#include <complex.h>

#define BUFFER_SIZE 32
#define ASCII_TO_INT(x) (x - 48)
#define IS_ASCII_INT(x) (x >= 48 && x <= 57)

// Pattern for decimal numbers: "-\\?[0-9 ]*[.,][0-9 ]*"
int regex_test(char *pattern, char *string)
{
    regex_t re;

    if (regcomp(&re, pattern, 0) != 0)
    {
	printf("Regex failed to compile.\n");
	regfree(&re);
	return 0;
    }

    size_t nmatches = 2;
    regmatch_t matches[2];
    int status = regexec(&re, string, nmatches, matches, 0);
    printf("Test string: %s : %d -> %d %d\n", string, status, matches[0].rm_so, matches[0].rm_eo);

    regfree(&re);

    return 1;
}

double complex nth_root (const int n, const int N)
{
    //double pi = acos(-1);
    double angle = 2.0f * M_PI * n / N;
    return cos(angle) - sin(angle) * I;
}

void DFT (const double complex *time, double complex *freq, const int N)
{	
    for (int k = 0; k < N; k++)
    {
	*(freq + k) = 0.0 + 0.0 * I;
	for (int n = 0; n < N; n++)
	{
	    *(freq + k) += *(time + n) * nth_root(n*k, N);
	}
    }
}

void inverse_DFT (double complex *time, const double complex *freq, const int N)
{
    for (int n = 0; n < N; n++)
    {
	*(time + n) = 0.0 + 0.0 * I;
	for (int k = 0; n < N; n++)
	{
	    *(time + n) += *(freq + k) * nth_root(-n*k, N);
	}
	*(time + n) /= N;
    }
}

int alloc_csv_data(const char *filepath, double complex **data)
{
    FILE *fp = fopen(filepath, "r");
    if(fp == NULL)
    {
	return 0;
    }
    
    char buff[BUFFER_SIZE];
    int size = 0;
    
    while(fgets(buff, BUFFER_SIZE, fp))
    {
	size++;
    }

    *data = calloc(size - 1, sizeof(double complex));
    fseek(fp, 0, 0);
    size = 0;
    fgets(buff, BUFFER_SIZE, fp);

    while (fgets(buff, BUFFER_SIZE, fp))
    {
	int index = 0;
	double sign = 1.0f;
	while (index < BUFFER_SIZE && buff[index] != ' ')
	{
	    index++;
	}
	index++;

	if (buff[index] == '-')
	{
	    sign = -1.0f;
	    index++;
	}

	double num = 0.0f;
	while (index < BUFFER_SIZE && IS_ASCII_INT(buff[index]) && buff[index] != '\n')
	{
	    num *= 10.0f;
	    num += ASCII_TO_INT(buff[index]);
	    index++;
	}
	index++;
	double frac = 0.0f;
	double scale = 1.0f;
	while (index < BUFFER_SIZE && IS_ASCII_INT(buff[index]) && buff[index] != '\n')
	{
	    frac *= 10.0f;
	    scale *= 10.0f;
	    frac += ASCII_TO_INT(buff[index]);
	    index++;
	}
	num += frac / scale;
	*(*data + size) = sign * num + 0.0 * I;
	size++;
    }
    fclose(fp);

    return size;
}

int write_output(const char *filename, const double complex *data, int size)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
	printf("ERROR :: Failed to open output file for writing\n");
	fclose(fp);
	return 0;
    }
    double fs = 1000.0;
    fprintf(fp, "Index, Real, Imaginary\n");
    for (int i = 0; i < size; i++)
    {
	fprintf(fp, "%lf, %lf, %lf\n", i * fs / 100, creal(*(data + i)), cimag(*(data + i)));
	//fprintf(fp, "%lf, %lf, %lf\n", i * fs / 100, cabs(*(data + i)), carg(*(data + i)));
    }
    fclose(fp);
    return 1;
}

int main (void)
{
    int size = 0;
    double complex *data = NULL;
    double complex *freq = NULL;

    size = alloc_csv_data("../signal-generator/Test Data.csv", &data);
    freq = calloc(size, sizeof(double complex));	
    
    if(data == NULL || freq == NULL)
    {
	printf("ERROR :: Failed to allocate memory\n");
	return 0;
    }
    
    DFT(data, freq, size);
    write_output("DFT.csv", freq, size);

    inverse_DFT(data, freq, size);
    write_output("inverse DFT.csv", data, size);

    free(data);
    free(freq);

    return 1;
}
