#ifndef TERMIOS_H
#define TERMIOS_H

typedef uint8_t cc_t;         // used for terminal special characters
typedef uint32_t speed_t;     // used for terminal baud rates
typedef uint32_t tcflag_t;    // used for terminal modes

#define NCCS   32    // size of the array c_cc for control characters

// local mode definitions
#define ECHO   1      // enable echo
#define ECHOE  2      // echo erase character as error-correcting backspace
#define ECHOK  4      // echo KILL
#define ECHONL 8      // echo NL
#define ICANON 16     // canonical input (erase and kill processing)
#define IEXTEN 32     // enable extended input character processing
#define ISIG   64     // enable signals
#define NOFLSH 128    // disable flush after interrupt or quit
#define TOSTOP 256    // send SIGTTOU for background output

// control characters indices
#define VEOF   1     // EOF character
#define VEOL   2     // EOL character
#define VERASE 3     // ERASE character
#define VINTR  4     // INTR character
#define VKILL  5     // KILL character
#define VMIN   6     // MIN character
#define VQUIT  7     // QUIT character
#define VSTART 8     // START character
#define VSTOP  9     // STOP character
#define VSUSP  10    // SUSP character
#define VTIME  11    // TIME character

typedef struct
{
	tcflag_t c_iflag;    // input modes
	tcflag_t c_oflag;    // output modes
	tcflag_t c_cflag;    // control modes
	tcflag_t c_lflag;    // local modes
	cc_t c_cc[NCCS];     // control characters
} termios;

#endif    // TERMIOS_H
