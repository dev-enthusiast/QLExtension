/* $Header: /home/cvs/src/tone/src/tone.h,v 1.3 1998/10/09 22:27:37 procmon Exp $ */
/****************************************************************
**
**	TONE.H	- Music functions
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_TONE_H )
#define IN_TONE_H


/*
**	Define types
*/

typedef unsigned long	MUSIC_NOTE;


/*
**	Note contants
*/

#define	MUSIC_OCTAVE_SIZE  	12L

#define	MUSIC_OCTAVE_0		(MUSIC_OCTAVE_SIZE * 0L)
#define	MUSIC_OCTAVE_1		(MUSIC_OCTAVE_SIZE * 1L)
#define	MUSIC_OCTAVE_2		(MUSIC_OCTAVE_SIZE * 2L)
#define	MUSIC_OCTAVE_3		(MUSIC_OCTAVE_SIZE * 3L)
#define	MUSIC_OCTAVE_4		(MUSIC_OCTAVE_SIZE * 4L)
#define	MUSIC_OCTAVE_5		(MUSIC_OCTAVE_SIZE * 5L)
#define	MUSIC_OCTAVE_6		(MUSIC_OCTAVE_SIZE * 6L)
#define	MUSIC_OCTAVE_7		(MUSIC_OCTAVE_SIZE * 7L)

#define	MUSIC_NOTE_A		0L
#define	MUSIC_NOTE_B		2L
#define	MUSIC_NOTE_C		3L
#define	MUSIC_NOTE_D		5L
#define	MUSIC_NOTE_E		7L
#define	MUSIC_NOTE_F		8L
#define	MUSIC_NOTE_G		10L

#define	MUSIC_SHARP			1L
#define	MUSIC_FLAT			(-1L)

#define	MUSIC_WHOLE			0x00200000L
#define	MUSIC_HALF			0x00100000L
#define	MUSIC_QUARTER		0x00080000L
#define	MUSIC_EIGHTH		0x00040000L
#define	MUSIC_SIXTEENTH		0x00020000L
#define	MUSIC_THIRTYSECOND	0x00010000L

#define	MUSIC_LEGATO		0x01000000L
#define	MUSIC_STACCATO		0x02000000L

#define	MUSIC_REST			0x10000000L

#define	MUSIC_END			0x00000000L


/*
**	Prototype functions
*/

DLLEXPORT void	MusicPlayNote(		MUSIC_NOTE MusicNote, unsigned int Tempo ),
		MusicPlayTune(		MUSIC_NOTE *Tune, unsigned int Tempo );


#endif
