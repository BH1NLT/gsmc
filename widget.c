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


#include <stdio.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include "calc.h"
#include "main.h"
#include "dataconv.h"
#include "widget.h"
#include "draw.h"
#include "autotune.h"
#include "print.h"

extern GtkTextBuffer *txtcircbuff;
extern int Zcirc, Ycirc, RHOcirc, Qcirc, tsidx, fsidx;
extern GtkWidget *scarea, *toptable, *menubar, *topwindow;
extern SMCDATA smcdata;
extern float tunestep, freqstep;
extern const float tunesteptab[6], freqsteptab[7];
extern gint scdim;
extern GtkWidget *statarr[20];
extern const char statlabelname[20][15];

GtkItemFactory *item_factory;
GtkWidget *filew, *about;
GtkWidget *evb, *etb1, *etb2, *etb3;
gchar *entry_text, entry_text_type;
GtkWidget *entryval;

static GtkItemFactoryEntry menu_items[] = {
  { "/_File",         NULL,              NULL, 0, "<Branch>", NULL },
  { "/File/New",   "<control>N",      restart, 0, NULL, NULL },
  { "/File/Write spice",   "<control>W",      write_activate, 0, NULL, NULL },
  { "/File/Postscript",   "<control>P",      print_activate, 0, NULL, NULL },
  { "/File/sep1",    NULL,              NULL, 0, "<Separator>", NULL },
  { "/File/Load",   "<control>L",      load_activate, 0, NULL, NULL },
  { "/File/Save",   "<control>S",      save_activate, 0, NULL, NULL },
  { "/File/sep1",    NULL,              NULL, 0, "<Separator>", NULL },
  { "/File/Quit",    "<control>Q",      main_quit, 0, NULL, NULL },

  { "/_Set",         NULL,              NULL, 0, "<Branch>", NULL },
  { "/Set/Start Point","S",             sp_activate, 0, NULL, NULL },
  { "/Set/Frequency","F",               f0_activate, 0, NULL, NULL },
  { "/Set/Characteristic Impedance","<control>0",z0_activate, 0,
    NULL, NULL },

  { "/_Add",     NULL,              NULL, 0, "<Branch>", NULL },
  { "/Add/[add] Resistance",       "R", add_resistance_activate, 0,
    NULL, NULL },
  { "/Add/[add] Inductance",       "L", add_inductance_activate, 0,
    NULL, NULL },
  { "/Add/[add] Capacitance",      "C", add_capacitance_activate, 0,
    NULL, NULL },
  { "/Add/[add] Transmission Line","T", add_tline_activate, 0, NULL, NULL },
  { "/Add/sep2",    NULL,              NULL, 0, "<Separator>", NULL },
  { "/Add/[ins] Resistance",       "<shift>R",
    ins_resistance_activate, 0, NULL, NULL },
  { "/Add/[ins] Inductance",       "<shift>L",
    ins_inductance_activate, 0, NULL, NULL },
  { "/Add/[ins] Capacitance",      "<shift>C",
    ins_capacitance_activate, 0, NULL, NULL },
  { "/Add/[ins] Transmission Line","<shift>T",
    ins_tline_activate, 0, NULL, NULL },

  { "/_Edit",     NULL,              NULL, 0, "<Branch>", NULL},
  { "/Edit/Delete component",       "D", delete_component_activate, 0,
    NULL, NULL },
  { "/Edit/sep3",    NULL,              NULL, 0, "<Separator>" , NULL},
  { "/Edit/Decrease first value",    "<shift>Left",
    decrease_first_activate, 0, NULL, NULL },
  { "/Edit/Increase first value",    "<shift>Right",
    increase_first_activate, 0, NULL, NULL },
  { "/Edit/Decrease second value",    "<alt>Left",
    decrease_second_activate, 0, NULL, NULL },
  { "/Edit/Increase second value",    "<alt>Right",
    increase_second_activate, 0, NULL, NULL },
  { "/Edit/Select previous element",    "<shift>Up",
    select_previous, 0, NULL, NULL },
  { "/Edit/Select next element",    "<shift>Down",
    select_next, 0, NULL , NULL},

  { "/Edit/Increase tuning step",    "<alt>Up",
    increase_tunestep, 0, NULL, NULL },
  { "/Edit/Decrease tuning step",    "<alt>Down",
    decrease_tunestep, 0, NULL, NULL },
  { "/Edit/Increase freq step",    "<shift><alt>Up",
    increase_freqstep, 0, NULL, NULL },
  { "/Edit/Decrease freq step",    "<shift><alt>Down",
    decrease_freqstep, 0, NULL, NULL },

  { "/Edit/Set first value",    "equal",
    set_first_val, 0, NULL, NULL },
  { "/Edit/Set second value",    "<alt>equal",
    set_second_val, 0, NULL, NULL },

  { "/Edit/Increase frequency", "<control>f",
    increase_freq, 0, NULL, NULL },
  { "/Edit/Decrease frequency", "<control><shift>f", 
    decrease_freq, 0, NULL, NULL },

  { "/_View", NULL, NULL, 0, "<Branch>", NULL},
  { "/View/Impedance", "<control>1", on_imped_activate, 0, 
    "<CheckItem>", NULL},
  { "/View/Admittance", "<control>2", on_admit_activate, 0, 
    "<CheckItem>", NULL},
  { "/View/Coefficient of Reflection",
    "<control>3", on_cor_activate, 0, "<CheckItem>", NULL},
  { "/View/Quality Factor (Q)", "<control>4", on_qfac_activate, 0,
    "<CheckItem>", NULL},

  { "/_Tools", NULL, NULL, 0, "<Branch>", NULL},
  { "/Tools/Autotune",       "<control>A", do_autotune, 0, NULL, NULL },
  { "/Tools/Lock for autotune",  "<control>X", lock_autotune_activate,
    0, NULL, NULL },

  { "/_Help",        NULL,              NULL, 0, "<LastBranch>", NULL },
  { "/Help/Keystrokes",  NULL,  help_keystrokes, 0, NULL, NULL },
  { "/Help/About",  NULL,  help_about, 0, NULL, NULL},
};

