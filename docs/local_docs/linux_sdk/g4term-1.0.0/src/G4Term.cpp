// G4Term.cpp

#include <gtk/gtk.h>
#include "G4TrackIncl.h"
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include "G4Term.h"
#include "PingPong.h"
#include "IncrDlg.h"
#include "FrameDlg.h"
#include "FilterDlg.h"
#include "UnitsDlg.h"
#include <string.h>
#include "config.h"




const char* piterm_version=VERSION;
const char* piterm_comment="Terminal program to communicate with the Polhemus G4 Tracker";

enum {CNX_BUT,DNX_BUT,SINGLE_BUT,START_BUT,STOP_BUT,NUMBUTTONS};
pthread_t contThread;
int gRunCont=0;
int gConnected=0;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

int isQuats=0;
int sysId;



typedef struct _THREADINFO {
  PingPong pp;
  GtkWidget* text;
  FILE* fCap;
}THREADINFO;

typedef struct _FILEINFO {
  FILE* f;
  char filename[200];
}FILEINFO;

int main(int argc,char* argv[]){


  GtkWidget* win;
  GtkWidget* butArr[NUMBUTTONS];
  GtkWidget *cnx_but_box,*act_button_box,*vbox,*hbox,*cfg_button_box;
  GtkWidget* textview,*scrWin;
  GtkWidget *capture,*boresight,*unboresight,*rotate,*translate,*filter,*incr,*units,*clear,*about,*quit;
  GtkWidget* image;

  FILEINFO fi;
  fi.f=NULL;
  memset(fi.filename,0,sizeof(fi.filename));

  gtk_init(&argc,&argv);

  // This should have defaulted to true, but it didn't
  g_object_set(gtk_settings_get_default(),"gtk-button_images",TRUE,NULL);

  win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win),"Polhemus G4 Terminal");
  gtk_container_set_border_width(GTK_CONTAINER(win),10);
  gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_CENTER);

  vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
  gtk_box_set_homogeneous(GTK_BOX(vbox),FALSE);

  hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
  gtk_box_set_homogeneous(GTK_BOX(hbox),FALSE);


  // connection buttons
  cnx_but_box=gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(cnx_but_box),GTK_BUTTONBOX_SPREAD);
  butArr[CNX_BUT]=gtk_button_new_from_stock(GTK_STOCK_CONNECT);
  butArr[DNX_BUT]=gtk_button_new_from_stock(GTK_STOCK_DISCONNECT);
  gtk_container_add(GTK_CONTAINER(cnx_but_box),butArr[CNX_BUT]);
  gtk_container_add(GTK_CONTAINER(cnx_but_box),butArr[DNX_BUT]);
  gtk_widget_set_sensitive(butArr[DNX_BUT],FALSE);

  g_signal_connect(butArr[CNX_BUT],"clicked",G_CALLBACK(OnCnx),butArr);
  g_signal_connect(butArr[DNX_BUT],"clicked",G_CALLBACK(OnDnx),butArr);

  gtk_box_pack_start(GTK_BOX(vbox),cnx_but_box,FALSE,FALSE,10);

  // action buttons
  act_button_box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
  butArr[SINGLE_BUT]=gtk_button_new_with_label("Single Frame");
  butArr[START_BUT]=gtk_button_new_with_label("Start Continuous");
  butArr[STOP_BUT]=gtk_button_new_with_label("Stop Continuous");
  gtk_box_pack_start(GTK_BOX(act_button_box),butArr[SINGLE_BUT],TRUE,TRUE,10);
  gtk_box_pack_start(GTK_BOX(act_button_box),butArr[START_BUT],TRUE,TRUE,10);
  gtk_box_pack_start(GTK_BOX(act_button_box),butArr[STOP_BUT],TRUE,TRUE,10);
  gtk_box_pack_start(GTK_BOX(vbox),act_button_box,FALSE,FALSE,10);
  gtk_widget_set_sensitive(butArr[SINGLE_BUT],FALSE);
  gtk_widget_set_sensitive(butArr[START_BUT],FALSE);
  gtk_widget_set_sensitive(butArr[STOP_BUT],FALSE);


  g_object_set_data(G_OBJECT(butArr[START_BUT]),"stop_but",(gpointer)butArr[STOP_BUT]);





  // output window
  scrWin=gtk_scrolled_window_new(NULL,NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrWin),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(scrWin,700,500);
  textview=gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview),FALSE);
  gtk_container_add(GTK_CONTAINER(scrWin),textview);
  gtk_box_pack_start(GTK_BOX(vbox),scrWin,TRUE,TRUE,10);

  g_object_set_data(G_OBJECT(butArr[SINGLE_BUT]),"file",&fi);
  g_object_set_data(G_OBJECT(butArr[START_BUT]),"file",&fi);
  g_signal_connect_swapped(butArr[SINGLE_BUT],"clicked",G_CALLBACK(OnSingle),textview);
  g_signal_connect_swapped(butArr[START_BUT],"clicked",G_CALLBACK(OnStartCont),textview);
  g_signal_connect(butArr[STOP_BUT],"clicked",G_CALLBACK(OnStopCont),(gpointer)butArr[START_BUT]);


  gtk_box_pack_start(GTK_BOX(hbox),vbox,TRUE,TRUE,10);

  //config buttons
  cfg_button_box=gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(cfg_button_box),GTK_BUTTONBOX_CENTER);
  capture=gtk_button_new_with_label("Capture...");
  image=gtk_image_new_from_stock(GTK_STOCK_FLOPPY,GTK_ICON_SIZE_BUTTON);
  gtk_button_set_image(GTK_BUTTON(capture),image);
  boresight=gtk_button_new_with_label("Boresight");
  unboresight=gtk_button_new_with_label("Unboresight");
  rotate=gtk_button_new_with_label("Frame Rotation");
  translate=gtk_button_new_with_label("Frame Translation");
  filter=gtk_button_new_with_label("Filters");
  incr=gtk_button_new_with_label("Increments");
  units=gtk_button_new_with_label("Units");
  clear=gtk_button_new_from_stock(GTK_STOCK_CLEAR);
  about=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
  quit=gtk_button_new_from_stock(GTK_STOCK_CLOSE);

  gtk_container_add(GTK_CONTAINER(cfg_button_box),capture);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),boresight);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),unboresight);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),rotate);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),translate);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),filter);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),incr);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),units);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),clear);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),about);
  gtk_container_add(GTK_CONTAINER(cfg_button_box),quit);

  gtk_box_pack_start(GTK_BOX(hbox),cfg_button_box,FALSE,FALSE,10);


  gtk_container_add(GTK_CONTAINER(win),hbox);

  g_signal_connect(capture,"clicked",G_CALLBACK(OnCapture),&fi);
  g_signal_connect(boresight,"clicked",G_CALLBACK(OnBoresight),(gpointer)1);
  g_signal_connect(unboresight,"clicked",G_CALLBACK(OnBoresight),(gpointer)0);
  g_signal_connect(rotate,"clicked",G_CALLBACK(OnRotate),NULL);
  g_signal_connect(translate,"clicked",G_CALLBACK(OnTranslate),NULL);
  g_signal_connect(filter,"clicked",G_CALLBACK(OnFilter),NULL);
  g_signal_connect(incr,"clicked",G_CALLBACK(OnIncr),NULL);
  g_signal_connect(clear,"clicked",G_CALLBACK(OnClear),(gpointer)textview);
  g_signal_connect(about,"clicked",G_CALLBACK(OnAbout),(gpointer)win);
  g_signal_connect(units,"clicked",G_CALLBACK(OnUnits),win);



  gtk_widget_show_all(win);


  g_signal_connect(win,"destroy",G_CALLBACK(OnClose),NULL);
  g_signal_connect(quit,"clicked",G_CALLBACK(OnClose),NULL);

  gtk_main();

  if (fi.f)
    fclose(fi.f);

  return 0;

}


