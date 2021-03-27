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

typedef long double (WaveGenerator)(const long double t, const struct WaveForm *wave);
typedef void (WaveOperation)(WaveGenerator *f, const long double t, const struct WaveForm *wave, long double *output);

static long double saw_wave(const long double t, const struct WaveForm *wave)
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

static long double triangle_wave(const long double t, const struct WaveForm *wave)
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

static long double square_wave(const long double t, const struct WaveForm *wave)
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

static long double sine_wave(const long double t, const struct WaveForm *wave)
{
	long double w = acosl(-1) * (2.0L * wave->frequency * t + wave->phase / 180.0);
	
	return wave->amplitude * sinl(w) + wave->dc_offset;
}

static long double cosine_wave(const long double t, const struct WaveForm *wave)
{
	long double w = acosl(-1) * (2.0L * wave->frequency * t + wave->phase / 180.0);
	
	return wave->amplitude * cosl(w) + wave->dc_offset;
}

static void wave_add(WaveGenerator *f, const long double t, const struct WaveForm *wave, long double *output)
{
	*output += f(t, wave);
}

static void wave_subtract(WaveGenerator *f, const long double t, const struct WaveForm *wave, long double *output)
{
	*output -= f(t, wave);
}

static void wave_AM(WaveGenerator *f, const long double t, const struct WaveForm *wave, long double *output)
{
	*output *= f(t, wave);
}

static void wave_divide(WaveGenerator *f, const long double t, const struct WaveForm *wave, long double *output)
{
	*output /= f(t, wave);
}

static void wave_FM(WaveGenerator *f, const long double t, const struct WaveForm *wave, long double *output)
{
	struct WaveForm temp = *wave;
	temp.frequency *= *output;
	*output = f(t, &temp);
}

int export_wave (struct WaveList *list)
{
	if (list->first == NULL)
	{
		printf("Wave list is empty.");
		return 0;
	}

	FILE *fp;
	fp = fopen("Test Data.csv", "w");	
	if (fp != NULL)
	{
		WaveGenerator *generate[5];
		generate[SINE] = &sine_wave;
		generate[COSINE] = &cosine_wave;
		generate[SAWTOOTH] = &saw_wave;
		generate[TRIANGLE] = &triangle_wave;
		generate[SQUARE] = &square_wave;

		WaveOperation *combine[6];
		combine[ADD] = &wave_add;
		combine[SUBTRACT] = &wave_subtract;
		combine[AM] = &wave_AM;
		combine[DIVIDE] = &wave_divide;
		combine[FM] = &wave_FM;

		long double T = 1.0 / list->sample_frequency;
		struct WaveForm *selected_wave = list->first;

		while (selected_wave->next != NULL)
		{
			selected_wave = selected_wave->next;
		}		

		long double *buffer = malloc(list->sample_count * sizeof(long double));
		for (int i = 0; i < list->sample_count; i++)
		{
			*(buffer + i) = generate[selected_wave->type](i * T, selected_wave); // WAVE_TYPE
		}

		// Waves combined from bottom of list up.
		selected_wave = selected_wave->previous;
		while (selected_wave != NULL)
		{
			for (int i = 0; i < list->sample_count; i++)
			{
				combine[selected_wave->next->mode](generate[selected_wave->type], i * T, selected_wave, buffer + i);
			}
			selected_wave = selected_wave->previous;
		}

		fprintf(fp, "Time (s), Combined Signal\n");
		for (int i = 0; i < list->sample_count; i++)
		{
			fprintf(fp, "%Lf, %Lf\n", i * T, *(buffer + i));
		}

		free(buffer);
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

void move_selected_wave_up (struct WaveList *list)
{
	struct WaveForm *target = list->selected;
	
	if (target != NULL && target->previous != NULL)
	{
		struct WaveForm *b = target->previous;
		struct WaveForm *c = target->next;
		
		if (c != NULL)
		{	
			b->next = c;
			c->previous = b;
		}
		else
		{
			b->next = NULL;
		}
		
		target->next = b;

		if (b->previous != NULL)
		{
			struct WaveForm *a = b->previous;

			target->previous = a;
			a->next = target;
		}
		else
		{
			target->previous = NULL;
			list->first = target;
		}
		b->previous = target;
	}
}

void move_selected_wave_down (struct WaveList *list)
{
	struct WaveForm *target = list->selected;

	if (target != NULL && target->next != NULL)
	{
		struct WaveForm *a = target->previous;
		struct WaveForm *b = target->next;

		if (a != NULL)
		{
			a->next = b;
			b->previous = a;
		}
		else
		{
			b->previous = NULL;
			list->first = b;
		}

		target->previous = b;

		if (b->next != NULL)
		{
			struct WaveForm *c = b->next;

			target->next = c;
			c->previous = target;
		}
		else
		{
			target->next = NULL;
		}
		b->next = target;
	}
}
