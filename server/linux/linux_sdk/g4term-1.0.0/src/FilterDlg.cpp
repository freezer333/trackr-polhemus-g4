// FilterDlg.cpp

#include <gtk/gtk.h>
#include "FilterDlg.h"
#include <stdlib.h>
#include <string.h>


FilterDlg::FilterDlg(float* pos,float* ori){

  GtkWidget* hbox[2],*frame[2],*vbox;
  char tmp[20];

  m_dlg=gtk_dialog_new_with_buttons("Set Filter Values",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,
				  GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
  gtk_container_set_border_width(GTK_CONTAINER(m_dlg),30);
  hbox[0]=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
  hbox[1]=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);

  frame[0]=gtk_frame_new("Position");
  frame[1]=gtk_frame_new("Orientation");

  vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,10);


  for (int a=0;a<2;a++){

    for (int i=0;i<4;i++){
      m_entry[a][i]=gtk_entry_new();
      gtk_widget_set_size_request(m_entry[a][i],75,25);
      gtk_box_pack_start(GTK_BOX(hbox[a]),m_entry[a][i],TRUE,TRUE,10);
    }


    gtk_container_add(GTK_CONTAINER(frame[a]),hbox[a]);


    gtk_box_pack_start(GTK_BOX(vbox),frame[a],FALSE,FALSE,5);

  }

  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(m_dlg))),vbox);

  for (int i=0;i<4;i++){
    sprintf(tmp,"%.4f",pos[i]);
    gtk_entry_set_text(GTK_ENTRY(m_entry[0][i]),tmp);
    sprintf(tmp,"%.4f",ori[i]);
    gtk_entry_set_text(GTK_ENTRY(m_entry[1][i]),tmp);
  }

  gtk_widget_show_all(m_dlg);
};


void FilterDlg::update_data(){

  const gchar* tmp;
  for (int i=0;i<4;i++){
    tmp=gtk_entry_get_text(GTK_ENTRY(m_entry[0][i]));
    m_posFiltVals[i]=atof(tmp);
    tmp=gtk_entry_get_text(GTK_ENTRY(m_entry[1][i]));
    m_oriFiltVals[i]=atof(tmp);
  }
}

void FilterDlg::GetFilterData(float* posVals,float* oriVals){

  const int FLOAT4=sizeof(float)*4;
  memcpy(posVals,m_posFiltVals,FLOAT4);
  memcpy(oriVals,m_oriFiltVals,FLOAT4);
}