void help_about(GtkWidget *widget __attribute__((unused)),
		gpointer user_data __attribute__((unused))) {
  GtkWidget *label, *button;

#define ABTBUFFLEN 160
  char abtbuff[ABTBUFFLEN+1];
  
  snprintf(abtbuff, ABTBUFFLEN,
	   "\n GSMC - A Smith Chart Calculator\n"\
	   "v%s - %s\n\n\n"\
	   "(c) 2003-2016 by Lapo Pieri\n"\
	   "ik5nax@radioteknos.it\n\n",
	   __VERSION, __VERSION_DATE);

  about=gtk_dialog_new();
  label=gtk_label_new(abtbuff);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(about)->vbox), label, TRUE, TRUE, 0);
  button=gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(about)->action_area), 
		     button, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     (GtkSignalFunc)help_about_destroy, NULL);

  gtk_widget_show(button); gtk_widget_show(label); gtk_widget_show(about);
}

void help_about_destroy(GtkWidget *widget __attribute__((unused)),
			gpointer user_data __attribute__((unused))) {
  gtk_widget_destroy(about);
}

void help_keystrokes(GtkWidget *widget __attribute__((unused)),
		     gpointer user_data __attribute__((unused))) {
  GtkWidget *label, *button, *label2;

  about=gtk_dialog_new();
  label=gtk_label_new("GSMC Keystrokes");
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(about)->vbox), label, TRUE, TRUE, 0);

  label2=gtk_label_new("\
Ctrl+q\t\t Quit the program\n\
Ctrl+1\t\t Toggle visualization of impedance (Z) circle\n\
Ctrl+2\t\t Toggle visualization of admittance (Y) circle\n\
Ctrl+3\t\t Toggle visualization of reflection coefficient (RHO) circle\n\
Ctrl+4\t\t Toggle visualization of constant Q circle\n\
s\t\t\t Set start point\n\
Ctrl+0\t\t (zero) Set characteristic impedance z0\n\
f\t\t\t Set frequency (f0)\n\
r\t\t\t Add a resistor\n\
l\t\t\t Add an inductor\n\
c\t\t\t Add a capacitor\n\
t\t\t\t Add a transmission line or stub\n\
Shift+r\t\t Insert a resistor before the highlighted element\n\
Shift+l\t\t Insert an inductor before the highlighted element\n\
Shift+c\t\t Insert a capacitor before the highlighted element\n\
Shift+t\t\t Insert a transmission line or stub before the highlighted element\n\
d\t\t\t Delete the currently highligthed element\n\
Shift+Curs\t Up Highlight previous element\n\
Shift+Curs\t Down Highlight next element\n\
Shift+Curs\t Left Decrease the first field of currently highlighted element\n\
Shift+Curs\t Rigth Increase the first field of currently highlighted element\n\
Mod+Curs\t Left Decrease the second field of currently highlighted element\n\
Mod+Curs\t Rigth Increase the second field of currently highlighted element\n\
Mod+Curs\t Up Increase the tuning step\n\
Mod+Curs\t Down Decrease the tuning step\n\
Mod+Shift+Curs\t Up Increase the freq step\n\
Mod+Shift+Curs\t Down Decrease the freq step\n\
Ctrl+w\t\t Write network file in spice format\n\
Ctrl+p\t\t Write postscript file with Smith chart image\n\
Ctrl+n\t\t Restart for a new calculation with a clean chart\n\
=\t\t\t Change first field of currently highlighted element\n\
Mod+=\t\t Change second field of currently highlighted element\n\
Ctrl+a\t\t Autotune\n\
Ctrl+x\t\t Lock current element for autotuning\n\
");
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(about)->vbox), label2, TRUE, TRUE, 0);


  button=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(about)->action_area), 
		     button, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     (GtkSignalFunc)help_about_destroy, NULL);

  gtk_widget_show(button); gtk_widget_show(label); 
  gtk_widget_show(label2); gtk_widget_show(about);
}

void help_keystrokes_destroy(GtkWidget *widget __attribute__((unused)),
			     gpointer user_data __attribute__((unused))) {
  gtk_widget_destroy(about);
}

void main_quit(GtkWidget *widget __attribute__((unused)),
	       gpointer user_data __attribute__((unused))) {
  gtk_main_quit();
}

void on_imped_activate(GtkWidget *widget __attribute__((unused)),
		       gpointer user_data __attribute__((unused))) {
  Zcirc^=1;
  drawchart();
  gtk_widget_draw(scarea, NULL);
}

void on_admit_activate(GtkWidget *widget __attribute__((unused)),
		       gpointer user_data __attribute__((unused))) {
  Ycirc^=1;
  drawchart();
  gtk_widget_draw(scarea, NULL);
}

void on_cor_activate(GtkWidget *widget __attribute__((unused)),
		     gpointer user_data __attribute__((unused))) {
  RHOcirc^=1;
  drawchart();
  gtk_widget_draw(scarea, NULL);
}

void on_qfac_activate(GtkWidget *widget __attribute__((unused)),
		      gpointer user_data __attribute__((unused))) {
  Qcirc^=1;
  drawchart();
  gtk_widget_draw(scarea, NULL);
}

void  sp_activate(GtkWidget *widget __attribute__((unused)),
		  gpointer user_data __attribute__((unused))) {
  GtkWidget *lb, *hbb, *b1, *b2, *b3, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new(); 
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Start point");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);


  /* Button for various type of entry format */
  hbb=gtk_hbox_new(FALSE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "_Z");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), "_Y");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  b3=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b2)), "_RHO");
  gtk_box_pack_start(GTK_BOX(hbb), b3, TRUE, FALSE, FALSE);
  gtk_widget_show(b3);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(hbb);
  switch(smcdata.SPtype){ /* Start with previous button toggled */
  case SPTYPE_Z: gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b1),
					     TRUE); break;
  case SPTYPE_Y: gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b2),
					     TRUE); break;
  case SPTYPE_RHO: gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b3),
					       TRUE); break;
  }

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_spZ), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_spY), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b3), "toggled", 
		     GTK_SIGNAL_FUNC(set_spRHO), GTK_OBJECT(evb));

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTSP;
}

