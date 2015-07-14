// FilterDlg.h

#ifndef FILTERDLG_H_
#define FILTERDLG_H_

#include "ModDlg.h"


class FilterDlg : public ModalDlg {

 private:

  GtkWidget* m_entry[2][4];

  float m_posFiltVals[4];
  float m_oriFiltVals[4];

  virtual void update_data();

 public:

  FilterDlg(float*,float*);
  void GetFilterData(float*,float*);


};


#endif