void OnCnx(GtkWidget* w,gpointer g){

  int rv;
  GtkWidget* mb,*fdlg;
  GtkWidget** buttons=(GtkWidget**)g;
  gchar* filename;
  GtkFileFilter* filter1,*filter2;

  // query for the source config file to use
  fdlg=gtk_file_chooser_dialog_new("Choose Source Configuration File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,
				GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);

  filter1=gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter1,"*.g4c");
  gtk_file_filter_set_name(filter1,"G4 Source Cfg File");
  filter2=gtk_file_filter_new();
  gtk_file_filter_add_pattern(filter2,"*.*");
  gtk_file_filter_set_name(filter2,"All Files");


  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fdlg),filter1);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fdlg),filter2);
  if (gtk_dialog_run(GTK_DIALOG(fdlg))==GTK_RESPONSE_CANCEL){
    gtk_widget_destroy(fdlg);
    return;
  }

  filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fdlg));
  gtk_widget_destroy(fdlg);
  if (filename==NULL)
    return;



  // connect to G4
  rv=g4_init_sys(&sysId,filename,NULL);
  g_free(filename);
  if (rv!=G4_ERROR_NONE){
    mb=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Error connecting to G4 Tracker\nError: %d",rv);
    gtk_dialog_run(GTK_DIALOG(mb));
    gtk_widget_destroy(mb);
    return;
  }


  gtk_widget_set_sensitive(w,FALSE);
  gtk_widget_set_sensitive(buttons[DNX_BUT],TRUE);
  gtk_widget_set_sensitive(buttons[SINGLE_BUT],TRUE);
  gtk_widget_set_sensitive(buttons[START_BUT],TRUE);
  gConnected=1;

}