void f0_activate(GtkWidget *widget __attribute__((unused)),
		 gpointer user_data __attribute__((unused))) {
  GtkWidget *lb, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Center frequency");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTF0;
}

void  z0_activate(GtkWidget *widget __attribute__((unused)),
		  gpointer user_data __attribute__((unused))) {
  GtkWidget *lb, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Caracteristic Impedance");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTZ0;
}


void add_resistance_activate(GtkWidget *widget __attribute__((unused)),
			     gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *hbb, *b1, *b2, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Add Resistance");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  hbb=gtk_hbox_new(FALSE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "_Series");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), "_Parallel");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(hbb);


  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_parallel), GTK_OBJECT(evb));

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);


  gtk_widget_grab_focus(etb);

  /* to be revised (???) */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b1), TRUE);
  set_series();

  entry_text_type=ETTAR;
}

void add_inductance_activate(GtkWidget *widget __attribute__((unused)),
			     gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *hbb, *b1, *b2, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Add Inductance");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  hbb=gtk_hbox_new(FALSE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "_Series");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), "_Parallel");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(hbb);

  /* Rivedere */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b1), TRUE);
  set_series();

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_parallel), GTK_OBJECT(evb));

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTAL;
}

void add_capacitance_activate(GtkWidget *widget __attribute__((unused)),
			      gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *hbb, *b1, *b2, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Add Capacitance");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  hbb=gtk_hbox_new(FALSE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "_Series");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), "_Parallel");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(hbb);

  /* Rivedere */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b1), TRUE);
  set_series();

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_parallel), GTK_OBJECT(evb));

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTAC;
}

void add_tline_activate(GtkWidget *widget __attribute__((unused)),
			gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *vbb, *hbb, *b1, *b2, *b3, *b4, *b5, *bok, *label;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Add Transmission line");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  vbb=gtk_vbox_new(TRUE, 0);
  hbb=gtk_hbox_new(TRUE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "Open series stub (_a)");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), 
				     "Short series stub (_l)");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(vbb), hbb, TRUE, FALSE, FALSE);
  gtk_widget_show(hbb); hbb=gtk_hbox_new(TRUE, 0);
  b3=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b2)), 
				     "Open parallel stub (_o)");
  gtk_box_pack_start(GTK_BOX(hbb), b3, TRUE, FALSE, FALSE);
  gtk_widget_show(b3);
  b4=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b3)), 
				     "Short parallel stub (_c)");
  gtk_box_pack_start(GTK_BOX(hbb), b4, TRUE, FALSE, FALSE);
  gtk_widget_show(b4);
  gtk_box_pack_start(GTK_BOX(vbb), hbb, TRUE, FALSE, FALSE);
  gtk_widget_show(hbb); hbb=gtk_hbox_new(TRUE, 0);
  b5=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b4)), 
				     "Transmissive (_t)");
  gtk_box_pack_start(GTK_BOX(hbb), b5, TRUE, FALSE, FALSE);
  gtk_widget_show(b5);
  bok=gtk_button_new_with_label("OK"); gtk_widget_show(bok);
  gtk_signal_connect(GTK_OBJECT(bok), "clicked", 
		     GTK_SIGNAL_FUNC(etbt_callback), GTK_OBJECT(bok));
  gtk_box_pack_start(GTK_BOX(hbb), bok, TRUE, FALSE, FALSE);
  gtk_widget_show(hbb);
  gtk_box_pack_start(GTK_BOX(vbb), hbb, TRUE, FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(evb), vbb, FALSE, FALSE, FALSE);
  gtk_widget_show(vbb);

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_open_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_short_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b3), "toggled", 
		     GTK_SIGNAL_FUNC(set_open_parallel), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b4), "toggled", 
		     GTK_SIGNAL_FUNC(set_short_parallel), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b5), "toggled", 
		     GTK_SIGNAL_FUNC(set_transmissive), GTK_OBJECT(evb));

  /* Rivedere */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b5), TRUE);
  set_transmissive();

  /* entry box */
  hbb=gtk_hbox_new(FALSE, 0);
  etb1=gtk_entry_new_with_max_length(16);
  gtk_entry_set_editable(GTK_ENTRY(etb1), TRUE);
  label=gtk_label_new("    Len [deg] ");
  gtk_box_pack_start(GTK_BOX(hbb), label, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(hbb), etb1, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(label); gtk_widget_show(etb1); gtk_widget_show(hbb);

  hbb=gtk_hbox_new(FALSE, 0);
  etb2=gtk_entry_new_with_max_length(16);
  gtk_entry_set_editable(GTK_ENTRY(etb2), TRUE);
  label=gtk_label_new("    Z [ohm]  ");
  gtk_box_pack_start(GTK_BOX(hbb), label, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(hbb), etb2, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(label); gtk_widget_show(etb2); gtk_widget_show(hbb);

  /*
  hbb=gtk_hbox_new(FALSE, 0);
  etb3=gtk_entry_new_with_max_length(16);
  gtk_entry_set_editable(GTK_ENTRY(etb3), TRUE);
  label=gtk_label_new("    Att [dB/m] ");
  gtk_box_pack_start(GTK_BOX(hbb), label, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(hbb), etb3, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(label); gtk_widget_show(etb3); gtk_widget_show(hbb);
  */
  
  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb1);

  entry_text_type=ETTAT;
}

void ins_resistance_activate(GtkWidget *widget __attribute__((unused)),
			     gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *hbb, *b1, *b2, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Ins Resistance");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  hbb=gtk_hbox_new(FALSE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "_Series");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), "_Parallel");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(hbb);

  /* Rivedere */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b1), TRUE);
  set_series();

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_parallel), GTK_OBJECT(evb));

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTIR;
}

