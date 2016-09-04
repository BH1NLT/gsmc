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
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "main.h"
#include "widget.h"
#include "calc.h"
#include "draw.h"

const char statlabelname[21][15]={"Z0= %sohm", 
				"f0= %sHz",
				"",
				"zSP= %sohm",
				"ySP= %smho",
				"rhoSP= %s",
				"SWR= %s",
				"",
				"zEP= %sohm",
				"yEP= %smho",
				"rhoEP= %s",
				"SWR= %s",
				"",
				"Tunestep= %s%%",
				"Freqstep= %s%%",
				"",
				"zCU= %sohm",
				"yCU= %smho",
				"rhoCU= %s",
				"SWR= %s"
};

GtkWidget *topwindow, *toptable, *scarea, *menubar;
GtkTextBuffer *txtcircbuff, *txtstatbuff;
GtkWidget *textcircdesc, *textstat;
GtkWidget *statZ0, *statf0;
GtkWidget *statarr[21];

extern GtkItemFactoryEntry menu_items[];
extern GtkItemFactory *item_factory;

GdkPixmap *mainpixmap=NULL;
int scdim;

int Zcirc=0, Ycirc=0, RHOcirc=0, Qcirc=0, tsidx, i, fsidx;
const float tunesteptab[6]={1.01, 1.02, 1.05, 1.1, 1.2, 1.5};
const float freqsteptab[7]={.001, .002, .005, .01, .02, .05, .1};
float tunestep=1.1;
float freqstep=.01;

SMCDATA smcdata;

int main(int argc, char **argv) {
  GtkWidget *tdscw, *tcfr, *statfr, *align;
  int i, fdrc;
  char buff[40];

  /* GTK and top window init */
  gtk_init(&argc, &argv);
  if((fdrc=open("/root/.gsmc/gtkrc", O_RDONLY))!=-1){
    close(fdrc);
    gtk_rc_parse("/root/.gsmc/gtkrc");
  }
  topwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize(GTK_WIDGET(topwindow), 600, 600);
  gtk_widget_set_name(topwindow, "topwindow");
  gtk_window_set_title(GTK_WINDOW(topwindow), "GSMC - Smith Chart Calculator");
  gtk_signal_connect(GTK_OBJECT(topwindow), "delete_event",
		     (GtkSignalFunc)main_quit, NULL);


  /* Main table (toptable)*/
  /* 0                          1               2
    0+--------------------------+---------------+
     |               Menu                       |
    1+--------------------------+---------------+
     |                          |               |
     |                          |               |
     |                          |   "Status"    |
     |      Smith Chart         |               |
     |                          |               |
     |                          |               |
     |                          |               |
     |                          |               |
     |                          |               |
     |                          |               |
    2+--------------------------+---------------+
     |                          |               |
     |                          |    (spare)    |
     |    Circuit description   | maybe entry   |
     |                          | dialog???     |
     |                          |               |
     |                          |               |
    3+--------------------------+---------------+
 */
  toptable=gtk_table_new(3, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(topwindow), toptable);

  /* Menu creation */
  menu_create(topwindow, &menubar);
  gtk_table_attach(GTK_TABLE(toptable), menubar, 0, 3, 0, 1,
		   GTK_FILL, 0, 0, 1);
  gtk_widget_show(menubar);

  /* Smith Chart drawing area */
  scarea=gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA(scarea), 400, 400);
  gtk_signal_connect(GTK_OBJECT(scarea), "expose_event",
		     GTK_SIGNAL_FUNC(mdw_expose_event), NULL);
  gtk_signal_connect(GTK_OBJECT(scarea), "configure_event",
		     GTK_SIGNAL_FUNC(mdw_configure_event), NULL);

  gtk_table_attach(GTK_TABLE(toptable), scarea, 0, 1, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND, 0, 2);
  gtk_widget_show(scarea);

  /* Text Circuit Description */
  tcfr=gtk_frame_new("Circuit Description");
  tdscw=gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(tdscw),
				 GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_widget_set_usize(GTK_WIDGET(tdscw), 400, 140);

  txtcircbuff=gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(txtcircbuff, "", 0);
  textcircdesc=gtk_text_view_new_with_buffer(txtcircbuff);
  gtk_text_buffer_create_tag(txtcircbuff, "highlighted",
			     "background", "green", NULL);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textcircdesc), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textcircdesc), FALSE);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(tdscw),
					textcircdesc);
  gtk_container_add(GTK_CONTAINER(tcfr), tdscw);
  gtk_table_attach(GTK_TABLE(toptable), tcfr, 0, 1, 2, 3,
  		   GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show(textcircdesc);
  gtk_widget_show(tcfr); gtk_widget_show(tdscw);
  
  /* Text Status */
  statfr=gtk_frame_new("Current value");
  textstat=gtk_table_new(21, 1, TRUE);
  gtk_widget_set_usize(GTK_WIDGET(textstat), 200, 350);

  for(i=0; i<21; i++){
    sprintf(buff, statlabelname[i], "0.00");
    statarr[i]=gtk_label_new(buff);
    gtk_label_set_justify(GTK_LABEL(statarr[i]), GTK_JUSTIFY_LEFT);
    align=gtk_fixed_new(); gtk_fixed_put(GTK_FIXED(align), statarr[i], 5, 0);
    gtk_table_attach(GTK_TABLE(textstat), align, 0, 1, i, i+1,
		     GTK_FILL, 0, 0, 0);
    gtk_widget_show(statarr[i]);
    gtk_widget_show(align);
  }

  gtk_container_add(GTK_CONTAINER(statfr), textstat);
  gtk_table_attach(GTK_TABLE(toptable), statfr, 1, 2, 1, 2,
		   GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show(statfr); gtk_widget_show(textstat);


  /* Data init */
  smcdata.SPtype=SPTYPE_Z; smcdata.reentrySP=50.; smcdata.imentrySP=0.;
  smcdata.z0=50.; smcdata.f0=1e6; tsidx=3; fsidx=3; 
  tunestep=tunesteptab[tsidx]; tunestep=tunesteptab[fsidx];
  recalc(); showtextstat();
  for(i=0; i<=NEL; i++)
    smcdata.ELlocked[i]=0;
  smcdata.projectname[0]='\0';

  /* Signal for cursor position display */
  g_signal_connect(G_OBJECT(scarea), "motion_notify_event",
		   G_CALLBACK(cursor_motion_in_sc), NULL);

  gtk_widget_set_events(scarea,GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK);


  /* Startup */
  gtk_widget_show(toptable);
  gtk_widget_show(topwindow);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(item_factory, "/View/Impedance")), TRUE);
  gtk_main();

  return 0;
}


