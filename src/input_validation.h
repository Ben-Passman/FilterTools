#ifndef INPUT_VALIDATION
#define INPUT_VALIDATION

/************************************************************************************************
 * FilterTools/input_validation.h
 *
 * Author	: Ben Passman
 * Created	: 27/12/2020
 *
 * Description	: Functions to validate form inputs.
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 27/12/2020	Ben P		1.0	File created.
 *
 ************************************************************************************************ */

void strip_whitespace(const char *string, char *new_string);
int format_number_string(const char *string, const char *expression, char *result);
double ASCII_string_to_double(const char *string);
void print_scientific(double number);
void print_SI(double number);

#endif