void ins_inductance_activate(GtkWidget *widget __attribute__((unused)),
			     gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *hbb, *b1, *b2, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Ins Inductance");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  hbb=gtk_hbox_new(FALSE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "_Series");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), "_Parallel");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(hbb);

  /* Rivedere */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b1), TRUE);
  set_series();

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_parallel), GTK_OBJECT(evb));

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTIL;
}

void ins_capacitance_activate(GtkWidget *widget __attribute__((unused)),
			      gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *hbb, *b1, *b2, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Add Capacitance");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  hbb=gtk_hbox_new(FALSE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "_Series");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), "_Parallel");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(hbb);

  /* Rivedere */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b1), TRUE);
  set_series();

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_parallel), GTK_OBJECT(evb));

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etb_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb);

  entry_text_type=ETTIC;
}


void ins_tline_activate(GtkWidget *widget __attribute__((unused)),
			gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *vbb, *hbb, *b1, *b2, *b3, *b4, *b5, *bok, *label;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Ins Transmission line");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* Button for various type of entry format */
  vbb=gtk_vbox_new(TRUE, 0);
  hbb=gtk_hbox_new(TRUE, 0);
  b1=gtk_radio_button_new_with_mnemonic(NULL, "Open series stub (_a)");
  gtk_box_pack_start(GTK_BOX(hbb), b1, TRUE, FALSE, FALSE);
  gtk_widget_show(b1);
  b2=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b1)), 
				     "Short series stub (_l)");
  gtk_box_pack_start(GTK_BOX(hbb), b2, TRUE, FALSE, FALSE);
  gtk_widget_show(b2);
  gtk_box_pack_start(GTK_BOX(vbb), hbb, TRUE, FALSE, FALSE);
  gtk_widget_show(hbb); hbb=gtk_hbox_new(TRUE, 0);
  b3=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b2)), 
				     "Open parallel stub (_o)");
  gtk_box_pack_start(GTK_BOX(hbb), b3, TRUE, FALSE, FALSE);
  gtk_widget_show(b3);
  b4=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b3)), 
				     "Short parallel stub (_c)");
  gtk_box_pack_start(GTK_BOX(hbb), b4, TRUE, FALSE, FALSE);
  gtk_widget_show(b4);
  gtk_box_pack_start(GTK_BOX(vbb), hbb, TRUE, FALSE, FALSE);
  gtk_widget_show(hbb); hbb=gtk_hbox_new(TRUE, 0);
  b5=gtk_radio_button_new_with_mnemonic(gtk_radio_button_group(
				     GTK_RADIO_BUTTON(b4)), 
				     "Transmissive (_t)");
  gtk_box_pack_start(GTK_BOX(hbb), b5, TRUE, FALSE, FALSE);
  gtk_widget_show(b5);
  bok=gtk_button_new_with_label("OK"); gtk_widget_show(bok);
  gtk_signal_connect(GTK_OBJECT(bok), "clicked", 
		     GTK_SIGNAL_FUNC(etbt_callback), GTK_OBJECT(bok));
  gtk_box_pack_start(GTK_BOX(hbb), bok, TRUE, FALSE, FALSE);
  gtk_widget_show(hbb);
  gtk_box_pack_start(GTK_BOX(vbb), hbb, TRUE, FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(evb), vbb, FALSE, FALSE, FALSE);
  gtk_widget_show(vbb);

  /* connecting radio button the signal */
  gtk_signal_connect(GTK_OBJECT(b1), "toggled", 
		     GTK_SIGNAL_FUNC(set_open_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b2), "toggled", 
		     GTK_SIGNAL_FUNC(set_short_series), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b3), "toggled", 
		     GTK_SIGNAL_FUNC(set_open_parallel), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b4), "toggled", 
		     GTK_SIGNAL_FUNC(set_short_parallel), GTK_OBJECT(evb));
  gtk_signal_connect(GTK_OBJECT(b5), "toggled", 
		     GTK_SIGNAL_FUNC(set_transmissive), GTK_OBJECT(evb));

  /* Rivedere */
  gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b5), TRUE);
  set_transmissive();

  /* entry box */
  hbb=gtk_hbox_new(FALSE, 0);
  etb1=gtk_entry_new_with_max_length(16);
  gtk_entry_set_editable(GTK_ENTRY(etb1), TRUE);
  label=gtk_label_new("    Len [deg] ");
  gtk_box_pack_start(GTK_BOX(hbb), label, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(hbb), etb1, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(label); gtk_widget_show(etb1); gtk_widget_show(hbb);

  hbb=gtk_hbox_new(FALSE, 0);
  etb2=gtk_entry_new_with_max_length(16);
  gtk_entry_set_editable(GTK_ENTRY(etb2), TRUE);
  label=gtk_label_new("    Z [ohm]  ");
  gtk_box_pack_start(GTK_BOX(hbb), label, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(hbb), etb2, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(label); gtk_widget_show(etb2); gtk_widget_show(hbb);

  /*
  hbb=gtk_hbox_new(FALSE, 0);
  etb3=gtk_entry_new_with_max_length(16);
  gtk_entry_set_editable(GTK_ENTRY(etb3), TRUE);
  label=gtk_label_new("    Att [dB/m] ");
  gtk_box_pack_start(GTK_BOX(hbb), label, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(hbb), etb3, FALSE, FALSE, FALSE);
  gtk_box_pack_start(GTK_BOX(evb), hbb, FALSE, FALSE, FALSE);
  gtk_widget_show(label); gtk_widget_show(etb3); gtk_widget_show(hbb);
  */
  
  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);

  gtk_widget_grab_focus(etb1);

  entry_text_type=ETTIT;
}