gint mdw_expose_event(GtkWidget *widget, GdkEventExpose *event,
		      gpointer user_data __attribute__((unused))){
  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		  mainpixmap,
		  event->area.x, event->area.y,
		  event->area.x, event->area.y,
		  event->area.width, event->area.height);
  
  return FALSE;
}

gint mdw_configure_event(GtkWidget *widget,
			 GdkEventExpose *event __attribute__((unused)),
			 gpointer user_data __attribute__((unused))){

  if(mainpixmap)
    gdk_pixmap_unref(mainpixmap);

  /* Smith Chart dimension retrieve */
  scdim=widget->allocation.width;
  if(widget->allocation.height<scdim)
    scdim=widget->allocation.height;

  mainpixmap=gdk_pixmap_new(widget->window, scdim+1, scdim+1, -1);
  gdk_draw_rectangle(mainpixmap,
		     widget->style->white_gc,
		     TRUE,
		     0, 0,
		     scdim, scdim);
  setup_gcs();
  drawchart();
  return TRUE;
}



/* Non va qui, ma non mi ricordo cosa faccia */
void removene(void) {
  int i;
  for(i=smcdata.neidx; i<smcdata.ne; i++) {
    smcdata.ELtype[i]=smcdata.ELtype[i+1];
    smcdata.ELval0[i]=smcdata.ELval0[i+1];
    smcdata.ELval1[i]=smcdata.ELval1[i+1];
    smcdata.ELval2[i]=smcdata.ELval2[i+1];
    smcdata.ELplace[i]=smcdata.ELplace[i+1];
    smcdata.rerhoIP[i]=smcdata.rerhoIP[i+1];
    smcdata.imrhoIP[i]=smcdata.imrhoIP[i+1];
  }
  smcdata.ne--; if(smcdata.neidx>smcdata.ne) smcdata.neidx--;
}


