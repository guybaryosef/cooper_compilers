/*
 *
 * 
 * Has some functions for conversions between
 * number systems.
 *
 */

#include <math.h>

/*
 * dec2oct - Converts a decimal number into
 * an octal number.
 */
int dec2oct(int val) {
    
}


/*
 * hex2dec - Converts a hexadecimal number into
 * a decimal number.
 * 
 * Note: The hexadecimal value can be of any length, 
 * yet reaches saturation overflow at the value 255. 
 */
int hex2dec(char *hex) {
    int i = strlen(hex)-1;
    int j = 0;
    int val = 0;
    while (i > 1) {
        if (hex[i] >= 'a' && hex[i] <= 'f')
            val += pow(16,j++)*(hex[i--]-'a');
        else if (hex[i] >= 'A' && hex[i] <= 'F')
            val += pow(16,j++)*(hex[i--]-'A');
        else 
            val += pow(16,j++)*(hex[i--]-'0');
    }
    return (val>255) ? 255 : val;
}