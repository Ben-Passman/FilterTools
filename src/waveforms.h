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

#define WAVETYPE_MENU { "Sine    ", "Cosine  ", "Sawtooth", "Triangle", "Square  " }
#define WAVEMODE_MENU { "Add     ", "Subtract", "AM      ", "Divide  ",  "FM      " }

enum WaveType { SINE, COSINE, SAWTOOTH, TRIANGLE, SQUARE };
enum WaveMode { ADD, SUBTRACT, AM, DIVIDE, FM };

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
    char *export_path;
    int sample_count;
    double sample_frequency;
    struct WaveForm *first;
    struct WaveForm *selected;
};

void add_wave (struct WaveList *list);
void delete_wave (struct WaveList *list);
int export_wave (struct WaveList *list); // Print null at end
void move_selected_wave_up (struct WaveList *list);
void move_selected_wave_down (struct WaveList *list); 

#endif