gint etb_callback(GtkWidget *widget __attribute__((unused)),
		  GtkWidget *entry) {
  float re, im, val0, val1, val2;
  char c, temp;
  int i;

  entry_text=(gchar*)gtk_entry_get_text(GTK_ENTRY(entry));
  if(strlen(entry_text)!=0){
    switch(entry_text_type) {
    case ETTSP:
      entrystringdecode(entry_text, &smcdata.reentrySP, &smcdata.imentrySP);
      break;
    case ETTZ0:
      if(entrystringdecode(entry_text, &re, &im)==0) {
	if(re<=0.)  
	  fprintf(stderr, "Invalid characteristic impedance value\n");
	else
	  smcdata.z0=re;
      }
      else
	fprintf(stderr, "Invalid characteristic impedance\n");
      break;
    case ETTF0:
      smcdata.f0=psdec(entry_text);
      break;
    case ETTAR:
      c='r';
      if(smcdata.ne<NEL) {
	smcdata.ne++; smcdata.ELtype[smcdata.ne]=c;
	if(componentvaluestringdecode(entry_text, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval0[smcdata.ne]=val0;
	  smcdata.ELval1[smcdata.ne]=val1; smcdata.ELval2[smcdata.ne]=val2;
	}
	else
	  smcdata.ne--;
      }
      else
	fprintf(stderr, "No more space left for component\n");
      smcdata.neidx=smcdata.ne;
      break;
      
    case ETTAL:
      c='l';
      if(smcdata.ne<NEL) {
	smcdata.ne++; smcdata.ELtype[smcdata.ne]=c;
	if(componentvaluestringdecode(entry_text, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval0[smcdata.ne]=val0;
	  smcdata.ELval1[smcdata.ne]=val1; smcdata.ELval2[smcdata.ne]=val2;
	}
	else
	  smcdata.ne--;
      }
      else
	fprintf(stderr, "No more space left for component\n");
      smcdata.neidx=smcdata.ne;
      break;
      
    case ETTAC:
      c='c';
      if(smcdata.ne<NEL) {
	smcdata.ne++; smcdata.ELtype[smcdata.ne]=c;
	if(componentvaluestringdecode(entry_text, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval0[smcdata.ne]=val0;
	  smcdata.ELval1[smcdata.ne]=val1; smcdata.ELval2[smcdata.ne]=val2;
	}
	else
	  smcdata.ne--;
      }
      else
	fprintf(stderr, "No more space left for component\n");
      smcdata.neidx=smcdata.ne;
      break;
    case ETTIR:
      /* Insert a component in the location specified by index */
      if(smcdata.ne<NEL) {
	/* make place for new network element */
	for(i=smcdata.ne; i>=smcdata.neidx; i--) {
	  smcdata.ELtype[i+1]=smcdata.ELtype[i];
	  smcdata.ELval0[i+1]=smcdata.ELval0[i];
	  smcdata.ELval1[i+1]=smcdata.ELval1[i];
	  smcdata.ELval2[i+1]=smcdata.ELval2[i];
	  temp=smcdata.ELplace[i+1];
	  smcdata.ELplace[i+1]=smcdata.ELplace[i];
	  smcdata.ELplace[i]=temp;
	  smcdata.rerhoIP[i+1]=smcdata.rerhoIP[i];
	  smcdata.imrhoIP[i+1]=smcdata.imrhoIP[i];
	}
	smcdata.ne++;
	
	/* and then add the network element in the place just freed */
	c='r';
	smcdata.ELtype[smcdata.neidx]=c;
	if(componentvaluestringdecode(entry_text, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval0[smcdata.neidx]=val0;
	  smcdata.ELval1[smcdata.neidx]=val1;
	  smcdata.ELval2[smcdata.neidx]=val2;
	}
	else 
	  removene();      
      }
      else
	fprintf(stderr, "No more space left for component\n");
      recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
      break;
      
    case ETTIL:
      /* Insert a component in the location specified by index */
      if(smcdata.ne<NEL) {
	/* make place for new network element */
	for(i=smcdata.ne; i>=smcdata.neidx; i--) {
	  smcdata.ELtype[i+1]=smcdata.ELtype[i];
	  smcdata.ELval0[i+1]=smcdata.ELval0[i];
	  smcdata.ELval1[i+1]=smcdata.ELval1[i];
	  smcdata.ELval2[i+1]=smcdata.ELval2[i];
	  temp=smcdata.ELplace[i+1];
	  smcdata.ELplace[i+1]=smcdata.ELplace[i];
	  smcdata.ELplace[i]=temp;
	  smcdata.rerhoIP[i+1]=smcdata.rerhoIP[i];
	  smcdata.imrhoIP[i+1]=smcdata.imrhoIP[i];
	}
	smcdata.ne++;
	
	/* and then add the network element in the place just freed */
	c='l';
	smcdata.ELtype[smcdata.neidx]=c;
	if(componentvaluestringdecode(entry_text, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval0[smcdata.neidx]=val0;
	  smcdata.ELval1[smcdata.neidx]=val1;
	  smcdata.ELval2[smcdata.neidx]=val2;
	}
	else 
	  removene();      
      }
      else
	fprintf(stderr, "No more space left for component\n");
      recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
      break;
    case ETTIC:
      /* Insert a component in the location specified by index */
      if(smcdata.ne<NEL) {
	/* make place for new network element */
	for(i=smcdata.ne; i>=smcdata.neidx; i--) {
	  smcdata.ELtype[i+1]=smcdata.ELtype[i];
	  smcdata.ELval0[i+1]=smcdata.ELval0[i];
	  smcdata.ELval1[i+1]=smcdata.ELval1[i];
	  smcdata.ELval2[i+1]=smcdata.ELval2[i];
	  temp=smcdata.ELplace[i+1];
	  smcdata.ELplace[i+1]=smcdata.ELplace[i];
	  smcdata.ELplace[i]=temp;
	  smcdata.rerhoIP[i+1]=smcdata.rerhoIP[i];
	  smcdata.imrhoIP[i+1]=smcdata.imrhoIP[i];
	}
	smcdata.ne++;
	
	/* and then add the network element in the place just freed */
	c='c';
	smcdata.ELtype[smcdata.neidx]=c;
	if(componentvaluestringdecode(entry_text, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval0[smcdata.neidx]=val0;
	  smcdata.ELval1[smcdata.neidx]=val1;
	  smcdata.ELval2[smcdata.neidx]=val2;
	}
	else 
	  removene();      
      }
      else
	fprintf(stderr, "No more space left for component\n");
      recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
      break;
      
    case ETTNULL:
    default: printf("%s\n", entry_text);
    }
    
    recalc(); showtextstat(); showcircdesc();
    entry_text_type=ETTNULL;
    drawchart(); gtk_widget_draw(scarea, NULL);
  }
  gtk_widget_destroy(entryval);
  return TRUE;
}

gint etbt_callback(GtkWidget *widget __attribute__((unused)),
		   GtkWidget *entry __attribute__((unused))) {
  gchar *entry_text1, *entry_text2;
  char c, temp;
  float val0, val1, val2;
  int i;

  entry_text1=(gchar*)gtk_entry_get_text(GTK_ENTRY(etb1));
  entry_text2=(gchar*)gtk_entry_get_text(GTK_ENTRY(etb2));
  /* entry_text3=(gchar*)gtk_entry_get_text(GTK_ENTRY(etb3)); */
  if((strlen(entry_text1)!=0) && (strlen(entry_text2)!=0)){

    switch(entry_text_type){
    case ETTAT:
      c='t';
      if(smcdata.ne<NEL) {
	smcdata.ne++; smcdata.ELtype[smcdata.ne]=c;
       if(componentvaluestringdecode(entry_text1, &val0, &val1, &val2, &c)==0){
	 smcdata.ELval0[smcdata.ne]=val0;
       }
       else
	 smcdata.ne--;
       if(componentvaluestringdecode(entry_text2, &val0, &val1, &val2, &c)==0){
	 smcdata.ELval1[smcdata.ne]=val0;
       }
       else
	 smcdata.ne--;
       /*
       if(componentvaluestringdecode(entry_text3, &val0, &val1, &val2, &c)==0){
       smcdata.ELval2[smcdata.ne]=val0;
       }
       else
       smcdata.ne--;
       */
      }
      else
	fprintf(stderr, "No more space left for component\n");
      smcdata.neidx=smcdata.ne;
      break;

    case ETTIT:
      /* Insert a component in the location specified by index */
      if(smcdata.ne<NEL) {
	/* make place for new network element */
	for(i=smcdata.ne; i>=smcdata.neidx; i--) {
	  smcdata.ELtype[i+1]=smcdata.ELtype[i];
	  smcdata.ELval0[i+1]=smcdata.ELval0[i];
	  smcdata.ELval1[i+1]=smcdata.ELval1[i];
	  smcdata.ELval2[i+1]=smcdata.ELval2[i];
	  temp=smcdata.ELplace[i+1];
	  smcdata.ELplace[i+1]=smcdata.ELplace[i];
	  smcdata.ELplace[i]=temp;
	  smcdata.rerhoIP[i+1]=smcdata.rerhoIP[i];
	  smcdata.imrhoIP[i+1]=smcdata.imrhoIP[i];
	}
	smcdata.ne++;

	/* and then add the network element in the place just freed */
	c='t';
        smcdata.ELtype[smcdata.neidx]=c;
       if(componentvaluestringdecode(entry_text1, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval0[smcdata.neidx]=val0;
	}
	else
	  removene();
       if(componentvaluestringdecode(entry_text2, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval1[smcdata.neidx]=val0;
	}
	else
	  removene();
	/*
       if(componentvaluestringdecode(entry_text3, &val0, &val1, &val2, &c)==0){
	  smcdata.ELval2[smcdata.neidx]=val0;
	  }
	  else
	  removene();
	*/
	
      }
      else
	fprintf(stderr, "No more space left for component\n");
      recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
      break;
      
    case ETTNULL:
    default: printf("%s\t%s\n", entry_text1, entry_text2);
    }
    
    recalc(); showtextstat(); showcircdesc();
    entry_text_type=ETTNULL;
    drawchart(); gtk_widget_draw(scarea, NULL);
  }
  gtk_widget_destroy(entryval);
  return TRUE;
}

gint set_spZ(void) {
  smcdata.SPtype=SPTYPE_Z;
  return TRUE;
}

gint set_spY(void) {
  smcdata.SPtype=SPTYPE_Y;
  return TRUE;
}

gint set_spRHO(void) {
  smcdata.SPtype=SPTYPE_RHO;
  return TRUE;
}

gint set_series(void) {
  smcdata.ELplace[smcdata.ne+1]='s';
  return TRUE;
}

gint set_parallel(void) {
  smcdata.ELplace[smcdata.ne+1]='p';
  return TRUE;
}

gint set_open_series(void) {
  smcdata.ELplace[smcdata.ne+1]='a';
  return TRUE;
}

gint set_short_series(void) {
  smcdata.ELplace[smcdata.ne+1]='l';
  return TRUE;
}

gint set_open_parallel(void) {
  smcdata.ELplace[smcdata.ne+1]='o';
  return TRUE;
}

gint set_short_parallel(void) {
  smcdata.ELplace[smcdata.ne+1]='c';
  return TRUE;
}

gint set_transmissive(void) {
  smcdata.ELplace[smcdata.ne+1]='t';
  return TRUE;
}


void menu_create(GtkWidget *window, GtkWidget **menu_bar) {
  GtkAccelGroup *accel_group;

  gint nmenu_items=sizeof(menu_items)/sizeof(menu_items[0]);

  accel_group=gtk_accel_group_new();
  item_factory=gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", 
				    accel_group);
  gtk_item_factory_create_items(item_factory, nmenu_items, menu_items, NULL);
  gtk_window_add_accel_group(GTK_WINDOW (window), accel_group);

  if(menu_bar)
    *menu_bar=gtk_item_factory_get_widget(item_factory, "<main>");
}

void delete_component_activate(GtkWidget *widget __attribute__((unused)),
			       gpointer user_data __attribute__((unused))){
  removene(); recalc(); 
  drawchart(); gtk_widget_draw(scarea, NULL);
  showtextstat(); showcircdesc();
}

void select_previous(GtkWidget *widget __attribute__((unused)),
		     gpointer user_data __attribute__((unused))){
  if(smcdata.neidx>1) {
    smcdata.neidx--; showcircdesc(); }
}

void select_next(GtkWidget *widget __attribute__((unused)),
		 gpointer user_data __attribute__((unused))){
  if(smcdata.neidx<smcdata.ne) { 
    smcdata.neidx++; showcircdesc(); }
}

void decrease_tunestep(GtkWidget *widget __attribute__((unused)),
		       gpointer user_data __attribute__((unused))){
  if(tsidx>0)
    tsidx--;
  tunestep=tunesteptab[tsidx];
  showtextstat();
}

void increase_tunestep(GtkWidget *widget __attribute__((unused)),
		       gpointer user_data __attribute__((unused))){
  if(tsidx<5)
    tsidx++;
  tunestep=tunesteptab[tsidx]; 
  showtextstat();
}

void decrease_freqstep(GtkWidget *widget __attribute__((unused)),
		       gpointer user_data __attribute__((unused))){
  if(fsidx>0)
    fsidx--;
  freqstep=freqsteptab[fsidx];
  showtextstat();
}

void increase_freqstep(GtkWidget *widget __attribute__((unused)),
		       gpointer user_data __attribute__((unused))){
  if(fsidx<6)
    fsidx++;
  freqstep=freqsteptab[fsidx]; 
  showtextstat();
}

void decrease_first_activate(GtkWidget *widget __attribute__((unused)),
			     gpointer user_data __attribute__((unused))){
  smcdata.ELval0[smcdata.neidx]/=tunestep; 
  recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
  showtextstat(); showcircdesc();
}

void increase_first_activate(GtkWidget *widget __attribute__((unused)),
			     gpointer user_data __attribute__((unused))){
  smcdata.ELval0[smcdata.neidx]*=tunestep;
  recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
  showtextstat(); showcircdesc();
}

void decrease_second_activate(GtkWidget *widget __attribute__((unused)),
			      gpointer user_data __attribute__((unused))){
  smcdata.ELval1[smcdata.neidx]/=tunestep; 
  recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
  showtextstat(); showcircdesc();
}

void increase_second_activate(GtkWidget *widget __attribute__((unused)),
			      gpointer user_data __attribute__((unused))){
  smcdata.ELval1[smcdata.neidx]*=tunestep;
  recalc(); drawchart(); gtk_widget_draw(scarea, NULL);
  showtextstat(); showcircdesc();
}


void write_activate(GtkWidget *widget __attribute__((unused)),
		    gpointer user_data __attribute__((unused))) {
  filew = gtk_file_selection_new ("Write SPICE file");
  gtk_signal_connect (GTK_OBJECT (filew), "destroy",
		      (GtkSignalFunc) gtk_widget_destroy,
		      GTK_OBJECT(filew));
  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		      "clicked", (GtkSignalFunc) wfile_ok_sel, filew );
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
					 (filew)->cancel_button),
			     "clicked", (GtkSignalFunc) gtk_widget_destroy,
			     GTK_OBJECT (filew));
  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
				   "untitled.cir");
  gtk_widget_show(filew);
}

void wfile_ok_sel(GtkWidget *w __attribute__((unused)), GtkFileSelection *fs ){
  
  writespice((char*)gtk_file_selection_get_filename(GTK_FILE_SELECTION (fs)));
  gtk_widget_hide(filew);
}

void load_activate(GtkWidget *widget __attribute__((unused)),
		    gpointer user_data __attribute__((unused))) {
  filew=gtk_file_selection_new("Load GSMC work");
  gtk_signal_connect (GTK_OBJECT (filew), "destroy",
		      (GtkSignalFunc) gtk_widget_destroy,
		      GTK_OBJECT(filew));
  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		      "clicked", (GtkSignalFunc) lfile_ok_sel, filew );
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
					 (filew)->cancel_button),
			     "clicked", (GtkSignalFunc) gtk_widget_destroy,
			     GTK_OBJECT (filew));
  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
				   "untitled.gdt");
  gtk_widget_show(filew);
}

