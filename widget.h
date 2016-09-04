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


#define ETTNULL 0
#define ETTSP   1
#define ETTZ0   2
#define ETTF0   3
#define ETTAR   4
#define ETTAL   5
#define ETTAC   6
#define ETTAT   7
#define ETTIR   8
#define ETTIL   9
#define ETTIC   10
#define ETTIT   11
#define ETTN1   12
#define ETTN2   13

void main_quit(GtkWidget*, gpointer),
  help_about(GtkWidget*, gpointer),
  help_about_destroy(GtkWidget *, gpointer),
  help_keystrokes(GtkWidget*, gpointer),
  help_keystrokes_destroy(GtkWidget *, gpointer),
  write_activate(GtkWidget*, gpointer),
  load_activate(GtkWidget*, gpointer),
  save_activate(GtkWidget*, gpointer),
  print_activate(GtkWidget*, gpointer),
  restart(GtkWidget*, gpointer),
  sp_activate(GtkWidget*, gpointer), f0_activate(GtkWidget*, gpointer), 
  z0_activate(GtkWidget*, gpointer),
  add_resistance_activate(GtkWidget*, gpointer),
  add_inductance_activate(GtkWidget*, gpointer),
  add_capacitance_activate(GtkWidget*, gpointer),
  add_tline_activate(GtkWidget*, gpointer),
  ins_resistance_activate(GtkWidget*, gpointer),
  ins_inductance_activate(GtkWidget*, gpointer),
  ins_capacitance_activate(GtkWidget*, gpointer),
  ins_tline_activate(GtkWidget*, gpointer),
  delete_component_activate(GtkWidget*, gpointer),
  decrease_first_activate(GtkWidget*, gpointer),
  increase_first_activate(GtkWidget*, gpointer),
  decrease_second_activate(GtkWidget*, gpointer),
  increase_second_activate(GtkWidget*, gpointer),
  select_previous(GtkWidget*, gpointer),  
  select_next(GtkWidget*, gpointer),
  decrease_tunestep(GtkWidget*, gpointer),
  increase_tunestep(GtkWidget*, gpointer),
  decrease_freqstep(GtkWidget*, gpointer),
  increase_freqstep(GtkWidget*, gpointer),
  on_imped_activate(GtkWidget*, gpointer),
  on_admit_activate(GtkWidget*, gpointer),
  on_cor_activate(GtkWidget*, gpointer),
  on_qfac_activate(GtkWidget*, gpointer),
  wfile_ok_sel(GtkWidget *, GtkFileSelection *),
  lfile_ok_sel(GtkWidget *, GtkFileSelection *),
  sfile_ok_sel(GtkWidget *, GtkFileSelection *),
  print_ok_sel(GtkWidget *, GtkFileSelection *),
  set_first_val(GtkWidget*, gpointer),
  set_second_val(GtkWidget*, gpointer),
  do_autotune(GtkWidget*, gpointer),
  lock_autotune_activate(GtkWidget*, gpointer),
  increase_freq(GtkWidget*, gpointer),
  decrease_freq(GtkWidget*, gpointer);

gint cursor_motion_in_sc(GtkWidget *, GdkEventMotion *);

void menu_create(GtkWidget *, GtkWidget **);

gint set_spZ(void), set_spY(void), set_spRHO(void);
gint etb_callback(GtkWidget*, GtkWidget*), 
  etbt_callback(GtkWidget*, GtkWidget*),
  etbn_callback(GtkWidget*, GtkWidget*);
gint set_series(void), set_parallel(void), set_short_series(void),
  set_open_series(void), set_short_parallel(void), set_open_parallel(void),
  set_transmissive(void);