void OnDnx(GtkWidget* w,gpointer g){

  GtkWidget** buttons=(GtkWidget**)g;

  // disconnect from G4
  g4_close_tracker();

  gtk_widget_set_sensitive(w,FALSE);
  gtk_widget_set_sensitive(buttons[CNX_BUT],TRUE);
  gtk_widget_set_sensitive(buttons[SINGLE_BUT],FALSE);
  gtk_widget_set_sensitive(buttons[START_BUT],FALSE);
  gtk_widget_set_sensitive(buttons[STOP_BUT],FALSE);
  gConnected=0;

}

void OnSingle(GtkWidget* textview,gpointer w){

  GtkWidget* dlg;
  // get a list of actual hubs
  int hubs,res;
  G4_CMD_STRUCT cs;
  FILEINFO* pfi=(FILEINFO*)g_object_get_data(G_OBJECT(w),"file");
  cs.cmd=G4_CMD_GET_ACTIVE_HUBS;
  cs.cds.id=G4_CREATE_ID(sysId,0,0);
  cs.cds.action=G4_ACTION_GET;
  cs.cds.pParam=NULL;
  g4_set_query(&cs);
  hubs=cs.cds.iParam;

  if (hubs==0){
    dlg=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,
			       "No Hubs Detected");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
    return;
  }

  int* hubList=new int[hubs];

  cs.cds.pParam=hubList;
  res=g4_set_query(&cs);
  G4_FRAMEDATA* fd=new G4_FRAMEDATA[hubs];




  res=g4_get_frame_data(fd,sysId,hubList,hubs);
  res&=0xffff;
  if (res)
    OutputData(textview,fd,hubs,pfi->f);

  delete[] fd;
  delete[] hubList;
}
int count1=0;
void OutputData(GtkWidget* textview,G4_FRAMEDATA* fd,int num,FILE* f){

  GtkTextBuffer* buffer;
  GtkTextIter iter;
  GtkTextMark* mark;
  char buf[500];
  int len;



  for (int i=0;i<num;i++){
    for (int a=0;a<G4_SENSORS_PER_HUB;a++){
      if (fd[i].stationMap & (0x01<<a)){
	if (!isQuats)
	  // {unsigned short fr=fd[i].sfd[a].pos[0]/39.37f*32767.0f/8.0f;
	  // sprintf(buf,"Hub %d, Sensor %d, %u --  %u  %.3f  %.3f  %.3f  %.3f  %.3f\n",fd[i].hub,a+1,
	  // 	  fd[i].frame,fr,fd[i].sfd[a].pos[1],fd[i].sfd[a].pos[2],fd[i].sfd[a].ori[0],
	  // 	  fd[i].sfd[a].ori[1],fd[i].sfd[a].ori[2]);}
	  len=sprintf(buf,"Hub %d, Sensor %d, %u --  %.3f  %.3f  %.3f  %.3f  %.3f  %.3f\n",fd[i].hub,a+1,
	  	  fd[i].frame,fd[i].sfd[a].pos[0],fd[i].sfd[a].pos[1],fd[i].sfd[a].pos[2],fd[i].sfd[a].ori[0],
	  	  fd[i].sfd[a].ori[1],fd[i].sfd[a].ori[2]);
	else
	  len=sprintf(buf,"Hub %d, Sensor %d, %u --  %.3f  %.3f  %.3f  %.3f  %.3f  %.3f  %.3f\n",fd[i].hub,a+1,
		  fd[i].frame, fd[i].sfd[a].pos[0],fd[i].sfd[a].pos[1],fd[i].sfd[a].pos[2],fd[i].sfd[a].ori[0],
		  fd[i].sfd[a].ori[1],fd[i].sfd[a].ori[2],fd[i].sfd[a].ori[3]);

	//write to display
	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	gtk_text_buffer_get_end_iter(buffer,&iter);
	gtk_text_buffer_insert(buffer,&iter,buf,-1);

	// scroll it down every 500 if cont
	if (!gRunCont || (!count1++%500==0)){
	mark=gtk_text_buffer_create_mark(buffer,"mark",&iter,TRUE);
	gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(textview),mark);
	gtk_text_buffer_delete_mark(buffer,mark);
	if (f)
	  fwrite(buf,1,len,f);
	}
      }// End if
    }// end a
  }// end i
}


