// FrameDlg.cpp

#include <gtk/gtk.h>
#include "FrameDlg.h"
#include <string.h>
#include <glib/gprintf.h>
#include <stdlib.h>


FrameDlg::FrameDlg(int t,float* data){

  char title[100];
  const char* strLab[6]={"Az:","El:","Rl:","X Translation:","Y Translation:","Z Translation:"};
  int labInd;
  GtkWidget* label[3];
  GtkWidget* table;
  gchar strVal[20];

  memcpy(m_data,data,sizeof(float)*3);
  if (t==FD_ROT){
    strcpy(title,"Set Frame Rotation");
    labInd=0;
  }
  else {
    strcpy(title,"Set Frame Translation");
    labInd=3;
  }


  m_dlg=gtk_dialog_new_with_buttons(title,NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,
				  GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
  gtk_container_set_border_width(GTK_CONTAINER(m_dlg),30);

  for (int i=0;i<3;i++){
    label[i]=gtk_label_new(strLab[labInd+i]);
    m_entry[i]=gtk_entry_new();
    g_sprintf(strVal,"%.3f",m_data[i]);
    gtk_entry_set_text(GTK_ENTRY(m_entry[i]),strVal);
  }

  table=gtk_grid_new();
  for (int i=0;i<3;i++){
      gtk_grid_attach(GTK_GRID(table),label[i],0,i,1,1);
      gtk_grid_attach(GTK_GRID(table),m_entry[i],1,i,1,1);
  }
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(m_dlg))),table);
  gtk_widget_show_all(m_dlg);

}

void FrameDlg::update_data(){

  const gchar* str;
  for (int i=0;i<3;i++){
    str=gtk_entry_get_text(GTK_ENTRY(m_entry[i]));
    m_data[i]=atof(str);
  }
}

void FrameDlg::GetData(float* data){

  memcpy(data,m_data,sizeof(float)*3);
}

