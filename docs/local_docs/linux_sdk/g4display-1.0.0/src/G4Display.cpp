#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Quaternion.h"
#include "struct.h"
#include "G4Trk.h"
#include "G4Hub.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "G4Display.h"
#include <gdk/gdkkeysyms.h>
#include "IncrDlg.h"
#include "FilterDlg.h"
#include "config.h"


int RunCollect=0;
pthread_t thread1;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

const char* g4_version=VERSION;
const char* g4_comment="Graphical Display Program to communicate with the Polhemus G4 Tracker";


int main(int argc,char* argv[]){

  GtkWidget* win,*da,*fileDlg;
  GdkGLConfig* glConfig;
  CG4Trk trk;
  REND_STRUCT rs;
  char* cfgFile;
  GtkFileFilter *all,*g4c;


  memset(rs.viewTrans,0,sizeof(float)*3);
  rs.srcScale=rs.senScale=0.5;
  rs.counter=0;

  gtk_init(&argc,&argv);
  gtk_gl_init(&argc,&argv);

  // show the images on the buttons
  g_object_set(gtk_settings_get_default(),"gtk-button_images",TRUE,NULL);

  win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win),"G4 Display Application");

  da=gtk_drawing_area_new();
  gtk_widget_set_size_request(da,900,700);
  gtk_widget_set_events(da,GDK_EXPOSURE_MASK|GDK_KEY_PRESS_MASK);
  gtk_container_add(GTK_CONTAINER(win),da);
  gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_NONE);
  gtk_widget_show(win);

  glConfig=gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGB|GDK_GL_MODE_DEPTH|GDK_GL_MODE_DOUBLE));
  gtk_widget_set_gl_capability(da,glConfig,NULL,TRUE,GDK_GL_RGBA);

  gtk_widget_show_all(win);
  gtk_window_maximize(GTK_WINDOW(win));


  g_signal_connect(win,"destroy",G_CALLBACK(QuitApp),NULL);
  g_signal_connect(da,"configure-event",G_CALLBACK(cb_configure),NULL);
  g_signal_connect(da,"expose-event",G_CALLBACK(cb_expose),&rs);
  g_signal_connect(win,"key-press-event",G_CALLBACK(cb_keyPress),&rs);

  g_object_set_data(G_OBJECT(da),"tracker",&trk);

  all=gtk_file_filter_new();
  g4c=gtk_file_filter_new();
  gtk_file_filter_set_name(all,"All Files");
  gtk_file_filter_set_name(g4c,"G4 Source Configuration Files (*.g4c)");
  gtk_file_filter_add_pattern(all,"*");
  gtk_file_filter_add_pattern(g4c,"*.g4c");
  fileDlg=gtk_file_chooser_dialog_new("Select a Source Configuration File",NULL,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,
				      GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fileDlg),g4c);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fileDlg),all);
  

  int resp=gtk_dialog_run(GTK_DIALOG(fileDlg));
  if (resp==GTK_RESPONSE_CANCEL){
    gtk_widget_destroy(fileDlg);
    return -2;
  }
  cfgFile=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileDlg));
  gtk_widget_destroy(fileDlg);
  resp=trk.Connect(cfgFile);
  g_free(cfgFile);
  if (!resp){
    printf("Error Connecting to G4\n");
    return -1;
  }
 
  usleep(500000);
  rs.numSrc=trk.GetNumSrc();
  rs.srcList=new float[rs.numSrc][6];
  trk.GetSrcLoc(rs.srcList);

  //  rs.numHub=trk.GetActHubs();
  rs.numHub=trk.UpdateHubs();
  rs.hubList=new CG4Hub[rs.numHub];
  int* hubIds=new int[rs.numHub];
  trk.GetHubList(hubIds,rs.numHub);
  int color_ind=0;
  for (int i=0;i<rs.numHub;i++){
    rs.hubList[i].SetId(hubIds[i]);
    rs.hubList[i].SetColor(triad_colors[color_ind]);
    color_ind++;
    if (color_ind>=15)
      color_ind=0;
  }
  delete[] hubIds;

  rs.pTrk=&trk;
 
  RunCollect=1;
  //  g_timeout_add(8,cb_timer,da);
  g_timeout_add(REFRESH_RATE,cb_timer,da);

  pthread_create(&thread1,NULL,Collect,&rs);
  gtk_main();

  delete[] rs.srcList;
  delete[] rs.hubList;

  return 0;
}
  
gboolean cb_configure(GtkWidget* w,GdkEventConfigure* e,gpointer g){

  GdkGLDrawable* glDraw;
  GdkGLContext* glContext;

  glDraw=gtk_widget_get_gl_drawable(w);
  glContext=gtk_widget_get_gl_context(w);

  gdk_gl_drawable_gl_begin(glDraw,glContext);

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,(GLsizei)w->allocation.width,(GLsizei)w->allocation.height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,(float)w->allocation.width/(float)w->allocation.height,0.1,200.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gdk_gl_drawable_gl_end(glDraw);
  return TRUE;
}