void OnClear(GtkWidget* w,gpointer g){

  GtkTextView* text=GTK_TEXT_VIEW(g);
  GtkTextBuffer* buffer=gtk_text_view_get_buffer(text);
  gtk_text_buffer_set_text(buffer,"",-1);
}


void OnStartCont(GtkWidget* textview,gpointer g){

  GtkWidget* stopButton;
  // get hub list

  THREADINFO* pTi=new THREADINFO;
  FILEINFO* pfi=(FILEINFO*)g_object_get_data(G_OBJECT(g),"file");
  pTi->pp.InitPingPong(sizeof(G4_FRAMEDATA)*50);
  pTi->text=textview;
  pTi->fCap=pfi->f;

  stopButton=GTK_WIDGET(g_object_get_data(G_OBJECT(g),"stop_but"));
  gtk_widget_set_sensitive(GTK_WIDGET(g),FALSE);
  gtk_widget_set_sensitive(stopButton,TRUE);




  gRunCont=1;
  pthread_create(&contThread,NULL,ContThreadFxn,&pTi->pp);

  g_timeout_add(4,(GSourceFunc)GetContData,pTi);
  //g_idle_add((GSourceFunc)GetContData,pTi);




}


void OnStopCont(GtkWidget* w,gpointer g){

  GtkWidget* start=GTK_WIDGET(g);
  gRunCont=0;
  pthread_join(contThread,NULL);

  gtk_widget_set_sensitive(w,FALSE);
  gtk_widget_set_sensitive(start,TRUE);

}

#include <unistd.h>
void* ContThreadFxn(void* p){

  PingPong* pPP=(PingPong*)p;
  struct timespec ts,tr;
  G4_CMD_STRUCT cs;
  int num_hubs;
  G4_FRAMEDATA* fd;
  int* hubList;
  int actHubsRead;

  cs.cmd=G4_CMD_GET_ACTIVE_HUBS;
  cs.cds.id=G4_CREATE_ID(sysId,0,0);
  cs.cds.action=G4_ACTION_GET;
  cs.cds.pParam=NULL;
  g4_set_query(&cs);


  num_hubs=cs.cds.iParam;
  if (num_hubs==0){
    gRunCont=0;
    return NULL;
  }
  hubList=new int[num_hubs];
  cs.cds.pParam=hubList;
  g4_set_query(&cs);


  fd=new G4_FRAMEDATA[num_hubs];
  int slRes;


  while (gRunCont){
    pthread_mutex_lock(&mutex);
    actHubsRead=g4_get_frame_data(fd,sysId,hubList,num_hubs);
    actHubsRead&=0xffff;	// just look at what's been read
    if (actHubsRead)
      pPP->WritePP((BYTE*)fd,sizeof(G4_FRAMEDATA)*actHubsRead);

    pthread_mutex_unlock(&mutex);

    ts.tv_sec=0;
    ts.tv_nsec=4000000;
    do {
      slRes=nanosleep(&ts,&tr);
      ts.tv_nsec=tr.tv_nsec;
    }while(slRes==-1);

  }

  delete[] fd;
  delete[] hubList;

  return NULL;
}

