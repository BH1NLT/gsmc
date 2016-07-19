/*
 *   GSMC - A GTK Smith Chart Calulator for RF impedance matching
 * 
 *	(c) by Lapo Pieri IK5NAX  2003-2016,
 *          and Johannes van der Horst
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
 *  Send bugs reports, comments, critique, etc, to ik5nax@amsat.org or
 *  lapo_pieri@virgilio.it
 */


void psdrawArc(FILE* f, float x0,float y0,float w,float h,float a0,float a1);
void psdrawLine(FILE *, float,  float,  float,  float);
void writePS(char* b);
void psdrawX(FILE* f, float x, float sa, float al);
void psdrawR(FILE* f, float x, float sa, float al);
void psdrawchart(FILE *);
void psdrawRk(FILE *, float), psdrawXk(FILE *, float), 
  psdrawGk(FILE *, float), psdrawBk(FILE *, float),
  psdrawRHOk(FILE *, float), psdrawQk(FILE *, float),
  psdrawSP(FILE *, float, float), pscircdesc(FILE *f);
