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


void z2rho(float, float, float*, float*), y2rho(float, float, float*, float*),
  rho2z(float, float, float*, float*),  rho2y(float, float, float*, float*),
  rhoz1z2(float *, float *, float, float);
void rect2polar(float *, float *), polar2rect(float *, float *);
void recalc(void);

float thetaonR(float, float, float), thetaonX(float, float, float), 
  thetaonG(float, float, float), thetaonB(float, float, float);

void float2prefs(float, char *, int);
