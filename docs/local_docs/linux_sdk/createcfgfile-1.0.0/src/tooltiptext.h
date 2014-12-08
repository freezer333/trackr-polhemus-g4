// tooltiptext.h

#ifndef TOOLTIPTEXT_H_
#define TOOLTIPTEXT_H_

enum {
  TT_SRC_CHKBOX,
  TT_PNO_ENTRY,
  TT_LD_CAL_BUTTON,
  TT_HEM_COMBO,
  TT_FLR_COMBO,
  TT_CAL_STATUS,
  TT_SRC_FREQ_LABEL,
  TT_OPEN_BUTTON,
  TT_CREATE_BUTTON,
  TT_POS_UNITS,
  TT_ATT_UNITS,
  TT_NUMBER_TIPS
};


const char* tooltip_text[TT_NUMBER_TIPS]={
  "Check this box to activate the controls on this line to enter data for a source.",
  "Enter the Position and Orientation of the sources in these boxes",
  "Select this button to specify which calibration file will be used for this source",
  "Use this control to select the proper starting hemisphere for a source.  This value is " \
  "relative to the source itself.",
  "Use this control to select a floor compensation model.",
  "This status indicates whether a calibration file has been selected for this particular source.",
  "This indicates which frequency has been loaded with the source calibration.",
  "Use this button to open an existing G4 Source Configuration File (*.g4c).",
  "Use this button to create a new G4 Source Configuration File (*.g4c) using the data shown.",
  "Use these buttons to select the proper units for the source position data.",
  "Use these buttons to select the proper units for the source orientation data."
};



#endif
