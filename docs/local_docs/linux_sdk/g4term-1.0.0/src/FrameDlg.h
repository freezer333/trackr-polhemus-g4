// FrameDlg.h

#ifndef FRAMEDLG_H_
#define FRAMEDLG_H_

#include "ModDlg.h"

//enum fd_type {FD_ROT,FD_TRANS};
#define FD_ROT  0
#define FD_TRANS 1

class FrameDlg : public ModalDlg {

 private:
  float m_data[3];
  GtkWidget* m_entry[3];

  virtual void update_data();

 public:

  FrameDlg(int,float*);
  void GetData(float*);
};


#endif
