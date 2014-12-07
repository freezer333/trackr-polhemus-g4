// ModDlg.h

#ifndef _MODDLG_H_
#define _MODDLG_H_


class ModalDlg {

 protected:

  GtkWidget* m_dlg;
  virtual void update_data()=0;

 public:

  ModalDlg(){}
  int present_dlg();

};

#endif
