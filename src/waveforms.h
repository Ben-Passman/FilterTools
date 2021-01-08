#ifndef WAVEFORMS
#define WAVEFORMS

/************************************************************************************************
 * FilterTools/waveforms.h
 * 
 * Author	: Ben Passman
 * Created	: 8/1/2021
 * 
 * Description	: Generator functions for creating test waveforms
 *
 * Revision History:
 * Date         Author      Rev     Notes
 * 8/1/2021     Ben P       1.0     File created
 *
 ************************************************************************************************ */

long double saw_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
long double triangle_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
long double square_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
long double sine_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
void generate_signal(const int number_of_samples, const long double sample_rate);

#endif