void lfile_ok_sel(GtkWidget *w __attribute__((unused)), GtkFileSelection *fs ){
  
  loadgw((char*)gtk_file_selection_get_filename(GTK_FILE_SELECTION (fs)));
  gtk_widget_hide(filew);
}

void save_activate(GtkWidget *widget __attribute__((unused)),
		    gpointer user_data __attribute__((unused))) {
  filew=gtk_file_selection_new("Load GSMC work");
  gtk_signal_connect (GTK_OBJECT (filew), "destroy",
		      (GtkSignalFunc) gtk_widget_destroy,
		      GTK_OBJECT(filew));
  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		      "clicked", (GtkSignalFunc) sfile_ok_sel, filew );
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
					 (filew)->cancel_button),
			     "clicked", (GtkSignalFunc) gtk_widget_destroy,
			     GTK_OBJECT (filew));
  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
				   "untitled.gdt");
  gtk_widget_show(filew);
}

void sfile_ok_sel(GtkWidget *w __attribute__((unused)), GtkFileSelection *fs ){
  
  savegw((char*)gtk_file_selection_get_filename(GTK_FILE_SELECTION (fs)));
  gtk_widget_hide(filew);
}

void print_activate(GtkWidget *widget __attribute__((unused)),
		    gpointer user_data __attribute__((unused))) {
  filew = gtk_file_selection_new ("Write PS/EPS file");
  gtk_signal_connect (GTK_OBJECT (filew), "destroy",
		      (GtkSignalFunc) gtk_widget_destroy,
		      GTK_OBJECT(filew));
  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		      "clicked", (GtkSignalFunc) print_ok_sel, filew );
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
					 (filew)->cancel_button),
			     "clicked", (GtkSignalFunc) gtk_widget_destroy,
			     GTK_OBJECT (filew));
  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
				   "out.ps");
  gtk_widget_show(filew);
}

