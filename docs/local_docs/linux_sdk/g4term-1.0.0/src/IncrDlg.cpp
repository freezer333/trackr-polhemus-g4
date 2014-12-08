// IncrDlg.cpp

#include <gtk/gtk.h>
#include "IncrDlg.h"
#include <glib/gprintf.h>
#include <stdlib.h>

IncrDlg::IncrDlg(float nPos,float nOri){

  m_posIncr=nPos;
  m_oriIncr=nOri;
  gchar str[100];

  m_dlg=gtk_dialog_new_with_buttons("Specify Increments",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,
				    GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);

  GtkWidget* table=gtk_grid_new();
  GtkWidget *lab1,*lab2;

  lab1=gtk_label_new("Position Increment");
  lab2=gtk_label_new("Orientation Increment");
  m_entry_p=gtk_entry_new();
  m_entry_o=gtk_entry_new();


  gtk_container_set_border_width(GTK_CONTAINER(table),10);
  gtk_grid_set_column_spacing(GTK_GRID(table),25);
  gtk_grid_attach(GTK_GRID(table),lab1,0,1,1,1);
  gtk_grid_attach(GTK_GRID(table),lab2,0,2,1,1);
  gtk_grid_attach(GTK_GRID(table),m_entry_p,1,1,1,1);
  gtk_grid_attach(GTK_GRID(table),m_entry_o,1,2,1,1);


  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(m_dlg))),table);

  g_sprintf(str,"%.3f",m_posIncr);
  gtk_entry_set_text(GTK_ENTRY(m_entry_p),str);
  g_sprintf(str,"%.3f",m_oriIncr);
  gtk_entry_set_text(GTK_ENTRY(m_entry_o),str);

  gtk_widget_show_all(m_dlg);

}

void IncrDlg::update_data(){

  const gchar* str=gtk_entry_get_text(GTK_ENTRY(m_entry_p));
  m_posIncr=atof(str);


  str=gtk_entry_get_text(GTK_ENTRY(m_entry_o));
  m_oriIncr=atof(str);

}

void IncrDlg::get_increments(float& nPos,float& nOri){

  nPos=m_posIncr;
  nOri=m_oriIncr;
}
