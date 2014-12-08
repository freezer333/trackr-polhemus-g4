// IncrDlg.h

#ifndef INCRDLG_H_
#define INCRDLG_H_

#include "ModDlg.h"

class IncrDlg : public ModalDlg {

 private:
  float m_posIncr;
  float m_oriIncr;
  GtkWidget* m_entry_p;
  GtkWidget* m_entry_o;

  virtual void update_data();

 public:

  IncrDlg(float,float);
  void get_increments(float&,float&);

};

#endif
