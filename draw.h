/*
 *   GSMC - A GTK Smith Chart Calulator for RF impedance matching
 * 
 *	(c) by Lapo Pieri IK5NAX  2003-2004,
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


void setup_gcs(void);
void drawRk(float), drawGk(float), drawXk(float), drawBk(float),
  drawRHOk(float), drawQk(float);
void drawR(float, float, float), drawX(float, float, float),
  drawG(float, float, float), drawB(float, float, float),
  drawRHO(float, float, float), drawLINE(float, float, float, float);
void drawSP(float, float), drawchart(void);

/* Non dovrebbero essere qui (?) */
void showtextstat(void), showcircdesc(void), exptlpln(char, char*);
