/* $Header: /home/cvs/src/window/src/wkeys.h,v 1.9 2007/01/09 18:06:18 piriej Exp $ */
/****************************************************************
**
**	WKEYS.H		- Window system keystroke definitions
**
**	$Version$
**
****************************************************************/

#ifndef IN_WKEYS_H
#define	IN_WKEYS_H	1


/*
**  Define keyboard key codes
*/

#define WK_UP			0x0148		// Up arrow
#define WK_DOWN			0x0150		// Down arrow
#define WK_LEFT			0x014B		// Left arrow
#define WK_RIGHT		0x014D		// Right arrow
#define WK_TABRT		0x0009		// Right tab or normal tab
#define WK_TABLF		0x010F		// Left tab or back tab
#define WK_PGUP			0x0149		// Page up
#define WK_PGDN			0x0151		// Page down
#define WK_END			0x014F		// End key
#define WK_HOME			0x0147		// Home key
#define WK_INS			0x0152		// Insert key
#define WK_DEL			0x0153		// Delete key

#define WK_ENTER		0x000D		// Enter key
#define WK_SPACE		0x0020		// Space bar
#define WK_ESC			0x001B		// Escape key
#define WK_BACK			0x0008		// Backspace key

#define WK_CLEFT		0x0173		// Control left arrow
#define WK_CRIGHT		0x0174		// Control right arrow
#define	WK_CPGUP		0x0184		// Control page up
#define	WK_CPGDN		0x0176		// Control page down
#define	WK_CEND			0x0175		// Control end
#define	WK_CHOME		0x0177		// Control home
#define	WK_CINS			0x0192		// Control insert
#define	WK_CDEL			0x0193		// Control delete
#define	WK_CTAB			0x0194		// Control tab
#define WK_CENTER		0x000A		// Control enter -- JWP 1/9/2007

#define WK_ALEFT		0x019B		// Alt left arrow
#define WK_ARIGHT		0x019D		// Alt right arrow

#define WK_F1			0x013B		// Normal function keys
#define WK_F2			0x013C
#define WK_F3			0x013D
#define WK_F4			0x013E
#define WK_F5			0x013F
#define WK_F6			0x0140
#define WK_F7			0x0141
#define WK_F8			0x0142
#define WK_F9			0x0143
#define WK_F0			0x0144
#define WK_F11			0x0185
#define WK_F12			0x0186

#define WK_SHFF1		0x0154		// Shift function keys
#define WK_SHFF2		0x0155
#define WK_SHFF3		0x0156
#define WK_SHFF4		0x0157
#define WK_SHFF5		0x0158
#define WK_SHFF6		0x0159
#define WK_SHFF7		0x015A
#define WK_SHFF8		0x015B
#define WK_SHFF9		0x015C
#define WK_SHFF0		0x015D
#define WK_SHFF11		0x0187
#define WK_SHFF12		0x0188

#define WK_CTLF1		0x015E		// Control function keys
#define WK_CTLF2		0x015F
#define WK_CTLF3		0x0160
#define WK_CTLF4		0x0161
#define WK_CTLF5		0x0162
#define WK_CTLF6		0x0163
#define WK_CTLF7		0x0164
#define WK_CTLF8		0x0165
#define WK_CTLF9		0x0166
#define WK_CTLF0		0x0167
#define WK_CTLF11		0x0189
#define WK_CTLF12		0x018A

#define WK_ALTF1		0x0168		// Alt function keys
#define WK_ALTF2		0x0169
#define WK_ALTF3		0x016A
#define WK_ALTF4		0x016B
#define WK_ALTF5		0x016C
#define WK_ALTF6		0x016D
#define WK_ALTF7		0x016E
#define WK_ALTF8		0x016F
#define WK_ALTF9		0x0170
#define WK_ALTF0		0x0171
#define WK_ALTF11		0x018B
#define WK_ALTF12		0x018C

#define WK_CTRLA		1
#define WK_CTRLB		2
#define WK_CTRLC		3
#define WK_CTRLD		4
#define WK_CTRLE		5
#define WK_CTRLF		6
#define WK_CTRLG		7
#define WK_CTRLH		8
#define WK_CTRLI		9
#define WK_CTRLJ		10
#define WK_CTRLK		11
#define WK_CTRLL		12
#define WK_CTRLM		13
#define WK_CTRLN		14
#define WK_CTRLO		15
#define WK_CTRLP		16
#define WK_CTRLQ		17
#define WK_CTRLR		18
#define WK_CTRLS		19
#define WK_CTRLT		20
#define WK_CTRLU		21
#define WK_CTRLV		22
#define WK_CTRLW		23
#define WK_CTRLX		24
#define WK_CTRLY		25
#define WK_CTRLZ		26

#define WK_ALTA			0x11e
#define WK_ALTB			0x130
#define WK_ALTC			0x12e
#define WK_ALTD			0x120
#define WK_ALTE			0x112
#define WK_ALTF			0x121
#define WK_ALTG			0x122
#define WK_ALTH			0x123
#define WK_ALTI			0x117
#define WK_ALTJ			0x124
#define WK_ALTK			0x125
#define WK_ALTL			0x126
#define WK_ALTM			0x132
#define WK_ALTN			0x131
#define WK_ALTO			0x118
#define WK_ALTP			0x119
#define WK_ALTQ			0x110
#define WK_ALTR			0x113
#define WK_ALTS			0x11f
#define WK_ALTT			0x114
#define WK_ALTU			0x116
#define WK_ALTV			0x12f
#define WK_ALTW			0x111
#define WK_ALTX			0x12d
#define WK_ALTY			0x115
#define WK_ALTZ			0x12c

#define	WK_ALT1			0x178
#define	WK_ALT2			0x179
#define	WK_ALT3			0x17a
#define	WK_ALT4			0x17b
#define	WK_ALT5			0x17c
#define	WK_ALT6			0x17d
#define	WK_ALT7			0x17e
#define	WK_ALT8			0x17f
#define	WK_ALT9			0x180
#define	WK_ALT0			0x181

#define	WK_ALTLBRACKET	0x11A
#define	WK_ALTRBRACKET	0x11B
#define	WK_ALTBACKSLASH	0x12B
#define	WK_ALTCOMMA		0x133
#define	WK_ALTPERIOD	0x134
#define	WK_ALTSEMICOLON	0x127
#define	WK_ALTRQUOTE	0x128
#define	WK_ALTLQUOTE	0x129
#define	WK_ALTINS		0x1A2

#define WK_CPRINT		0x172	// control-PrintScreen
#define WK_CTRL6  		0x01E
#define WK_CTRLMINUS	0x01F

#define WK_PLUS			0x02B
#define WK_MINUS		0x02D
#define WK_STAR			0x02A
#define WK_SLASH		0x02F

#define WK_ALTPLUS		0x183
#define WK_ALTMINUS		0x182
#define WK_ALTSTAR		0x17F
#define WK_ALTSLASH		0x135

#define WK_ALTKPLUS		0x14E
#define WK_ALTKMINUS	0x14A
#define WK_ALTKSTAR		0x137
#define WK_ALTKSLASH	0x1A4

#define WK_CTRLKPLUS  	0x190
#define WK_CTRLKMINUS	0x18E
#define WK_CTRLKSTAR  	0x196
#define WK_CTRLKSLASH	0x195

#define WK_TABLET		0x200

#define WK_KEY_ABORT	0x400

#endif