void writespice(char *b) {
  FILE *spf;
  int i, nr=0, nc=0, nl=0, nt=0, nod=0, nos=0, node=0;
  float vr, vi;
  char buff[40];

  if((spf=fopen(b, "w"))==NULL) {
    fprintf(stderr, "Unable to open file %s for writing\n", b);
    return;
  }

  fprintf(spf, "Network generated by GTK Smith Chart Calculator\n");
  fprintf(spf, "* Check/modify this file before simulation\n\n");

  /* Start impedance is saved as a series of a resistance and a reactance
     and as a parallel network, the last is commented */
  rho2z(smcdata.rerhoIP[0], smcdata.imrhoIP[0], &vr, &vi);
  node++;
  float2prefs(vr*smcdata.z0, buff, 1);
  fprintf(spf, "rsp %d %d %s\n", node-1, node, buff);
  node++;
  if(vi<0.) {
    float2prefs(-1./(2.*M_PI*smcdata.f0*vi*smcdata.z0), buff, 1);
    fprintf(spf, "csp %d %d %s\n", node-1, node, buff);
  }
  else {
    float2prefs(vi*smcdata.z0/(2.*M_PI*smcdata.f0) ,buff, 1);
    fprintf(spf, "lsp %d %d %s\n", node-1, node, buff);
  }

  rho2y(smcdata.rerhoIP[0], smcdata.imrhoIP[0], &vr, &vi);
  float2prefs(smcdata.z0/vr ,buff, 1); 
  fprintf(spf, "* rsp %d %d %s\n", node, 0, buff);
  if(vi>=0.) {
    float2prefs(vi/smcdata.z0/(2.*M_PI*smcdata.f0), buff, 1);
    fprintf(spf, "* csp %d %d %s\n", node, 0, buff);
  }
  else {
    float2prefs(-1./(2.*M_PI*smcdata.f0*vi/smcdata.z0), buff, 1);
    fprintf(spf, "* lsp %d %d %s\n", node, 0, buff);
  }

  for(i=1; i<=smcdata.ne; i++) {
    switch(tolower(smcdata.ELtype[i])) {
    case 'r': 
      nr++; 
      if(smcdata.ELplace[i]=='s') { 
	node++; float2prefs(smcdata.ELval0[i], buff, 1);
	fprintf(spf, "r%d %d %d %s\n", nr, node-1, node, buff); 
      }
      else {
	float2prefs(smcdata.ELval0[i], buff, 1);
	fprintf(spf, "r%d %d %d %s\n", nr, node, 0, buff); 
      }
      break;
    case 'l': 
      nl++;
      if(smcdata.ELplace[i]=='s') {
	node++; float2prefs(smcdata.ELval0[i], buff, 1);
	fprintf(spf, "l%d %d %d %s\n", nl, node-1, node, buff);
      }
      else {
	float2prefs(smcdata.ELval0[i], buff, 1);
	fprintf(spf, "l%d %d %d %s\n", nl, node, 0, buff);
      }
      break;
    case 'c': 
      nc++;
      if(smcdata.ELplace[i]=='s') {
	node++; float2prefs(smcdata.ELval0[i], buff, 1);
	fprintf(spf, "c%d %d %d %s\n", nc, node-1, node, buff);
      }
      else {
	float2prefs(smcdata.ELval0[i], buff, 1);
	fprintf(spf, "c%d %d %d %s\n", nc, node, 0, buff);
      }
      break;
    case 't':
      nt++;
      switch(tolower(smcdata.ELplace[i])) {
      case 't':
	node++; float2prefs(smcdata.ELval1[i], buff, 1);
	fprintf(spf, "t%d %d %d %d %d z0=%s ", nt, node-1, 0, node, 0, buff);
	float2prefs(smcdata.f0, buff, 1);
	fprintf(spf, "f=%s nl=%f\n", buff,  smcdata.ELval0[i]/360.);
	break;
      case 'o':	
	nod+=2; float2prefs(smcdata.ELval1[i], buff, 1);
	fprintf(spf, "t%d %d %d %dd %dd z0=%s ", 
		nt, node, 0, nod-1, nod, buff);
	float2prefs(smcdata.f0, buff, 1);
	fprintf(spf, "f=%s nl=%f\n", buff,  smcdata.ELval0[i]/360.);
	break;
      case 'c': 
	nos++; float2prefs(smcdata.ELval1[i], buff, 1);
	fprintf(spf, "t%d %d %d %dd %dd z0=%s ", nt, node, 0, nos, nos, buff);
	float2prefs(smcdata.f0, buff, 1);
	fprintf(spf, "f=%s nl=%f\n", buff,  smcdata.ELval0[i]/360.);
	break;
      case 'l':	
	node++; nos++; float2prefs(smcdata.ELval1[i], buff, 1);
	fprintf(spf, "t%d %d %d %dd %dd z0=%s ",
		nt, node-1, node, nos, nos, buff);
	float2prefs(smcdata.f0, buff, 1);
	fprintf(spf, "f=%s nl=%f\n", buff,  smcdata.ELval0[i]/360.);
	break;
      case 'a': 
	node++; nos+=2; float2prefs(smcdata.ELval1[i], buff, 1);
	fprintf(spf, "t%d %d %d %dd %dd z0=%s ",
		nt, node-1, node, nos-1, nos, buff);
	float2prefs(smcdata.f0, buff, 1);
	fprintf(spf, "f=%s nl=%f\n", buff,  smcdata.ELval0[i]/360.);
	break;

      }
      break;
    default: fprintf(spf, "* Unknown component '%c'\n", smcdata.ELtype[i]);
    }
  }

  node++;
  float2prefs(smcdata.z0, buff, 1);
  fprintf(spf, "r0 %d %d %s\n", node-1, node, buff);
  fprintf(spf, "v1 %d %d dc 0 ac 1\n", node, 0);

  float2prefs(smcdata.f0, buff, 1);
  fprintf(spf, "\n.ac lin 1 %s %s\n", buff, buff);
  fprintf(spf, ".end\n");
  fclose(spf);
}