void print_ok_sel(GtkWidget *w __attribute__((unused)), GtkFileSelection *fs ){
  
  writePS((char*)gtk_file_selection_get_filename(GTK_FILE_SELECTION (fs)));
  gtk_widget_hide(filew);
}


void restart(GtkWidget *widget __attribute__((unused)),
	     gpointer user_data __attribute__((unused))) {

  smcdata.SPtype=SPTYPE_Z; smcdata.reentrySP=50.; smcdata.imentrySP=0.;
  smcdata.z0=50.; smcdata.f0=1e6; tsidx=3; tunestep=tunesteptab[tsidx];
  smcdata.ne=0; smcdata.neidx=0;
  recalc(); showtextstat(); drawchart(); showcircdesc();
  gtk_widget_draw(topwindow, NULL);  
}

gint cursor_motion_in_sc(GtkWidget *widget __attribute__((unused)),
			 GdkEventMotion *event){
  int x, y;
  GdkModifierType state;
  char b[80], b2[80];
  float r, j;

  if (event->is_hint)
    gdk_window_get_pointer (event->window, &x, &y, &state);
  else
    {
      x = event->x;
      y = event->y;
      state = event->state;
    }

  smcdata.rerhoCU=((float)x/(float)scdim-0.5)*2.;
  smcdata.imrhoCU=(0.5-(float)y/(float)scdim)*2.;

  /* More efficient rather than calling showtextstat() */
  rho2z(smcdata.rerhoCU, smcdata.imrhoCU, &r, &j);
  r*=smcdata.z0; j*=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelzCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelzCU]), b);

  rho2y(smcdata.rerhoCU, smcdata.imrhoCU, &r, &j);
  r/=smcdata.z0; j/=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);  
  strcpy(b2, b); sprintf(b, statlabelname[labelyCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelyCU]), b);

  r=smcdata.rerhoCU; j=smcdata.imrhoCU;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelrhoCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelrhoCU]), b);

  r=sqrt((smcdata.rerhoCU*smcdata.rerhoCU)+
	 (smcdata.imrhoCU*smcdata.imrhoCU));
  sprintf(b, "%5.2f", (1.+r)/(1.-r));
  strcpy(b2, b); sprintf(b, statlabelname[labelSWRCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelSWRCU]), b);


  return TRUE;
}

