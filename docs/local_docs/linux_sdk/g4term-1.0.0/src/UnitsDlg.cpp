// UnitsDlg.cpp


#include <gtk/gtk.h>
#include "UnitsDlg.h"
#include "G4TrackIncl.h"


UnitsDlg::UnitsDlg(int posUnit,int oriUnit){

  GtkWidget *vbox1,*vbox2,*hbox,*fr1,*fr2;
  GtkWidget *inch,*ft,*met,*cm,*deg,*rad,*quat;

  m_posUnit=posUnit;
  m_oriUnit=oriUnit;

  m_dlg=gtk_dialog_new_with_buttons("Set Tracker Units",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,
				  GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);

  gtk_widget_set_size_request(m_dlg,300,200);
  fr1=gtk_frame_new("Position");
  fr2=gtk_frame_new("Orientation");
  hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);


  vbox1=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
  inch=gtk_radio_button_new_with_label(NULL,"Inches");
  gtk_box_pack_start(GTK_BOX(vbox1),inch,TRUE,TRUE,3);
  ft=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(inch),"Feet");
  gtk_box_pack_start(GTK_BOX(vbox1),ft,TRUE,TRUE,3);
  cm=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(inch),"cm");
  gtk_box_pack_start(GTK_BOX(vbox1),cm,TRUE,TRUE,3);
  met=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(inch),"meters");
  gtk_box_pack_start(GTK_BOX(vbox1),met,TRUE,TRUE,3);
  gtk_container_add(GTK_CONTAINER(fr1),vbox1);
  gtk_box_pack_start(GTK_BOX(hbox),fr1,TRUE,TRUE,5);

  vbox2=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
  deg=gtk_radio_button_new_with_label(NULL,"Degrees");
  gtk_box_pack_start(GTK_BOX(vbox2),deg,TRUE,TRUE,3);
  rad=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(deg),"Radians");
  gtk_box_pack_start(GTK_BOX(vbox2),rad,TRUE,TRUE,3);
  quat=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(deg),"Quaternions");
  gtk_box_pack_start(GTK_BOX(vbox2),quat,TRUE,TRUE,3);
  gtk_container_add(GTK_CONTAINER(fr2),vbox2);
  gtk_box_pack_start(GTK_BOX(hbox),fr2,TRUE,TRUE,5);

  g_signal_connect(inch,"toggled",G_CALLBACK(OnPosRadio),(gpointer)G4_TYPE_INCH);
  g_signal_connect(ft,"toggled",G_CALLBACK(OnPosRadio),(gpointer)G4_TYPE_FOOT);
  g_signal_connect(cm,"toggled",G_CALLBACK(OnPosRadio),(gpointer)G4_TYPE_CM);
  g_signal_connect(met,"toggled",G_CALLBACK(OnPosRadio),(gpointer)G4_TYPE_METER);

  g_object_set_data(G_OBJECT(inch),"pos_unit",&m_posUnit);
  g_object_set_data(G_OBJECT(ft),"pos_unit",&m_posUnit);
  g_object_set_data(G_OBJECT(cm),"pos_unit",&m_posUnit);
  g_object_set_data(G_OBJECT(met),"pos_unit",&m_posUnit);


  g_signal_connect(deg,"toggled",G_CALLBACK(OnOriRadio),(gpointer)G4_TYPE_EULER_DEGREE);
  g_signal_connect(rad,"toggled",G_CALLBACK(OnOriRadio),(gpointer)G4_TYPE_EULER_RADIAN);
  g_signal_connect(quat,"toggled",G_CALLBACK(OnOriRadio),(gpointer)G4_TYPE_QUATERNION);

  g_object_set_data(G_OBJECT(deg),"ori_unit",&m_oriUnit);
  g_object_set_data(G_OBJECT(rad),"ori_unit",&m_oriUnit);
  g_object_set_data(G_OBJECT(quat),"ori_unit",&m_oriUnit);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inch),posUnit==G4_TYPE_INCH);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ft),posUnit==G4_TYPE_FOOT);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cm),posUnit==G4_TYPE_CM);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(met),posUnit==G4_TYPE_METER);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(deg),oriUnit==G4_TYPE_EULER_DEGREE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rad),oriUnit==G4_TYPE_EULER_RADIAN);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(quat),oriUnit==G4_TYPE_QUATERNION);

  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(m_dlg))),hbox);
  gtk_widget_show_all(m_dlg);

}


void UnitsDlg::OnPosRadio(GtkToggleButton* b,gpointer g){

  int* pPos=(int*)g_object_get_data(G_OBJECT(b),"pos_unit");
  if (gtk_toggle_button_get_active(b))
    *pPos=(int)(long)g;
}

void UnitsDlg::OnOriRadio(GtkToggleButton* b,gpointer g){

  int* pOri=(int*)g_object_get_data(G_OBJECT(b),"ori_unit");
  if (gtk_toggle_button_get_active(b))
    *pOri=(int)(long)g;
}

void UnitsDlg::GetUnits(int& posUnit,int& oriUnit){

  posUnit=m_posUnit;
  oriUnit=m_oriUnit;
}

void UnitsDlg::update_data(){}
