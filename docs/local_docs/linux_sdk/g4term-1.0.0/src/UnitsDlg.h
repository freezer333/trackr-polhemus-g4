// UnitsDlg.h

#ifndef UNITSDLG_H_
#define UNITSDLG_H_

#include "ModDlg.h"

class UnitsDlg : public ModalDlg {

 private:

  int m_posUnit;
  int m_oriUnit;

  virtual void update_data();
  static void OnPosRadio(GtkToggleButton*,gpointer);
  static void OnOriRadio(GtkToggleButton*,gpointer);

 public:

  UnitsDlg(int,int);
  void GetUnits(int&,int&);

};

#endif
