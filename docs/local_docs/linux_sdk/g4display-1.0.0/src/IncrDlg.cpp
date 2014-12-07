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

  GtkWidget* table=gtk_table_new(4,2,TRUE);
  GtkWidget *lab1,*lab2;

  lab1=gtk_label_new("Position Increment");
  lab2=gtk_label_new("Orientation Increment");
  m_entry_p=gtk_entry_new();
  m_entry_o=gtk_entry_new();

  gtk_table_attach_defaults(GTK_TABLE(table),lab1,0,1,1,2);
  gtk_table_attach_defaults(GTK_TABLE(table),lab2,0,1,3,4);
  gtk_table_attach(GTK_TABLE(table),m_entry_p,1,2,1,2,
		   (GtkAttachOptions)(GTK_EXPAND|GTK_FILL),
		   (GtkAttachOptions)(GTK_EXPAND|GTK_FILL),5,0);
  gtk_table_attach(GTK_TABLE(table),m_entry_o,1,2,3,4,
		   (GtkAttachOptions)(GTK_FILL|GTK_EXPAND),
		   (GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,0);

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(m_dlg)->vbox),table,TRUE,TRUE,10);

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
