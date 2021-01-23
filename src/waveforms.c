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

#include "waveforms.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long double saw_wave(const long double t, const struct WaveForm *wave)
{
	long double delay = wave->phase / (wave->frequency * 360);
	long double output_point = 2.0L * wave->amplitude * wave->frequency * (t + delay) + wave->amplitude;
	output_point = fmodl(output_point, 2.0L * wave->amplitude) - wave->amplitude;
	if (output_point < -wave->amplitude)  // Correction for -ve time
	{
		output_point += 2.0L * wave->amplitude;
	}

	return output_point + wave->dc_offset;
}

long double triangle_wave(const long double t, const struct WaveForm *wave)
{
	long double delay = wave->phase / (wave->frequency * 360);
	long double output_point = 4.0L * wave->amplitude * wave->frequency * (t + delay);
	output_point = fmodl(output_point, 4.0L * wave->amplitude);
	if (output_point < 0)	// Correction for -ve time
	{
		output_point += 4.0L * wave->amplitude;
	}
	if (output_point > wave->amplitude)	
	{
		output_point = 2.0L * wave->amplitude - output_point;
	}
	if (output_point < -wave->amplitude)
	{
		output_point = -output_point - 2.0L * wave->amplitude;
	}

	return output_point + wave->dc_offset;
}

long double square_wave(const long double t, const struct WaveForm *wave)
{
	long double T = 1 / wave->frequency;
	long double delay = T * wave->phase / 360;
	long double local_time = fmodl(t + delay, T);

	if (local_time < 0) // Correction for -ve time
	{
		local_time = T + local_time;
	}

	long double output_point = wave->amplitude;
	if (local_time >= wave->duty * T) 
	{
		output_point = -output_point;
	}

	return output_point + wave->dc_offset;
}

long double sine_wave(const long double t, const struct WaveForm *wave)
{
	long double w = acosl(-1) * (2.0L * wave->frequency * t + wave->phase / 180.0);
	long double outputPoint = wave->amplitude * sinl(w) + wave->dc_offset;

	return outputPoint;
}

int export_wave (struct WaveList *list)
{
	FILE *fp;
	fp = fopen("Test Data.csv", "w");

	if (fp != NULL)
	{
		long double T = 1.0 / list->sample_frequency;
		struct WaveForm *last = list->first;

		while (last->next != NULL)
		{
			last = last->next;
		}		

		for (long double t = 0; t <= list->sample_count * T; t += T)
		{
			fprintf(fp, "%Lf, %Lf, %Lf, %Lf, %Lf\n", t, sine_wave(t, last), saw_wave(t, last), triangle_wave(t, last), square_wave(t, last));
			
		}
		fclose(fp);
		return 1;
	}
	else
	{
		printf("Could not open file for editing.\n");
		return 0;
	}
}

void add_wave(struct WaveList *list)
{
	struct WaveForm *selection = list->selected;
	struct WaveForm *new_wave = calloc(1, sizeof(struct WaveForm));

	new_wave->type = SINE;
	new_wave->amplitude = 1.0;
	new_wave->frequency = 1000.0;
	new_wave->phase = 0.0;
	new_wave->duty = 0.0;
	new_wave->mode = ADD;
	new_wave->dc_offset = 0.0;
	new_wave->next = NULL;
	new_wave->previous = NULL;

	if (list->first == NULL)
	{
		list->first = new_wave;
		list->selected = new_wave;
	}
	else
	{
		// Insert new_wave after selection
		new_wave->next = selection->next;
		new_wave->previous = selection;
		selection->next = new_wave;
		if (new_wave->next != NULL)
		{
			new_wave->next->previous = new_wave;
		}

		list->selected = new_wave;
	}
}

void delete_wave(struct WaveList *list)
{
	if (list->selected != NULL)
	{
		struct WaveForm *old_wave = list->selected;
		struct WaveForm *next_wave = old_wave->next;
		struct WaveForm *previous_wave = old_wave->previous;

        if (previous_wave != NULL)
		{
			previous_wave->next = old_wave->next;
			list->selected = previous_wave;
		}
		else
        {
            list->first = old_wave->next;
        }
		if (next_wave != NULL)
		{
			next_wave->previous = old_wave->previous;
			list->selected = next_wave;
		}
		if (list->selected == old_wave)
		{
			list->first = NULL;
			list->selected = NULL;
		}
		free(old_wave);
	}
}