gboolean cb_expose(GtkWidget* w,GdkEventExpose* e,gpointer g){

  GdkGLDrawable* glDraw;
  GdkGLContext* glContext;
  REND_STRUCT* prs=(REND_STRUCT*)g;
  int refresh_cnt=(int)(1.0/(float)REFRESH_RATE*1000.0);	// about every sec
  CG4Trk* pTrk=(CG4Trk*)g_object_get_data(G_OBJECT(w),"tracker");
  int numHubs;

  if (!RunCollect)
    return TRUE;

  // periodically check for new hubs coming on line
  if (++prs->counter>=refresh_cnt){
    prs->counter=0;
    if (prs->tot_hubs_on_system!=prs->numHub){
      prs->numHub=numHubs=prs->tot_hubs_on_system=pTrk->UpdateHubs();
    // numHubs=pTrk->UpdateHubs();
    // if (numHubs!=prs->numHub){
      int colorInd=0;
      if (prs->hubList)
  	delete[] prs->hubList;
      prs->hubList=new CG4Hub[numHubs];
      int* hubIds=new int[numHubs];
      pTrk->GetHubList(hubIds,numHubs);
      for (int i=0;i<numHubs;i++){
  	prs->hubList[i].SetId(hubIds[i]);
  	prs->hubList[i].SetColor(triad_colors[colorInd++]);
	// 	colorInd+=3;
  	if (colorInd>=15)
  	  colorInd=0;
      }
      delete[] hubIds;
    }
  }



  glDraw=gtk_widget_get_gl_drawable(w);
  glContext=gtk_widget_get_gl_context(w);

  gdk_gl_drawable_gl_begin(glDraw,glContext);

  GLfloat lightPos[4]={-100.0f,500.0f,-100.0f,1.0f};
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);

  glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
  glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);





  glLoadIdentity();
  gluLookAt(0.0,0.0,30.0,0.0,0.0,0.0,0.0,10.0,30.0);

  GLUquadricObj* quad=gluNewQuadric();

  // Tracker coords    x  y  z  |  y  -z  -x
  // OpenGL coords    -z  x  -y |  x   y   z

  float ang,vec[3];
  prs->viewRot.GetAxisAngle(vec,ang);

  glPushMatrix();
  glTranslatef(prs->viewTrans[1],-prs->viewTrans[2],-prs->viewTrans[0]);
  glRotatef(ang,vec[1],-vec[2],-vec[0]);
  //glScalef(0.3,0.3,0.3);
  //  glScalef(5.0,5.0,5.0);

  glPushMatrix();

  RenderSources(quad,prs);

  pthread_mutex_lock(&mutex);
  RenderSensors(quad,prs);
  pthread_mutex_unlock(&mutex);

  glPopMatrix();
  glPopMatrix();

  gluDeleteQuadric(quad);

  gdk_gl_drawable_swap_buffers(glDraw);
  gdk_gl_drawable_gl_end(glDraw);
  return TRUE;
}

gboolean cb_timer(gpointer g){

  GtkWidget* w=GTK_WIDGET(g);

  gdk_window_invalidate_rect(w->window,&w->allocation,FALSE);
  gdk_window_process_updates(w->window,FALSE);
  return TRUE;
}

void RenderSources(GLUquadricObj* quad,REND_STRUCT* prs){

  for (int i=0;i<prs->numSrc;i++){

    glPushMatrix();
    glTranslatef(prs->srcList[i][1],-prs->srcList[i][2],-prs->srcList[i][0]); // ogl(x,y,z)->trk(y,-z,-x)
    glColor3f(1.0,0.0,0.0);
    glPushMatrix();
    glScalef(prs->srcScale,prs->srcScale,prs->srcScale);
    gluSphere(quad,1.0,50,25);
    glPopMatrix();
    glPopMatrix();
  }
}


void RenderSensors(GLUquadricObj* quad,REND_STRUCT* prs){

  for (int i=0;i<prs->numHub;i++){
    if (prs->hubreadmap&(0x01<<i))
      prs->hubList[i].Render(quad,prs->senScale);
  }


}

void QuitApp(GtkWidget* w,gpointer g){

  RunCollect=0;
  pthread_join(thread1,NULL);
  gtk_main_quit();

}