void savegw(char *b) {
  FILE *gwd;
  int i;
  time_t now;

  if((gwd=fopen(b, "w"))==NULL) {
    fprintf(stderr, "Unable to open file %s for writing\n", b);
    return;
  }

  strncpy(smcdata.projectname, b, 255);
 
  now=time(NULL);
  fprintf(gwd, "GSMC Data File %s on %s\n", b, ctime(&now));

  fprintf(gwd, "z0: %g\n", smcdata.z0);
  fprintf(gwd, "f0: %g\n", smcdata.f0);

  fprintf(gwd, "StartPoint: %x %g %g %g %g %g %g %g %g\n",
	  smcdata.SPtype, smcdata.reentrySP, smcdata.imentrySP,
	  smcdata.rezSP, smcdata.imzSP, smcdata.reySP, smcdata.imySP, 
	  smcdata.rerhoSP, smcdata.imrhoSP);

  fprintf(gwd, "Elements: %d %d\n", smcdata.ne, smcdata.neidx);

  for(i=0; i<=smcdata.ne; i++)
    fprintf(gwd, "%x %g %g %g %x\n", smcdata.ELtype[i], smcdata.ELval0[i],
	    smcdata.ELval1[i], smcdata.ELval2[i], smcdata.ELplace[i]);
  
  fprintf(gwd, "GSMC Data File - end\n");

  fclose(gwd);
}

void loadgw(char *b) {
  FILE *gwd;
  char buff[101];
  int i;

  if((gwd=fopen(b, "r"))==NULL) {
    fprintf(stderr, "Unable to open file %s\n", b);
    restart(NULL, NULL); return;
  }

  strncpy(smcdata.projectname, b, 255);

  fgets(buff, 100, gwd);
  fscanf(gwd, "%s %g", buff, &smcdata.z0);
  if(strcmp(buff, "z0:")!=0){
    fprintf(stderr, "Error reading file %s\n", b);
    restart(NULL, NULL); return;
  }

  fscanf(gwd, "%s %g", buff, &smcdata.f0);
  if(strcmp(buff, "f0:")!=0){
    fprintf(stderr, "Error reading file %s\n", b);
    restart(NULL, NULL); return;
  }

  fscanf(gwd, "%s %x %g %g %g %g %g %g %g %g", buff,
	 &smcdata.SPtype, &smcdata.reentrySP, &smcdata.imentrySP,
	 &smcdata.rezSP, &smcdata.imzSP, &smcdata.reySP, &smcdata.imySP, 
	 &smcdata.rerhoSP, &smcdata.imrhoSP);
  if(strcmp(buff, "StartPoint:")!=0){
    fprintf(stderr, "Error reading file %s\n", b);
    restart(NULL, NULL); return;
  }

  fscanf(gwd, "%s %d %d\n", buff, &smcdata.ne, &smcdata.neidx);
  if(strcmp(buff, "Elements:")!=0){
    fprintf(stderr, "Error reading file %s\n", b);
    restart(NULL, NULL); return;
  }

  for(i=0; i<=smcdata.ne; i++)
    if(fscanf(gwd, "%x %g %g %g %x",
	      (int*)&smcdata.ELtype[i], &smcdata.ELval0[i],
	       &smcdata.ELval1[i], &smcdata.ELval2[i],
	      (int*)&smcdata.ELplace[i])!=5){
      fprintf(stderr, "Error reading file %s\n", b);
      restart(NULL, NULL); return;
    }
  
  fclose(gwd);
  recalc(); drawchart(); showtextstat(); showcircdesc();
}
