/************************************************************************************************
 * FilterTools/waveforms.c
 * 
 * Author	: Ben Passman
 * Created	: 8/1/2021
 * 
 * Description	: Generator functions for creating test waveforms
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 8/1/2021	Ben P		1.0	File created
 *
 ************************************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long double saw_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double output_point = 2.0L * amplitude * freq * (t - delay) + amplitude;
	output_point = fmodl(output_point, 2.0L * amplitude) - amplitude;
	if (output_point < -amplitude)  // Correction for -ve time
	{
		output_point += 2.0L * amplitude;
	}

	return output_point;
}

long double triangle_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double output_point = 4.0L * amplitude * freq * (t - delay);
	output_point = fmodl(output_point, 4.0L * amplitude);
	if (output_point < 0)	// Correction for -ve time
	{
		output_point += 4.0L * amplitude;
	}
	if (output_point > amplitude)	
	{
		output_point = 2.0L * amplitude - output_point;
	}
	if (output_point < -amplitude)
	{
		output_point = -output_point - 2.0L * amplitude;
	}

	return output_point;
}

long double square_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double T = 1 / freq;
	long double delay = phase / (2.0L * acosl(-1) * freq);
	long double local_time = fmodl(t - delay, T);
	if (local_time < 0) // Correction for -ve time
	{
		local_time = T + local_time;
	}

	long double output_point = amplitude;
	if (local_time > duty * T) 
	{
		output_point = -amplitude;
	}

	return output_point;
}

long double sine_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty)
{
	long double w = (t * 2.0L * acosl(-1) * freq) - phase;
	long double outputPoint = amplitude * sinl(w);

	return outputPoint;
}

void generate_signal(const int number_of_samples, const long double sample_rate)
{
	FILE *fp;
	fp = fopen("FFT Test Data.csv", "w");

	if(fp != NULL) {
		long double T = 1.0/sample_rate;	// Sampling period
		long double pi = acosl(-1);		// Use long double for Pi
	
		for(long double i = -number_of_samples * T; i <= number_of_samples * T; i+=T)
		{
			long double w = i * 2.0 * pi;
			long double test_wave = sinl(5.0*w) + 
						sinl(10.0*w) + 
						sinl(50.0*w) + 
						sinl(55.0*w) + 
						sinl(300.0*w) +
						sinl(450.0*w) + 
						//sinl(7000.0*w) + 
						//sinl(10000.0*w) + 
						triangle_wave(i, 7000, 1.2, 0, 0) +
						saw_wave(i, 10000, 1.4, 1.5, 0) +
						square_wave(i, 20000, 1.0, 0.75, 0.3) +
						//sinl(12000.0*w) + 
						//sinl(25000.0*w);
						sine_wave(i, 25000, 1.0, 1.5, 0);
			fprintf(fp, "%Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf, %Lf\n",
				i, 
				sinl(5.0*w), 
				sinl(10.0*w), 
				sinl(50.0*w), 
				sinl(55.0*w), 
				sinl(300.0*w),
				sinl(450.0*w), 
				//sinl(7000.0*w), 
				//sinl(10000.0*w), 
				triangle_wave(i, 2000, 1.0, 0.75, 0),
				saw_wave(i, 1000, 1.4, 1.5, 0),
				square_wave(i, 2000, 1.0, 3.14, 0.3),
				sine_wave(i, 25000, 1.0, 1.5, 0),
				//sinl(12000.0*w), 
				//sinl(25000.0*w),
				test_wave);
		}
	}
	else
	{
		printf("Could not open file for editing.\n");
		exit(1);
	}
	fclose(fp);
}