// ModDlg.cpp

#include <gtk/gtk.h>
#include "ModDlg.h"



int ModalDlg::present_dlg(){

  int rv=gtk_dialog_run(GTK_DIALOG(m_dlg));
  update_data();
  gtk_widget_destroy(m_dlg);
  return rv;

}