void set_first_val(GtkWidget *widget __attribute__((unused)),
		   gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Set new value");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etbn_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);


  gtk_widget_grab_focus(etb);

  entry_text_type=ETTN1;
}

void set_second_val(GtkWidget *widget __attribute__((unused)),
		    gpointer user_data __attribute__((unused))){
  GtkWidget *lb, *etb;

  /* New widget for value entry */
  entryval=gtk_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(entryval), GTK_WINDOW(topwindow));

  /* Entry vbox */
  evb=gtk_vbox_new(FALSE, 0);

  /* Label */
  lb=gtk_label_new("Set new value");
  gtk_box_pack_start(GTK_BOX(evb), lb, FALSE, FALSE, FALSE);
  gtk_widget_show(lb);

  /* entry box */
  etb=gtk_entry_new_with_max_length(20);
  gtk_entry_set_editable(GTK_ENTRY(etb), TRUE);
  gtk_signal_connect(GTK_OBJECT(etb), "activate",
		     GTK_SIGNAL_FUNC(etbn_callback), GTK_OBJECT(etb));
  gtk_box_pack_start(GTK_BOX(evb), etb, FALSE, FALSE, FALSE);
  gtk_widget_show(etb);

  /* realize in entryval separate widget */
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(entryval)->vbox),
		    evb);
  gtk_widget_show(entryval);
  gtk_widget_show(evb);


  gtk_widget_grab_focus(etb);

  entry_text_type=ETTN2;
}


gint etbn_callback(GtkWidget *widget __attribute__((unused)),
		   GtkWidget *entry) {
  float val1, val2;
  char p;

  entry_text=(gchar*)gtk_entry_get_text(GTK_ENTRY(entry));
  if(strlen(entry_text)!=0){
    switch(entry_text_type) {
    case ETTN1:
      componentvaluestringdecode(entry_text,
				 &smcdata.ELval0[smcdata.neidx], &val1,
				 &val2, &p);
      break;
    case ETTN2:
      componentvaluestringdecode(entry_text,
				 &smcdata.ELval1[smcdata.neidx], &val1,
				 &val2, &p);
      break;
    default: printf("%s\n", entry_text);
    }
    
    recalc(); showtextstat(); showcircdesc();
    entry_text_type=ETTNULL;
    drawchart(); gtk_widget_draw(scarea, NULL);
  }
  gtk_widget_destroy(entryval);

  return TRUE;
}

void do_autotune(GtkWidget *widget __attribute__((unused)),
		 gpointer user_data __attribute__((unused))) {

  autotune();

  showtextstat(); showcircdesc();
}


void lock_autotune_activate(GtkWidget *widget __attribute__((unused)),
			    gpointer user_data __attribute__((unused))) {

  smcdata.ELlocked[smcdata.neidx]^=1;
  showcircdesc();
}

void increase_freq(GtkWidget *widget __attribute__((unused)),
		   gpointer user_data __attribute__((unused))) {

  smcdata.f0+=pow(10., floor(log10(smcdata.f0)+0.5))*freqstep;
  recalc(); drawchart(); gtk_widget_draw(scarea, NULL); showtextstat();
}

void decrease_freq(GtkWidget *widget __attribute__((unused)),
		   gpointer user_data __attribute__((unused))) {

  smcdata.f0-=pow(10., floor(log10(smcdata.f0)+0.5))*freqstep;
  recalc(); drawchart(); gtk_widget_draw(scarea, NULL); showtextstat();
}
