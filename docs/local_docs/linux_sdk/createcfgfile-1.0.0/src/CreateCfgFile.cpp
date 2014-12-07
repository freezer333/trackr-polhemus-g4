    /*  CreateCfgFile.cpp */

#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include "G4SrcCfgIncl.h"
#include "tooltiptext.h"
#include "Quaternion.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>


    static void cb_checkbox(GtkToggleButton*,gpointer);
    static void cb_open_cfg_file(GtkButton*,gpointer);
    static void cb_ld_attr(GtkButton*,gpointer);
    static void cb_create_cfg_file(GtkButton*,gpointer);
    static void cb_about(GtkButton*,gpointer);
    static void cb_pos_unit_chg(GtkToggleButton*,gpointer);
    static void cb_att_unit_chg(GtkToggleButton*,gpointer);


    gchar* browse4file();

    const float IN2MET=0.02540f;
    const char* app_version=VERSION;

    typedef struct _CONTROL_STRUCT {
      GtkWidget* chkSrcButton;
      GtkWidget* srcPno[7];
      GtkWidget* AttrLdButton;
      GtkWidget* hemCombo;
      GtkWidget* floorCompCombo;
      GtkWidget* AttrLoaded;
      GtkWidget* srcFreq;
    }*LPCONTROL_STRUCT,CONTROL_STRUCT;


    typedef struct _UNITS_STRUCT {
      LPCONTROL_STRUCT pcs;
      GtkWidget* label;
      int* patt_units;
      int* ppos_units;
    }*LPUNITS_STRUCT,UNITS_STRUCT;

    static void update_pos_vals(int,int,LPCONTROL_STRUCT);
    static void update_att_vals(int,int,LPCONTROL_STRUCT);

    const char* hemLab[6]={"POS X","POS Y","POS Z","NEG X","NEG Y","NEG Z"};
    const char freqAlphaLab[MAX_SOURCES]={'A','B','C','D','E','F','G','H'};
    const char* flrLab[4]={"None","50 cm","75 cm","100 cm"};



    int main(int argc,char* argv[]){

      CONTROL_STRUCT cs[MAX_SOURCES];
      UNITS_STRUCT us;
      G4_SRC_DATA_STRUCT sds;

      GtkWidget *win,*table,*openButton,*create,*close;
      GtkWidget *chkLab,*ldLab,*pnoLab,*hemLabel,*flrLabel,*freqLab;
      GtkWidget* openImage,*saveImage,*about,*hsep,*vsep,*spacer;
      GtkWidget* bottom_hbox,*but_table,*pos_table,*ori_box,*vbox,*unit_table;
      GtkWidget *inches,*feet,*cm,*meters,*deg,*rads,*quats,*pos_frame,*ori_frame;

      int pos_units,att_units;

      int i,j,a;


      gtk_init(&argc,&argv);

      pos_units=G4_SRC_CFG_INCH;
      att_units=G4_SRC_CFG_EULER_DEGREE;

      g_object_set(gtk_settings_get_default(),"gtk_button_images",TRUE,NULL);  /*show images*/

      win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title(GTK_WINDOW(win),"Create G4 Source Configuration File");
      gtk_widget_set_size_request(win,1100,600);
        gtk_container_set_border_width(GTK_CONTAINER(win),5);
        gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_CENTER);
      //  gtk_window_set_default_icon_from_file("icon.png",NULL);

      table=gtk_grid_new();
      gtk_grid_set_column_homogeneous(GTK_GRID(table),FALSE);
      gtk_grid_set_row_spacing(GTK_GRID(table),5);
       /* create, add labels */
      chkLab=gtk_label_new("System\nSources");
      pnoLab=gtk_label_new("Location X Y Z A E R (inches and degrees)");
      hemLabel=gtk_label_new("Startup\nHemisphere");
      flrLabel=gtk_label_new("Floor\nCompensation");
      ldLab=gtk_label_new("Calibration\nStatus");
      freqLab=gtk_label_new("Source\nFreq");
      gtk_grid_attach(GTK_GRID(table),chkLab,0,0,1,1);
      gtk_grid_attach(GTK_GRID(table),pnoLab,1,0,7,1);
      gtk_grid_attach(GTK_GRID(table),hemLabel,10,0,2,1);
      gtk_grid_attach(GTK_GRID(table),flrLabel,12,0,2,1);
      gtk_grid_attach(GTK_GRID(table),ldLab,14,0,2,1);
      gtk_grid_attach(GTK_GRID(table),freqLab,16,0,1,1);
      gtk_grid_set_column_spacing(GTK_GRID(table),10);


       /* create, add ui widgets */
      for (i=0;i<MAX_SOURCES;i++){
        cs[i].chkSrcButton=gtk_check_button_new_with_label(NULL);
        gtk_grid_attach(GTK_GRID(table),cs[i].chkSrcButton,0,i+1,1,1);
        g_object_set_data(G_OBJECT(cs[i].chkSrcButton),"att_units",&att_units);
        gtk_widget_set_tooltip_text(cs[i].chkSrcButton,tooltip_text[TT_SRC_CHKBOX]);
        for (j=0;j<7;j++){
          cs[i].srcPno[j]=gtk_entry_new();
            gtk_entry_set_width_chars(GTK_ENTRY(cs[i].srcPno[j]),8);
          gtk_grid_attach(GTK_GRID(table),cs[i].srcPno[j],j+1,i+1,1,1);
          gtk_widget_set_sensitive(cs[i].srcPno[j],FALSE);
          gtk_widget_set_tooltip_text(cs[i].srcPno[j],tooltip_text[TT_PNO_ENTRY]);
        }
        cs[i].AttrLdButton=gtk_button_new_with_label("Ld Src Calib...");
        gtk_grid_attach(GTK_GRID(table),cs[i].AttrLdButton,8,i+1,2,1);
        gtk_widget_set_sensitive(cs[i].AttrLdButton,FALSE);
        g_object_set_data(G_OBJECT(cs[i].AttrLdButton),"src_data_struct",&sds);
        g_object_set_data(G_OBJECT(cs[i].AttrLdButton),"if_data",&cs[i]);
        gtk_widget_set_tooltip_text(cs[i].AttrLdButton,tooltip_text[TT_LD_CAL_BUTTON]);
        cs[i].hemCombo=gtk_combo_box_text_new();
        for (a=0;a<6;a++)
          gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cs[i].hemCombo),hemLab[a]);
        gtk_combo_box_set_active(GTK_COMBO_BOX(cs[i].hemCombo),0);
        gtk_widget_set_sensitive(cs[i].hemCombo,FALSE);
        gtk_widget_set_tooltip_text(cs[i].hemCombo,tooltip_text[TT_HEM_COMBO]);
        gtk_grid_attach(GTK_GRID(table),cs[i].hemCombo,10,i+1,2,1);
        cs[i].floorCompCombo=gtk_combo_box_text_new();

        for (a=0;a<4;a++)
          gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cs[i].floorCompCombo),flrLab[a]);
        gtk_combo_box_set_active(GTK_COMBO_BOX(cs[i].floorCompCombo),0);
        gtk_widget_set_sensitive(cs[i].floorCompCombo,FALSE);
        gtk_widget_set_tooltip_text(cs[i].floorCompCombo,tooltip_text[TT_FLR_COMBO]);
        gtk_grid_attach(GTK_GRID(table),cs[i].floorCompCombo,12,i+1,2,1);
        cs[i].AttrLoaded=gtk_label_new("Not Loaded");
        gtk_widget_set_tooltip_text(cs[i].AttrLoaded,tooltip_text[TT_CAL_STATUS]);
        gtk_grid_attach(GTK_GRID(table),cs[i].AttrLoaded,14,i+1,2,1);
        gtk_widget_set_sensitive(cs[i].AttrLoaded,FALSE);
        cs[i].srcFreq=gtk_label_new("N/A");
        gtk_widget_set_tooltip_text(cs[i].srcFreq,tooltip_text[TT_SRC_FREQ_LABEL]);
        gtk_grid_attach(GTK_GRID(table),cs[i].srcFreq,16,i+1,2,1);


        g_signal_connect(cs[i].chkSrcButton,"toggled",G_CALLBACK(cb_checkbox),cs+i);
        g_signal_connect(cs[i].AttrLdButton,"clicked",G_CALLBACK(cb_ld_attr),(gpointer)(long)i);
      }

      hsep=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
      gtk_grid_attach(GTK_GRID(table),hsep,0,9,17,1);

      openButton=gtk_button_new_with_label("Open Existing Configuration File...");
      openImage=gtk_image_new_from_stock(GTK_STOCK_OPEN,GTK_ICON_SIZE_BUTTON);
      gtk_widget_set_tooltip_text(openButton,tooltip_text[TT_OPEN_BUTTON]);
      gtk_button_set_image(GTK_BUTTON(openButton),openImage);
      g_object_set_data(G_OBJECT(openButton),"src_data_struct",&sds);
      g_object_set_data(G_OBJECT(openButton),"pos_units",&pos_units);
      g_object_set_data(G_OBJECT(openButton),"att_units",&att_units);
      create=gtk_button_new_with_label("Create Source Configuration File...");
      saveImage=gtk_image_new_from_stock(GTK_STOCK_SAVE,GTK_ICON_SIZE_BUTTON);
      gtk_button_set_image(GTK_BUTTON(create),saveImage);
      gtk_widget_set_tooltip_text(create,tooltip_text[TT_CREATE_BUTTON]);
      g_object_set_data(G_OBJECT(create),"src_data_struct",&sds);
      g_object_set_data(G_OBJECT(create),"pos_units",&pos_units);
      g_object_set_data(G_OBJECT(create),"att_units",&att_units);
      close=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
      about=gtk_button_new_from_stock(GTK_STOCK_ABOUT);

      vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);	// overall container

      bottom_hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
      but_table=gtk_grid_new();
      gtk_grid_set_column_spacing(GTK_GRID(but_table),10);
      gtk_grid_set_row_spacing(GTK_GRID(but_table),10);
      gtk_container_set_border_width(GTK_CONTAINER(but_table),50);
      gtk_grid_attach(GTK_GRID(but_table),openButton,0,1,4,2);
      gtk_grid_attach(GTK_GRID(but_table),create,0,3,4,2);
      gtk_grid_attach(GTK_GRID(but_table),close,0,5,2,2);
      gtk_grid_attach(GTK_GRID(but_table),about,2,5,2,2);
      spacer=gtk_label_new(NULL);
      gtk_box_pack_start(GTK_BOX(bottom_hbox),spacer,FALSE,TRUE,20);
      gtk_box_pack_start(GTK_BOX(bottom_hbox),but_table,FALSE,TRUE,0);
      vsep=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
      gtk_box_pack_start(GTK_BOX(bottom_hbox),vsep,TRUE,TRUE,0);

      // radio buttons
      inches=gtk_radio_button_new_with_label_from_widget(NULL,"inches");
      feet=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(inches),"feet");
      cm=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(inches),"cm");
      meters=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(inches),"meters");

      g_object_set_data(G_OBJECT(inches),"pos_unit",(gpointer)(long)G4_SRC_CFG_INCH);
      g_object_set_data(G_OBJECT(feet),"pos_unit",(gpointer)(long)G4_SRC_CFG_FOOT);
      g_object_set_data(G_OBJECT(cm),"pos_unit",(gpointer)(long)G4_SRC_CFG_CM);
      g_object_set_data(G_OBJECT(meters),"pos_unit",(gpointer)(long)G4_SRC_CFG_METER);

      g_object_set_data(G_OBJECT(inches),"c_struct",(gpointer)&cs);
      g_object_set_data(G_OBJECT(feet),"c_struct",(gpointer)&cs);
      g_object_set_data(G_OBJECT(cm),"c_struct",(gpointer)&cs);
      g_object_set_data(G_OBJECT(meters),"c_struct",(gpointer)&cs);

      deg=gtk_radio_button_new_with_label_from_widget(NULL,"Euler Degrees");
      rads=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(deg),"Euler Radians");
      quats=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(deg),"Quaternions");

      g_object_set_data(G_OBJECT(deg),"att_unit",(gpointer)(long)G4_SRC_CFG_EULER_DEGREE);
      g_object_set_data(G_OBJECT(rads),"att_unit",(gpointer)(long)G4_SRC_CFG_EULER_RADIAN);
      g_object_set_data(G_OBJECT(quats),"att_unit",(gpointer)(long)G4_SRC_CFG_QUATERNION);

      g_object_set_data(G_OBJECT(deg),"c_struct",(gpointer)&cs);
      g_object_set_data(G_OBJECT(rads),"c_struct",(gpointer)&cs);
      g_object_set_data(G_OBJECT(quats),"c_struct",(gpointer)&cs);


      unit_table=gtk_grid_new();
      gtk_grid_set_column_homogeneous(GTK_GRID(unit_table),FALSE);
      gtk_grid_set_column_spacing(GTK_GRID(unit_table),40);
      gtk_container_set_border_width(GTK_CONTAINER(unit_table),50);

      pos_table=gtk_grid_new();
      gtk_grid_attach(GTK_GRID(pos_table),inches,0,0,1,1);
      gtk_grid_attach(GTK_GRID(pos_table),feet,0,1,1,1);
      gtk_grid_attach(GTK_GRID(pos_table),cm,1,0,1,1);
      gtk_grid_attach(GTK_GRID(pos_table),meters,1,1,1,1);
      pos_frame=gtk_frame_new("Position Units");
      gtk_widget_set_tooltip_text(pos_frame,tooltip_text[TT_POS_UNITS]);
      gtk_container_set_border_width(GTK_CONTAINER(pos_table),10);
      gtk_container_add(GTK_CONTAINER(pos_frame),pos_table);
      gtk_grid_attach(GTK_GRID(unit_table),pos_frame,0,1,1,2);


      ori_box=gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
      gtk_container_add(GTK_CONTAINER(ori_box),deg);
      gtk_container_add(GTK_CONTAINER(ori_box),rads);
      gtk_container_add(GTK_CONTAINER(ori_box),quats);
      ori_frame=gtk_frame_new("Orientation Units");
      gtk_widget_set_tooltip_text(ori_frame,tooltip_text[TT_ATT_UNITS]);
      gtk_container_set_border_width(GTK_CONTAINER(ori_box),10);
      gtk_container_add(GTK_CONTAINER(ori_frame),ori_box);
      gtk_grid_attach(GTK_GRID(unit_table),ori_frame,2,1,3,2);


      gtk_box_pack_start(GTK_BOX(bottom_hbox),unit_table,FALSE,FALSE,2);


      gtk_box_pack_start(GTK_BOX(vbox),table,TRUE,TRUE,0);
      gtk_box_pack_start(GTK_BOX(vbox),bottom_hbox,TRUE,TRUE,0);

      gtk_container_add(GTK_CONTAINER(win),vbox);

      us.pcs=cs;
      us.label=pnoLab;
      us.patt_units=&att_units;
      us.ppos_units=&pos_units;


      g_signal_connect(win,"destroy",G_CALLBACK(gtk_main_quit),NULL);
      g_signal_connect(close,"clicked",G_CALLBACK(gtk_main_quit),NULL);
      g_signal_connect(openButton,"clicked",G_CALLBACK(cb_open_cfg_file),cs);
      g_signal_connect(create,"clicked",G_CALLBACK(cb_create_cfg_file),cs);
      g_signal_connect(about,"clicked",G_CALLBACK(cb_about),NULL);

      g_signal_connect(inches,"toggled",G_CALLBACK(cb_pos_unit_chg),&us);
      g_signal_connect(feet,"toggled",G_CALLBACK(cb_pos_unit_chg),&us);
      g_signal_connect(cm,"toggled",G_CALLBACK(cb_pos_unit_chg),&us);
      g_signal_connect(meters,"toggled",G_CALLBACK(cb_pos_unit_chg),&us);

      g_signal_connect(deg,"toggled",G_CALLBACK(cb_att_unit_chg),&us);
      g_signal_connect(rads,"toggled",G_CALLBACK(cb_att_unit_chg),&us);
      g_signal_connect(quats,"toggled",G_CALLBACK(cb_att_unit_chg),&us);

      gtk_widget_show_all(win);

      gtk_main();

      return 0;


    }


    void cb_checkbox(GtkToggleButton* cb,gpointer data){

      int i;
      LPCONTROL_STRUCT pcs=(LPCONTROL_STRUCT)data;
      gboolean ckd=gtk_toggle_button_get_active(cb);
      int* patt_units=(int*)g_object_get_data(G_OBJECT(cb),"att_units");

      int num_vals=*patt_units==G4_SRC_CFG_QUATERNION?7:6;

      for (i=0;i<num_vals;i++){
        gtk_widget_set_sensitive(pcs->srcPno[i],ckd);
        gtk_entry_set_text(GTK_ENTRY(pcs->srcPno[i]),ckd?"0.00":"");
      }
      gtk_widget_set_sensitive(pcs->AttrLdButton,ckd);
      gtk_widget_set_sensitive(pcs->AttrLoaded,ckd);
      gtk_widget_set_sensitive(pcs->srcFreq,ckd);
      gtk_widget_set_sensitive(pcs->hemCombo,ckd);
      gtk_widget_set_sensitive(pcs->floorCompCombo,ckd);

    }

    void cb_open_cfg_file(GtkButton* b,gpointer p){

      LPCONTROL_STRUCT pcs=(LPCONTROL_STRUCT)p;
      LPG4_SRC_DATA_STRUCT pdata=(LPG4_SRC_DATA_STRUCT)g_object_get_data(G_OBJECT(b),"src_data_struct");
      int rv,i,pos,len,a;
      GtkWidget* dlg;
      gchar* filename;
      gchar str[100];
      GtkFileFilter *all,*g4c;
      int *ppos_units,*patt_units;
      int num_vals;
      char fstr[5];

      ppos_units=(int*)g_object_get_data(G_OBJECT(b),"pos_units");
      patt_units=(int*)g_object_get_data(G_OBJECT(b),"att_units");

      if (*patt_units==G4_SRC_CFG_QUATERNION){
        num_vals=7;
        strcpy(fstr,"%.4f");
      }
      else {
        num_vals=6;
        strcpy(fstr,"%.3f");
      }

      // get file to read
      all=gtk_file_filter_new();
      g4c=gtk_file_filter_new();
      gtk_file_filter_set_name(all,"All Files");
      gtk_file_filter_set_name(g4c,"G4 Source Configuration Files (*.g4c)");
      gtk_file_filter_add_pattern(all,"*");
      gtk_file_filter_add_pattern(g4c,"*.g4c");
      dlg=gtk_file_chooser_dialog_new("Select a Configuration File",NULL,
                      GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,
                      GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);

      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg),g4c);
      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg),all);
      rv=gtk_dialog_run(GTK_DIALOG(dlg));
      if (rv==GTK_RESPONSE_CANCEL){
        gtk_widget_destroy(dlg);
        return;
      }

      filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      gtk_widget_destroy(dlg);



      // read the file
      rv=g4_read_source_cfg_file(*ppos_units,*patt_units,pdata,filename);
      if (rv){
        dlg=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,
                       "Error reading Source Configuration File: %s",filename);
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return;
      }
      g_free(filename);

      // update the interface
      for (i=0;i<MAX_SOURCES;i++){
        if (pdata->sourceMap & (0x01<<i)){
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pcs[i].chkSrcButton),TRUE);
          pos=0;
          for (a=0;a<num_vals;a++){
        len=g_sprintf(str,a<3?"%.4f":fstr,pdata->pos_ori[i][a]);
        gtk_editable_delete_text(GTK_EDITABLE(pcs[i].srcPno[a]),0,-1);
        gtk_editable_insert_text(GTK_EDITABLE(pcs[i].srcPno[a]),str,len,&pos);
          }
          gtk_combo_box_set_active(GTK_COMBO_BOX(pcs[i].hemCombo),pdata->start_hem[i]&0x07);
          gtk_combo_box_set_active(GTK_COMBO_BOX(pcs[i].floorCompCombo),(pdata->start_hem[i]>>4)&0x07);
          gtk_label_set_text(GTK_LABEL(pcs[i].AttrLoaded),"Loaded");
          g_sprintf(str,"Freq %c",freqAlphaLab[pdata->freq[i]&0xff]);
          gtk_label_set_text(GTK_LABEL(pcs[i].srcFreq),str);


        }
        else{
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pcs[i].chkSrcButton),FALSE);
          for (a=0;a<num_vals;a++)
        gtk_editable_delete_text(GTK_EDITABLE(pcs[i].srcPno[a]),0,-1);
          gtk_label_set_text(GTK_LABEL(pcs[i].AttrLoaded),"Not Loaded");
          gtk_label_set_text(GTK_LABEL(pcs[i].srcFreq),"N/A");


        }
      }

    }

    void cb_ld_attr(GtkButton* b,gpointer p){

      LPCONTROL_STRUCT pcs=(LPCONTROL_STRUCT)g_object_get_data(G_OBJECT(b),"if_data");
      LPG4_SRC_DATA_STRUCT pdata=(LPG4_SRC_DATA_STRUCT)g_object_get_data(G_OBJECT(b),"src_data_struct");
      int index=(int)(long)p;
      GtkWidget* dlg;
      GtkFileFilter *all,*g4s;
      int rv;
      gchar* filename;
      FILE* f;
      int freq;
      gchar str[20];

      dlg=gtk_file_chooser_dialog_new("Choose the G4S file for this source",NULL,
                      GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,
                      GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
      g4s=gtk_file_filter_new();
      gtk_file_filter_set_name(g4s,"G4 Attribute Files (*.g4s)");
      gtk_file_filter_add_pattern(g4s,"*.g4s");
      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg),g4s);

      all=gtk_file_filter_new();
      gtk_file_filter_set_name(all,"All Files");
      gtk_file_filter_add_pattern(all,"*");
      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg),all);

      rv=gtk_dialog_run(GTK_DIALOG(dlg));
      if (rv==GTK_RESPONSE_CANCEL){
        gtk_widget_destroy(dlg);
        return;
      }

      filename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      gtk_widget_destroy(dlg);
      f=fopen(filename,"r");
      if (!f){
        dlg=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,
                       "Error opening file: %s",filename);
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return;
      }
      g_free(filename);

      rv=fread(pdata->attr[index],1,ATTR_LEN,f);
      fclose(f);

      if (rv!=ATTR_LEN){
        dlg=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,
                       "Error reading file: %s",filename);
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return;
      }

      gtk_label_set_text(GTK_LABEL(pcs->AttrLoaded),"Loaded");
      freq=g4_get_freq_sn_from_attr(pdata->attr[index],pdata->id[index]);
      g_sprintf(str,"Freq %c",freqAlphaLab[freq]);
      gtk_label_set_text(GTK_LABEL(pcs->srcFreq),str);
      pdata->freq[index]=freq&0xff;


    }

    #include <string.h>
    void cb_create_cfg_file(GtkButton* b,gpointer p){

      LPCONTROL_STRUCT pcs=(LPCONTROL_STRUCT)p;
      LPG4_SRC_DATA_STRUCT pdata=(LPG4_SRC_DATA_STRUCT)g_object_get_data(G_OBJECT(b),"src_data_struct");
      int rv;
      GtkWidget* dlg;
      int i,a;
      const gchar* buf;
      gchar* file;
      int num_vals;
      int *ppos_units,*patt_units;


      file=browse4file();
      if (file==NULL)
        return;

      ppos_units=(int*)g_object_get_data(G_OBJECT(b),"pos_units");
      patt_units=(int*)g_object_get_data(G_OBJECT(b),"att_units");

      num_vals=*patt_units==G4_SRC_CFG_QUATERNION?7:6;

      // set source map and start-up hem
      pdata->sourceMap=0;
      for (i=0;i<MAX_SOURCES;i++){
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pcs[i].chkSrcButton))){
           pdata->sourceMap|=(0x01<<i);
          // load up the pno
           for (a=0;a<num_vals;a++){
         buf=gtk_entry_get_text(GTK_ENTRY(pcs[i].srcPno[a]));
         pdata->pos_ori[i][a]=strtod(buf,NULL);
           }
          // start-up hemisphere
          pdata->start_hem[i]=gtk_combo_box_get_active(GTK_COMBO_BOX(pcs[i].hemCombo))&0x07;
          // floor comp
          pdata->start_hem[i]|=((gtk_combo_box_get_active(GTK_COMBO_BOX(pcs[i].floorCompCombo))&0x07)<<4);
        }
      }



      rv=g4_create_source_cfg_file(*ppos_units,*patt_units,pdata,file);

      if (rv)
        dlg=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,
                       "Failure creating source configuration file.  Error %d",rv);
      else
        dlg=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,
                       "Source Configuration File Created Successfully:\n%s",file);

      gtk_dialog_run(GTK_DIALOG(dlg));
      gtk_widget_destroy(dlg);
      g_free(file);

    }

    gchar* browse4file(){

      GtkWidget* dlg;
      int rv;
      gchar* file;
      gchar* ret_file=NULL;
      GtkFileFilter *all,*g4c;

      all=gtk_file_filter_new();
      g4c=gtk_file_filter_new();
      gtk_file_filter_set_name(all,"All Files");
      gtk_file_filter_set_name(g4c,"G4 Source Configuration Files (*.g4c)");
      gtk_file_filter_add_pattern(all,"*");
      gtk_file_filter_add_pattern(g4c,"*.g4c");
      dlg=gtk_file_chooser_dialog_new("Select a path and name for the Configuration file",NULL,
                      GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,
                      GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);

      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg),g4c);
      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg),all);
      gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dlg),TRUE);

      rv=gtk_dialog_run(GTK_DIALOG(dlg));
      if (rv==GTK_RESPONSE_ACCEPT){
        file=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        ret_file=g_strdup(file);
        g_free(file);
      }

      gtk_widget_destroy(dlg);
      return ret_file;

    }

    void cb_about(GtkButton* b,gpointer p){

      const gchar* author[]={"James C. Farr","<jimfarr@polhemus.com>",NULL};

      gtk_show_about_dialog(NULL,"program-name","Create G4 Src Cfg File","version",app_version,"website",
                "http://www.polhemus.com/","copyright","Copyright © 2010-2013 by Polhemus","comments",
                "Program to create G4 Source Configuration files",
                "authors",author,NULL);

    }


    void cb_pos_unit_chg(GtkToggleButton* b,gpointer g){

      LPUNITS_STRUCT pus=(LPUNITS_STRUCT)g;
      LPCONTROL_STRUCT lcs;
      int pos_unit;
      char label[100];
      char attLabel[50];

      if (gtk_toggle_button_get_active(b)){
        pos_unit=(int)(long)g_object_get_data(G_OBJECT(b),"pos_unit");
        lcs=(LPCONTROL_STRUCT)g_object_get_data(G_OBJECT(b),"c_struct");
        update_pos_vals(*pus->ppos_units,pos_unit,lcs);
        *pus->ppos_units=pos_unit;
        strcpy(label,"X Y Z ");

        if (*pus->patt_units==G4_SRC_CFG_QUATERNION){
          strcat(label,"q0 q1 q2 q3 ");
          strcpy(attLabel," quaternions)");
        }
        else {
          strcat(label,"A E R ");
          if (*pus->patt_units==G4_SRC_CFG_EULER_DEGREE)
        strcpy(attLabel," degrees)");
          else
        strcpy(attLabel," radians)");
        }

        switch (pos_unit){
        case G4_SRC_CFG_INCH:
          strcat(label,"(inches and");
          break;
        case G4_SRC_CFG_FOOT:
          strcat(label,"(feet and");
          break;
        case G4_SRC_CFG_CM:
          strcat(label,"(centimeters and");
          break;
        case G4_SRC_CFG_METER :
          strcat(label,"(meters and");
          break;
        }
        strcat(label,attLabel);

        gtk_label_set_text(GTK_LABEL(pus->label),label);
      }
    }


    void cb_att_unit_chg(GtkToggleButton* b,gpointer g){

      LPUNITS_STRUCT pus=(LPUNITS_STRUCT)g;
      int att_unit;
      char label[100];
      char attLabel[50];
      int i;
      LPCONTROL_STRUCT lcs;


      if (gtk_toggle_button_get_active(b)){
        att_unit=(int)(long)g_object_get_data(G_OBJECT(b),"att_unit");
        lcs=(LPCONTROL_STRUCT)g_object_get_data(G_OBJECT(b),"c_struct");
        update_att_vals(*pus->patt_units,att_unit,lcs);
        *pus->patt_units=att_unit;
        strcpy(label,"X Y Z ");

        if (att_unit==G4_SRC_CFG_QUATERNION){
          strcat(label,"q0 q1 q2 q3 ");
          strcpy(attLabel," quaternions)");
        }
        else {
          strcat(label,"A E R ");
          if (att_unit==G4_SRC_CFG_EULER_DEGREE)
        strcpy(attLabel," degrees)");
          else
        strcpy(attLabel," radians)");
        }

        switch (*pus->ppos_units){
        case G4_SRC_CFG_INCH:
          strcat(label,"(inches and");
          break;
        case G4_SRC_CFG_FOOT:
          strcat(label,"(feet and");
          break;
        case G4_SRC_CFG_CM:
          strcat(label,"(centimeters and");
          break;
        case G4_SRC_CFG_METER :
          strcat(label,"(meters and");
          break;
        }
        strcat(label,attLabel);

        gtk_label_set_text(GTK_LABEL(pus->label),label);

        // go thru and enable/disable 7th entry depending on att unit
        for (i=0;i<MAX_SOURCES;i++){
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pus->pcs[i].chkSrcButton))){
        if (att_unit==G4_SRC_CFG_QUATERNION)
          gtk_widget_set_sensitive(pus->pcs[i].srcPno[6],TRUE);
        else {
          gtk_editable_delete_text(GTK_EDITABLE(pus->pcs[i].srcPno[6]),0,-1);
          gtk_widget_set_sensitive(pus->pcs[i].srcPno[6],FALSE);
        }
          }
        }
      }

    }


    void update_pos_vals(int old_pos,int new_pos,LPCONTROL_STRUCT cs){

      float factor=1.0f;
      float f[MAX_SOURCES][3];

      int i,a;
      const gchar* text;
      gchar str[20];

      // first change to meters then to new val


      if (old_pos!=new_pos){

        switch (old_pos){
        case G4_SRC_CFG_INCH:
          factor=IN2MET;
          break;
        case G4_SRC_CFG_FOOT:
          factor=IN2MET*12.0f;
          break;
        case G4_SRC_CFG_CM:
          factor=0.01f;
        }

        for (i=0;i<MAX_SOURCES;i++){
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cs[i].chkSrcButton))){
        for (a=0;a<3;a++){
          text=gtk_entry_get_text(GTK_ENTRY(cs[i].srcPno[a]));
          f[i][a]=strtof(text,NULL);
          f[i][a]*=factor;
        }
          }
        }

        factor=1.0f;
        switch (new_pos){
        case G4_SRC_CFG_INCH:
          factor=1.0f/IN2MET;
          break;
        case G4_SRC_CFG_FOOT:
          factor=1.0f/IN2MET/12.0f;
          break;
        case G4_SRC_CFG_CM:
          factor=100.0f;
        }

        for (i=0;i<MAX_SOURCES;i++){
          if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cs[i].chkSrcButton))){
        for (a=0;a<3;a++){
          f[i][a]*=factor;
          sprintf(str,"%.4f",f[i][a]);
          gtk_entry_set_text(GTK_ENTRY(cs[i].srcPno[a]),str);
        }
          }
        }
      }
    }



    void update_att_vals(int old_att,int new_att,LPCONTROL_STRUCT cs){

      int i,a;
      int deg;
      float f[4];
      const gchar* text;
      char str[10];

      if (old_att!=new_att){

        CQuaternion q[MAX_SOURCES];

        // - first change to quaternion
        if (old_att==G4_SRC_CFG_QUATERNION){	// already quats

          for (i=0;i<MAX_SOURCES;i++){
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cs[i].chkSrcButton))){
          for (a=0;a<4;a++){
            text=gtk_entry_get_text(GTK_ENTRY(cs[i].srcPno[a+3]));
            f[a]=strtof(text,NULL);
          }
          q[i].SetQuatVals(f);
        }
          }
        }


        else {
          deg=(old_att==G4_SRC_CFG_EULER_DEGREE);
          for (i=0;i<MAX_SOURCES;i++){
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cs[i].chkSrcButton))){
          for (a=0;a<3;a++){
            text=gtk_entry_get_text(GTK_ENTRY(cs[i].srcPno[a+3]));
            f[a]=strtof(text,NULL);
          }
          q[i].SetFromEulers(f,deg);
        }
          }
        }

        // change to new format
        if (new_att==G4_SRC_CFG_QUATERNION){
          for (i=0;i<MAX_SOURCES;i++){
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cs[i].chkSrcButton))){
          q[i].GetQuatVal(f);
          for (a=0;a<4;a++){
            sprintf(str,"%.4f",f[a]);
            gtk_entry_set_text(GTK_ENTRY(cs[i].srcPno[a+3]),str);
          }
        }
          }
        }
        else {
          deg=(new_att==G4_SRC_CFG_EULER_DEGREE);
          for (i=0;i<MAX_SOURCES;i++){
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cs[i].chkSrcButton))){
          q[i].GetEuler(f,deg);
          for (a=0;a<3;a++){
            sprintf(str,"%.3f",f[a]);
            gtk_entry_set_text(GTK_ENTRY(cs[i].srcPno[a+3]),str);
          } // end for a
        } // end if
          }// end for i
        }// end else
      }// end if
    }



