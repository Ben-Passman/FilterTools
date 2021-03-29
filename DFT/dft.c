#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define BUFFER_SIZE 32
#define ASCII_TO_INT(x) (x - 48)
#define IS_ASCII_INT(x) (x >= 48 && x <= 57)

double complex nth_root (int n, int N)
{
	//double pi = acos(-1);
	double angle = 2.0f * M_PI * n / N;
	return cos(angle) - sin(angle) * I;
}

void DFT (double complex *time, double complex *freq, int N)
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

int main (int argc, char *argv)
{
	FILE *fp;
	char buff[BUFFER_SIZE];
	int size = 0;
	double complex *data;
	double complex *freq;

	fp = fopen("../signal-generator/Test Data.csv", "r");
	if (fp != NULL)
	{	
		while (fgets(buff, BUFFER_SIZE, fp))
		{
			size++;
		}
	
		data = calloc(size - 1, sizeof(double complex));
		freq = calloc(size - 1, sizeof(double complex));
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
			*(data + size) = sign * num + 0.0 * I;
			size++;
		}
	}
	fclose(fp);

	DFT(data, freq, size);

	fp = fopen("Test Data.csv", "w");
	if (fp != NULL)
	{
		double fs = 1000.0;
		fprintf(fp, "Index, Magnitude, Phase\n");
		for (int i = 0; i < size; i++)
		{
			fprintf(fp, "%lf, %lf, %lf\n", i * fs / 100, creal(*(freq + i)), cimag(*(freq + i)));
		}
	}
	fclose(fp);

	free(data);
}
