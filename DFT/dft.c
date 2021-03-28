#include <stdio.h>
#include <stdlib.h>
#include <complex.h>

#define BUFFER_SIZE 32
#define ASCII_TO_INT(x) (x - 48)
#define IS_ASCII_INT(x) (x >= 48 && x <= 57)

int main (int argc, char *argv)
{
	FILE *fp;
	char buff[BUFFER_SIZE];
	int size = 0;
	double complex *data;

	fp = fopen("../signal-generator/Test Data.csv", "r");
	
	while (fgets(buff, BUFFER_SIZE, fp))
	{
		size++;
	}
	
	data = calloc(size - 1, sizeof(double complex));
	fseek(fp, 0, 0);
	size = 0;
	fgets(buff, BUFFER_SIZE, fp);
	
	while (fgets(buff, BUFFER_SIZE, fp))
	{
		int index = 0;
		while (index < BUFFER_SIZE && buff[index] != ' ')
		{
			index++;
		}
		index++;
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
		*(data + size) = num + 0.0 * I;
		size++;
	}

	for (int i = 0; i < size; i++)
	{
		printf("%lf, %lf\n", creal(*(data + i)), cimag(*(data + i)));
	}

	fclose(fp);
	free(data);
}
