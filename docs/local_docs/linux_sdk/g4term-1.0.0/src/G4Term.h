

// G4Term.h


#ifndef G4TERM_H_
#define G4TERM_H_




void OnCnx(GtkWidget*,gpointer);
void OnDnx(GtkWidget*,gpointer);
void OnSingle(GtkWidget*,gpointer);
static void* ContThreadFxn(void*);
void OnStartCont(GtkWidget*,gpointer);
void OnStopCont(GtkWidget*,gpointer);
gboolean GetContData(gpointer);

void OnCapture(GtkWidget*,gpointer);
void OnBoresight(GtkWidget*,gpointer);
void OnRotate(GtkWidget*,gpointer);
void OnTranslate(GtkWidget*,gpointer);
void OnFilter(GtkWidget*,gpointer);
void OnIncr(GtkWidget*,gpointer);
void OnUnits(GtkWidget*,gpointer);
void OnClear(GtkWidget*,gpointer);
void OnAbout(GtkWidget*,gpointer);
void OnClose(GtkWidget*,gpointer);

void OutputData(GtkWidget* textview,G4_FRAMEDATA* fd,int num,FILE* f);


#endif
