#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define A4 440.0 //fixed frequency
#define F0 A4
#define F0_note A
#define F0_octave 4

typedef enum {C=1, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B,
    END = B, HALF_STEPS_PER_OCTAVE = B 
} note_t;

double freq(note_t note, int octave_delta);


int main(int argc, char *argv[])
{
    double frequen;
    double tolerance = 0.01;
    double array[] = { 16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12,	24.50,
	25.96, 27.50, 29.14, 30.87, 32.70, 34.65, 36.71, 38.89, 41.20, 43.65,
	46.25, 49.00, 51.91, 55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78,
	82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47, 130.81,
	138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00,
	233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99,
	392.00, 415.30, 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25,
	659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77, 1046.50,
	1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22,
	1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02, 2637.02,
	2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01,
	4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88,
	7040.00, 7458.62, 7902.13 };


    note_t note;
    int num = 1;
    int k = 0, i = 0;
    //note_t note;
    //int octave_delta;

    //if (argc != 3) {
    //printf("Usage: %s NOTE OCTAVE_DELTA\n", argv[0]);
    //return 0;
    //}
    //
    //note = (note_t)(toupper(argv[1][0]) - 64);
    while(k <= 8) {
	while(num <= 12) {
	    switch(num) {
		case 10 : note = A; break;
		case 11 : note = As; break;
		case 12 : note = B; break;
		case  1 : note = C; break;
		case  2 : note = Cs; break;
		case  3 : note = D; break;
		case  4 : note = Ds; break;
		case  5 : note = E; break;
		case  6 : note = F; break;
		case  7 : note = Fs; break;
		case  8 : note = G; break;
		case  9 : note = Gs; break;
	    }
	    frequen = freq(note, k);
	    printf("Frequency: %f \n", frequen);
	    if(fabs(frequen - array[i]) > tolerance)
		printf("Error: Not Expected \n");
	    num++;
	    i++;
	}
	k++;
	num = 1;
    }
    /*
       if (note >= END) {
       fprintf(stderr, "Invalid note\n");
       return 1;
       }
       octave_delta = atoi(argv[2]);
    //
    //printf("%d\n", HALF_STEPS_PER_OCTAVE);
    printf("%f\n", freq(note, octave_delta));
    */
    return 0;
}

//
//Generate a frequency in hz that is half_steps away from C_4
//
double freq(note_t note, int octave_delta)
{
    double freq;
    double factor;
    double a;
    int n;
    int octave = octave_delta - F0_octave;

    a = pow(2.0, 1.0/(double)HALF_STEPS_PER_OCTAVE);
    n = note - F0_note;
    freq = F0 * pow(a, (double)n);
    factor = pow(2.0, (double)octave);
    freq = freq * factor;
    return freq;
}