void* Collect(void* p){

  REND_STRUCT* prs=(REND_STRUCT*)p;
  
  while (RunCollect){
    pthread_mutex_lock(&mutex);
    prs->hubreadmap=0;
    prs->tot_hubs_on_system=prs->pTrk->GetHubsPno(prs);
    pthread_mutex_unlock(&mutex);
    usleep(15000);
    // usleep(8000);
  }

  return NULL;
}

gboolean cb_keyPress(GtkWidget* w,GdkEventKey* e,gpointer g){

  int incr=1.0;
  REND_STRUCT* prs=(REND_STRUCT*)g;
  CQuaternion q;

  // alt-Q
  if ((e->state&GDK_MOD1_MASK)&&((e->keyval==GDK_Q)||(e->keyval==GDK_q)))
    QuitApp(w,NULL);

  // Reset -- Escape Key
  else if (e->keyval==GDK_Escape){
    memset(prs->viewTrans,0,sizeof(float)*3);
    prs->viewRot.SetQuatVals(1.0,0.0,0.0,0.0);
    prs->senScale=prs->srcScale=0.5;
  }

  // Directional Keys / Borsight
  else {

    if (e->state&GDK_SHIFT_MASK)
      incr*=5.0;

    switch (e->keyval){
    case GDK_F2:
      prs->viewTrans[0]+=incr;
      break;
    case GDK_F3:
      prs->viewTrans[0]-=incr;
      break;
    case GDK_Right:
      if (e->state & GDK_CONTROL_MASK)
	prs->viewTrans[1]+=incr;
      else if (e->state & GDK_MOD1_MASK){    // alt is pressed
	q=CQuaternion(0.0,0.0,incr);
	prs->viewRot*=q;
      }
      else{
	q=CQuaternion(incr,0.0,0.0);
	prs->viewRot*=q;
      }

      break;
    case GDK_Left:
      if(e->state & GDK_CONTROL_MASK)
	prs->viewTrans[1]-=incr;
      else if (e->state & GDK_MOD1_MASK){
	q=CQuaternion(0.0,0.0,-incr);
	prs->viewRot*=q;
      }
      else{
	q=CQuaternion(-incr,0.0,0.0);
	prs->viewRot*=q;
      }
      break;
    case GDK_Up:
      if (e->state & GDK_CONTROL_MASK)
	prs->viewTrans[2]-=incr;
      else{
	q=CQuaternion(0.0,incr,0.0);
	prs->viewRot*=q;
      }
      break;
    case GDK_Down:
      if (e->state & GDK_CONTROL_MASK)
	prs->viewTrans[2]+=incr;
      else {
	q=CQuaternion(0.0,-incr,0.0);
	prs->viewRot*=q;
      }
      break;
    case GDK_B:
    case GDK_b:
      if (e->state & GDK_CONTROL_MASK){
	if(e->state & GDK_SHIFT_MASK)
	  prs->pTrk->Boresight(false);
	else
	  prs->pTrk->Boresight(true);
      }
      break;

    case GDK_F:
    case GDK_f:
      OnFilter(prs->pTrk);
      break;

    case GDK_N:
    case GDK_n:
      OnIncr(prs->pTrk);
      break;

    case GDK_V:
    case GDK_v:
      OnVersion(w);
      break;

    case GDK_plus:
	prs->srcScale+=0.5;
	break;
    case GDK_equal:
	prs->senScale+=0.5;
      break;

    case GDK_underscore:
	prs->srcScale-=0.5;
	break;
    case GDK_minus:
	prs->senScale-=0.5f;
      break;

    }// end switch

  }// end else


  return FALSE;
}

void OnFilter(CG4Trk* pTrk){

  // first get current filters to load dialog
  float posFilt[4];
  float oriFilt[4];

  pTrk->GetFilterValues(posFilt,oriFilt);
  FilterDlg dlg(posFilt,oriFilt);
  int resp=dlg.present_dlg();
  if (resp==GTK_RESPONSE_OK){
    dlg.GetFilterData(posFilt,oriFilt);
    pTrk->SetFilterValues(posFilt,oriFilt);
  }

}

void OnIncr(CG4Trk* pTrk){

  float p_incr,o_incr;
  pTrk->GetSetIncr(p_incr,o_incr,FALSE);
  IncrDlg dlg(p_incr,o_incr);
  int resp=dlg.present_dlg();
  if (resp==GTK_RESPONSE_OK){
    dlg.get_increments(p_incr,o_incr);
    pTrk->GetSetIncr(p_incr,o_incr,TRUE);
  }

}

void OnVersion(GtkWidget* w){

  const gchar* authors[]={"James C. Farr, Polhemus Inc.",NULL};

  gtk_show_about_dialog(GTK_WINDOW(w),"authors",authors,"comments",g4_comment,"copyright","Copyright © Polhemus 2009-2013","program-name","g4display","version",g4_version,
  			"website","http://www.polhemus.com/",NULL);

}