gboolean GetContData(gpointer g){

  THREADINFO* pTi=(THREADINFO*)g;
  G4_FRAMEDATA fd[10];// max 10 hubs
  int len;
  int fd_size=sizeof(G4_FRAMEDATA);
  int num_hubs;
  if (gRunCont){
    pthread_mutex_lock(&mutex);
    do {
      len=pTi->pp.ReadPP((BYTE*)fd);
      if (len){
	num_hubs=len/fd_size;
	OutputData(pTi->text,fd,num_hubs,pTi->fCap);
      }
    }while (len);
    pthread_mutex_unlock(&mutex);
  }

  return gRunCont?TRUE:FALSE;
}

void OnBoresight(GtkWidget* w,gpointer g){

  // set boresight to 0,0,0
  int bore=(int)(long)g;
  float boreAng[3]={0.0f,0.0f,0.0};
  G4_CMD_STRUCT cs;

  if (!gConnected)
    return;
  cs.cmd=G4_CMD_BORESIGHT;
  cs.cds.id=G4_CREATE_ID(sysId,-1,0);
  cs.cds.action=(bore==1)?G4_ACTION_SET:G4_ACTION_RESET;
  cs.cds.iParam=G4_TYPE_EULER_DEGREE;
  cs.cds.pParam=boreAng;
  g4_set_query(&cs);

}

void OnRotate(GtkWidget*,gpointer){

  float aer[3];
  G4_CMD_STRUCT cs;
  if (!gConnected)
    return;


  cs.cmd=G4_CMD_FOR_ROTATE;
  cs.cds.id=G4_CREATE_ID(sysId,0,0);
  cs.cds.action=G4_ACTION_GET;
  cs.cds.iParam=G4_TYPE_EULER_DEGREE;
  cs.cds.pParam=aer;
  g4_set_query(&cs);

  FrameDlg fdlg(FD_ROT,aer);
  int resp=fdlg.present_dlg();
  if (resp==GTK_RESPONSE_OK){
    fdlg.GetData(aer);
    cs.cds.action=G4_ACTION_SET;
    g4_set_query(&cs);

  }


}

void OnTranslate(GtkWidget* w,gpointer g){

  float xyz[3];
  G4_CMD_STRUCT cs;
  if (!gConnected)
    return;

  cs.cmd=G4_CMD_FOR_TRANSLATE;
  cs.cds.id=G4_CREATE_ID(sysId,0,0);
  cs.cds.action=G4_ACTION_GET;
  cs.cds.iParam=G4_TYPE_INCH;
  cs.cds.pParam=xyz;
  g4_set_query(&cs);

  FrameDlg dlg(FD_TRANS,xyz);
  int resp=dlg.present_dlg();
  if (resp==GTK_RESPONSE_OK){
    dlg.GetData(xyz);
    cs.cds.action=G4_ACTION_SET;
    g4_set_query(&cs);
  }


}

void OnFilter(GtkWidget*,gpointer){

  G4_CMD_STRUCT cs;
  float filt[2][4];

  if (!gConnected)
    return;

  // get current values and fill dialog
  cs.cmd=G4_CMD_FILTER;
  cs.cds.id=G4_CREATE_ID(sysId,0,0);// using hub 0 to fill dialog
  cs.cds.action=G4_ACTION_GET;
  cs.cds.iParam=G4_DATA_POS;
  cs.cds.pParam=filt[0];
  g4_set_query(&cs);

  cs.cds.iParam=G4_DATA_ORI;
  cs.cds.pParam=filt[1];
  g4_set_query(&cs);

  FilterDlg dlg(filt[0],filt[1]);
  int resp=dlg.present_dlg();

  if (resp==GTK_RESPONSE_OK){
    dlg.GetFilterData(filt[0],filt[1]);
    cs.cds.id=G4_CREATE_ID(sysId,-1,0);		// set for all hubs
    cs.cds.action=G4_ACTION_SET;
    g4_set_query(&cs);

    cs.cds.iParam=G4_DATA_POS;
    cs.cds.pParam=filt[0];
    g4_set_query(&cs);
  }

}

