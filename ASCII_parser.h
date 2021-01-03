/************************************************************************************************
 * FilterTools/ASCII_parser.h
 *
 * Author	: Ben Passman
 * Created	: 27/12/2020
 *
 * Description	: Convert ASCII input into numerical values.
 *
 * Revision History:
 * Date		Author		Rev	Notes
 * 27/12/2020	Ben P		1.0	File created. Added lookup table for SI unit conversion.
 *
 ************************************************************************************************ */

#define REGEX_NUMERIC_PATTERN "-?([0-9]*[.|,])?[0-9]+(([eE][+-]?[0-9]+)|[kMGTPEZY])?"

#define SI_DEFAULT 8
#define char_to_SI_value(char) SI_prefix_values[char-65]

void copy_non_whitespace(const char *, char *);
int format_number_string(const char *, const char *, char *);
double ASCII_string_to_double(const char *);
