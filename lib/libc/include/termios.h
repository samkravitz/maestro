#ifndef TERMIOS_H
#define TERMIOS_H

typedef uint8_t cc_t;         // used for terminal special characters
typedef uint32_t speed_t;     // used for terminal baud rates
typedef uint32_t tcflag_t;    // used for terminal modes

#define NCCS 32    // size of the array c_cc for control characters

typedef struct
{
	tcflag_t c_iflag;    // input modes
	tcflag_t c_oflag;    // output modes
	tcflag_t c_cflag;    // control modes
	tcflag_t c_lflag;    // local modes
	cc_t c_cc[NCCS];     // special characters
} termios;

#endif    // TERMIOS_H