void OnIncr(GtkWidget* w,gpointer g){

  float incr[2];
  G4_CMD_STRUCT cs;

  cs.cmd=G4_CMD_INCREMENT;
  cs.cds.id=G4_CREATE_ID(sysId,-1,0);
  cs.cds.action=G4_ACTION_GET;
  cs.cds.iParam=G4_TYPE_INCH<<16|G4_TYPE_EULER_DEGREE;
  cs.cds.pParam=incr;

  g4_set_query(&cs);

  IncrDlg dlg(incr[0],incr[1]);
  int rv=dlg.present_dlg();
  if (rv==GTK_RESPONSE_OK){
    dlg.get_increments(incr[0],incr[1]);
    cs.cds.action=G4_ACTION_SET;
    g4_set_query(&cs);
  }

}


void OnUnits(GtkWidget* w,gpointer g){

  G4_CMD_STRUCT cs;
  int p_unit,o_unit;
  cs.cmd=G4_CMD_UNITS;
  cs.cds.id=G4_CREATE_ID(sysId,0,0);	// system wide
  cs.cds.action=G4_ACTION_GET;
  cs.cds.iParam=G4_DATA_POS;
  cs.cds.pParam=&p_unit;
  g4_set_query(&cs);

  cs.cds.iParam=G4_DATA_ORI;
  cs.cds.pParam=&o_unit;
  g4_set_query(&cs);

  UnitsDlg dlg(p_unit,o_unit);
  int resp=dlg.present_dlg();
  if (resp==GTK_RESPONSE_OK){
    dlg.GetUnits(p_unit,o_unit);
    cs.cds.action=G4_ACTION_SET;
    g4_set_query(&cs);
    cs.cds.iParam=G4_DATA_POS;
    cs.cds.pParam=&p_unit;
    g4_set_query(&cs);
    isQuats=o_unit==G4_TYPE_QUATERNION?1:0;
  }


}



void OnAbout(GtkWidget* w,gpointer g){

  const gchar* authors[]={"James C. Farr, Polhemus Inc.",NULL};

  gtk_show_about_dialog(GTK_WINDOW(g),"authors",authors,"comments",piterm_comment,"copyright","Copyright © Polhemus 2009-2013","program-name","G4Term","version",piterm_version,
  			"website","http://www.polhemus.com/",NULL);

}


void OnCapture(GtkWidget* w,gpointer g){

  GtkWidget* dlg;
  FILEINFO* pfi=(FILEINFO*)g;
  gchar* filename;
  int resp;

  if (pfi->f){
    dlg=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,
			   "Data is presently being captured to %s.\n Would you like to stop capturing now?",
			   pfi->filename);
    resp=gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
    if (resp==GTK_RESPONSE_YES){
      fclose(pfi->f);
      pfi->f=NULL;
    }
  }
  else {
    dlg=gtk_file_chooser_dialog_new("Select a Capture File",NULL,GTK_FILE_CHOOSER_ACTION_SAVE,
				    GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,GTK_STOCK_CANCEL,
				    GTK_RESPONSE_CANCEL,NULL);
    resp=gtk_dialog_run(GTK_DIALOG(dlg));
    if (resp==GTK_RESPONSE_ACCEPT){
      filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      strcpy(pfi->filename,filename);
      g_free(filename);
      pfi->f=fopen(pfi->filename,"a");
    }
    gtk_widget_destroy(dlg);

  }

}

void OnClose(GtkWidget* w,gpointer g){

    if (gConnected)
        g4_close_tracker();

    gtk_main_quit();

}
