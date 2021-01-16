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

enum WaveType { SINE, COSINE, SAWTOOTH, TRIANGLE, SQUARE };
enum WaveMode { ADD, SUBTRACT, MULTIPLY, DIVIDE, AM, FM };

struct WaveForm {
    enum WaveType type;
    double amplitude;
    double frequency;
    double phase;
    double duty;
    enum WaveMode mode;
    double dc_offset;
    struct WaveForm *previous;
    struct WaveForm *next;
};

struct WaveList {
    struct WaveForm *first;
    struct WaveForm *selected;
};

long double saw_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
long double triangle_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
long double square_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
long double sine_wave(const long double t, const double freq, const double amplitude, const double phase, const double duty);
void generate_signal(const int number_of_samples, const long double sample_rate);

//move_wave_up { if previous != NULL }
//move_wave_down { if next != NULL }
void add_wave(struct WaveList *list);
void delete_wave(struct WaveList *list);
//print_wave { } // Print null at end

#endif
