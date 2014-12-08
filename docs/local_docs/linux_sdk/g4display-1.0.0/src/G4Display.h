// G4Display.h

#ifndef G4DISPLAY_H_
#define G4DISPLAY_H_




#define REFRESH_RATE	30


gboolean cb_configure(GtkWidget*,GdkEventConfigure*,gpointer);
gboolean cb_expose(GtkWidget*,GdkEventExpose*,gpointer);
gboolean cb_timer(gpointer);
gboolean cb_keyPress(GtkWidget*,GdkEventKey*,gpointer);

void RenderSources(GLUquadricObj*,REND_STRUCT*);
void RenderSensors(GLUquadricObj* quad,REND_STRUCT* prs);
void QuitApp(GtkWidget*,gpointer);
static void* Collect(void*);

void OnFilter(CG4Trk*);
void OnIncr(CG4Trk*);
void OnVersion(GtkWidget*);


const float triad_colors[15][3]={

  {0.0,0.5,0.5},   // teal
  {1.0,0.0,0.0},   // red
  {0.0,0.0,1.0},   // blue
  {1.0,1.0,0.0},   // yellow
  {0.5,0.0,1.0},   // purple
  {0.0,0.5,1.0},   // sky blue
  {0.0,0.0,0.0},   // black
  {0.5,1.0,0.5},   // bright green
  {0.0,0.5,0.0},   // lime
  {0.25,0.0,0.0},  // brown
  {1.0,0.5,0.0},   // orange
  {0.5,0.5,0.5},   // grey
  {0.5,0.5,0.0},   // kinda mustard green
  {1.0,0.0,0.5},   // pink
  {0.9,0.9,0.9}    // almond white
};




#endif
