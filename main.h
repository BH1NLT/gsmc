/*
 *   GSMC - A GTK Smith Chart Calulator for RF impedance matching
 * 
 *	(c) by Lapo Pieri IK5NAX  2003-2016,
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Send bugs reports, comments, critique, etc, to
 *
 *        lapo_CHANGE_THIS_AT_CHANGE_THIS_radioteknos.it
 *        ik5nax_CHANGE_THIS_AT_CHANGE_THIS_radioteknos.it
 */


/*---------------------------------------------------------------------------
                         Definitions & Constants
  ---------------------------------------------------------------------------*/
#define NEL 32              /* Max number of network elements (???) */
#define QZ 0.05             /* Quasi-Zero: to draw arc as lines */
#define __VERSION "1.2.1"   /* Current version */
#define __VERSION_DATE "06/09/2016" /* version date */

#define SPTYPE_UNDEF 0
#define SPTYPE_Z 1
#define SPTYPE_Y 2
#define SPTYPE_RHO 3

#define labelZ0       0
#define labelf0       1
#define labelzSP      3
#define labelySP      4
#define labelrhoSP    5
#define labelSWRSP    6
#define labelzEP      8
#define labelyEP      9
#define labelrhoEP    10
#define labelSWREP    11
#define labeltunestep 13
#define labelfreqstep 14
#define labelzCU      16
#define labelyCU      17
#define labelrhoCU    18
#define labelSWRCU    19


/*---------------------------------------------------------------------------
                             Data Strucutures
  ---------------------------------------------------------------------------*/
typedef struct maindata{
  /* General data */
  float z0;        /* Characteristic impedance [ohm] */
  float f0;        /* Main frequency [Hz] */

  /* Cursor point */
  float rerhoCU;  /* Real part of reflection coefficient at cursor */
  float imrhoCU;  /* Imaginary part of reflection coefficient at cursor */

  /* Start Point */
  int SPtype;     /* Type of start point supplyed                            */
  float reentrySP;/* Real part of start point impedance [ohm]                */
  float imentrySP;/* Imaginary part of start point impedance [ohm]           */


  float rezSP;    /* Real part of start point impedance [ohm]                */
  float imzSP;    /* Imaginary part of start point impedance [ohm]           */
  float reySP;    /* Real part of start point admittance [S]                 */
  float imySP;    /* Imaginary part of start point admittance [S]            */
  float rerhoSP;  /* Real part of start point reflection coefficient []      */
  float imrhoSP;  /* Immaginary part of start point reflection coefficient []*/

  /* Network data */
  int ne;              /* Number of element present in network (<=NEL) */
  int neidx;           /* Currently selected element */
  char ELtype[NEL+1];    /* Network element type: R, L, C, T */
                       /* R=resistor, C=capacitor, L=inductor,
			  T=transmision line */
  float ELval0[NEL+1];   /* Value 0 for network element (R, L, C, lenght of T)
			  [ohm], [H], [F], [m] */
  float ELval1[NEL+1];   /* Value 0 for network element (z0 of T) [ohm] */
  float ELval2[NEL+1];   /* Value 0 for network element (atten of T) [dB/m]*/
  char ELplace[NEL+1];   /* Placement of component: s=series, p=parallel, ...
			  to be enlarged with transmission line */

  char ELlocked[NEL+1];  /* =1 if element is locked for autotune (i.e. cannot
			    be touched from autotune algorithm), 0 otherwise */

  /* Impedance point */
  float rerhoIP[NEL+1];
  float imrhoIP[NEL+1];

  /* Misc */
  char projectname[256]; /* Name of the project (i.e. the .gdt file) */

}SMCDATA;



/*---------------------------------------------------------------------------
                             Function Prototypes
  ---------------------------------------------------------------------------*/
gint mdw_expose_event(GtkWidget *, GdkEventExpose *, gpointer),
  mdw_configure_event(GtkWidget *, GdkEventExpose *, gpointer);

void writespice(char *);
void removene(void);
void loadgw(char *b), savegw(char *b);
