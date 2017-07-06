#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

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
    note_t note;
    int octave_delta;

    if (argc != 3) {
        printf("Usage: %s NOTE OCTAVE_DELTA\n", argv[0]);
        return 0;
    }
    //
    note = (note_t)(toupper(argv[1][0]) - 64);
    switch(toupper(argv[1][0])) {
        case 'A': note = A; break;
        case 'B': note = B; break;
        case 'C': note = C; break;
        case 'D': note = D; break;
        case 'E': note = E; break;
        case 'F': note = F; break;
    }
    if (note >= END) {
        fprintf(stderr, "Invalid note\n");
        return 1;
    }
    octave_delta = atoi(argv[2]);
    //
    printf("%d\n", HALF_STEPS_PER_OCTAVE);
    printf("%f\n", freq(note, octave_delta));
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

