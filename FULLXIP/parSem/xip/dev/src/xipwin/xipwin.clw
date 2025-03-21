; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xipwin.h"
LastPage=0

ClassCount=25
Class1=BuildingRule
Class2=CChildFrame
Class3=CFeaturetree
Class4=filenameinput
Class5=CFiletree
Class6=helpscripting
Class7=Cinforule
Class8=Climit
Class9=CListeVariable
Class10=CMainFrame
Class11=CoptionsXip
Class12=CParametreFile
Class13=Cprintvar
Class14=CResdoc
Class15=Cresultat
Class16=RuleBuilding
Class17=CRuleDoc
Class18=CSaisieDoc
Class19=CSaisieView
Class20=CSearchstring
Class21=Cseerule
Class22=CXipwinApp
Class23=CAboutDlg
Class24=CXipwinDoc
Class25=CXipwinView

ResourceCount=16
Resource1=IDD_search
Resource2=IDD_Filenameinput
Resource3=IDR_XIPWINTYPE (English (U.S.))
Resource4=IDD_ListeVariables
Resource5=IDR_MAINFRAME (English (U.S.))
Resource6=IDD_ABOUTBOX (English (U.S.))
Resource7=IDD_OPTIONSXIP
Resource8=IDD_createrule
Resource9=IDD_Limit
Resource10=IDD_seerule
Resource11=IDD_inforule
Resource12=IDD_parameterfile
Resource13=IDD_featuretree
Resource14=IDD_variableaccess
Resource15=IDD_grammarfiletree
Resource16=IDD_help_script

[CLS:BuildingRule]
Type=0
BaseClass=CDialog
HeaderFile=BuildingRule.h
ImplementationFile=BuildingRule.cpp
LastObject=BuildingRule

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
LastObject=CChildFrame

[CLS:CFeaturetree]
Type=0
BaseClass=CDialog
HeaderFile=Featuretree.h
ImplementationFile=Featuretree.cpp
LastObject=CFeaturetree

[CLS:filenameinput]
Type=0
BaseClass=CDialog
HeaderFile=filenameinput.h
ImplementationFile=filenameinput.cpp

[CLS:CFiletree]
Type=0
BaseClass=CDialog
HeaderFile=Filetree.h
ImplementationFile=Filetree.cpp
LastObject=CFiletree

[CLS:helpscripting]
Type=0
BaseClass=CDialog
HeaderFile=helpscripting.h
ImplementationFile=helpscripting.cpp
LastObject=helpscripting

[CLS:Cinforule]
Type=0
BaseClass=CDialog
HeaderFile=inforule.h
ImplementationFile=inforule.cpp
LastObject=Cinforule

[CLS:Climit]
Type=0
BaseClass=CDialog
HeaderFile=limit.h
ImplementationFile=limit.cpp
LastObject=Climit

[CLS:CListeVariable]
Type=0
BaseClass=CDialog
HeaderFile=ListeVariable.h
ImplementationFile=ListeVariable.cpp
LastObject=CListeVariable

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
VirtualFilter=fWC
LastObject=ID_NTM_ONLY

[CLS:CoptionsXip]
Type=0
BaseClass=CDialog
HeaderFile=optionsXip.h
ImplementationFile=optionsXip.cpp

[CLS:CParametreFile]
Type=0
BaseClass=CDialog
HeaderFile=ParametreFile.h
ImplementationFile=ParametreFile.cpp

[CLS:Cprintvar]
Type=0
BaseClass=CDialog
HeaderFile=printvar.h
ImplementationFile=printvar.cpp

[CLS:CResdoc]
Type=0
BaseClass=CDocument
HeaderFile=Resdoc.h
ImplementationFile=Resdoc.cpp

[CLS:Cresultat]
Type=0
BaseClass=CScrollView
HeaderFile=resultat.h
ImplementationFile=resultat.cpp

[CLS:RuleBuilding]
Type=0
BaseClass=CFormView
HeaderFile=RuleBuilding.h
ImplementationFile=RuleBuilding.cpp

[CLS:CRuleDoc]
Type=0
BaseClass=CDocument
HeaderFile=ruledoc.h
ImplementationFile=ruledoc.cpp

[CLS:CSaisieDoc]
Type=0
BaseClass=CDocument
HeaderFile=saisiedoc.h
ImplementationFile=saisiedoc.cpp

[CLS:CSaisieView]
Type=0
BaseClass=CEditView
HeaderFile=saisieview.h
ImplementationFile=saisieview.cpp

[CLS:CSearchstring]
Type=0
BaseClass=CDialog
HeaderFile=Searchstring.h
ImplementationFile=Searchstring.cpp

[CLS:Cseerule]
Type=0
BaseClass=CDialog
HeaderFile=seerule.h
ImplementationFile=seerule.cpp

[CLS:CXipwinApp]
Type=0
BaseClass=CWinApp
HeaderFile=xipwin.h
ImplementationFile=xipwin.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=xipwin.cpp
ImplementationFile=xipwin.cpp
LastObject=CAboutDlg

[CLS:CXipwinDoc]
Type=0
BaseClass=CDocument
HeaderFile=xipwinDoc.h
ImplementationFile=xipwinDoc.cpp

[CLS:CXipwinView]
Type=0
BaseClass=CEditView
HeaderFile=xipwinView.h
ImplementationFile=xipwinView.cpp
LastObject=ID_FILE_OPENXMLFILE

[DLG:IDD_resultatanalyse]
Type=1
Class=BuildingRule

[DLG:IDD_featuretree]
Type=1
Class=CFeaturetree
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_TREEfeatures,SysTreeView32,1350631431
Control3=IDCANCEL,button,1342242816

[DLG:IDD_Filenameinput]
Type=1
Class=filenameinput
ControlCount=4
Control1=IDC_filename,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308352

[DLG:IDD_grammarfiletree]
Type=1
Class=CFiletree
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_pathfiletree,SysTreeView32,1350631559
Control4=IDC_newfile,button,1342242816
Control5=IDC_deleterulefile,button,1342242816
Control6=IDC_fileadding,button,1342242816

[DLG:IDD_help_script]
Type=1
Class=helpscripting
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_instruction,SysTreeView32,1350631431

[DLG:IDD_inforule]
Type=1
Class=Cinforule
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_ruletext,edit,1353779396
Control3=IDC_select,edit,1350631552
Control4=IDCANCEL,button,1342242817
Control5=IDC_LIST,SysListView32,1350631425

[DLG:IDD_Limit]
Type=1
Class=Climit
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT1,edit,1350631552
Control4=IDC_STATIC,static,1342308352

[DLG:IDD_ListeVariables]
Type=1
Class=CListeVariable
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_listevar,listbox,1352728833
Control3=IDC_LISTTYPES,listbox,1352728833
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_OPTIONSXIP]
Type=1
Class=CoptionsXip
ControlCount=22
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_lemme,button,1342373891
Control4=IDC_surface,button,1342373891
Control5=IDC_offset,button,1342242819
Control6=IDC_cetegories,button,1342242819
Control7=IDC_reduit,button,1342373891
Control8=IDC_full,button,1342373891
Control9=IDC_wordnumb,button,1342242819
Control10=IDC_sentence,button,1342242819
Control11=IDC_utf8,button,1342242819
Control12=IDC_byname,button,1342308361
Control13=IDC_bynode,button,1342308361
Control14=IDC_bycreation,button,1342308361
Control15=IDC_dependency,button,1342373891
Control16=IDC_maths,button,1342242819
Control17=IDC_rulenumber,button,1342242819
Control18=IDC_STATIC,button,1342308359
Control19=IDC_STATIC,button,1342308359
Control20=IDC_STATIC,button,1342177287
Control21=IDC_inpututf8,button,1342242819
Control22=IDC_detectlanguage,button,1342242819

[DLG:IDD_parameterfile]
Type=1
Class=CParametreFile
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_rule,edit,1350633604
Control3=IDC_load,button,1342242816
Control4=IDCANCEL,button,1342242817
Control5=IDC_listfilename,combobox,1344339970

[DLG:IDD_variableaccess]
Type=1
Class=Cprintvar
ControlCount=6
Control1=IDC_variablename,edit,1350631552
Control2=IDC_variablevalue,edit,1350631552
Control3=ID_printvalue,button,1342242817
Control4=IDOK,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_createrule]
Type=1
Class=RuleBuilding
ControlCount=59
Control1=IDC_dependencyrule,button,1342177289
Control2=IDC_sequencerule,button,1342177289
Control3=IDC_taggingrule,button,1342177289
Control4=IDC_idrule,button,1342177289
Control5=IDC_resultat,SysTreeView32,1350635815
Control6=IDC_valueselection,button,1342242816
Control7=IDC_EDIT1,edit,1353777348
Control8=IDC_STATIC,static,1342308352
Control9=IDC_dependencies,SysTreeView32,1350635815
Control10=IDC_focus,button,1342242816
Control11=IDC_clearfocus,button,1342242816
Control12=IDC_clearselection,button,1342242816
Control13=IDC_breakpoint,edit,1350631552
Control14=IDC_BUTTONRUN,button,1342242817
Control15=IDC_runbar,msctls_trackbar32,1342242840
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_WINDOWREADING,edit,1350631552
Control19=IDC_addone,button,1342242816
Control20=IDC_minusone,button,1342242816
Control21=IDC_info,button,1342242816
Control22=IDC_editrule,button,1342242816
Control23=IDC_addrule,button,1342242816
Control24=IDC_editonfocus,edit,1350631553
Control25=IDC_focusmessage,static,1342308352
Control26=IDC_depsize,edit,1350631552
Control27=IDC_toggledepcat,static,1342308352
Control28=IDC_STATICDEP,static,1342308352
Control29=IDC_catdep,edit,1350637700
Control30=IDC_leftcontext,button,1073807360
Control31=IDC_rightcontext,button,1073807360
Control32=IDC_selectrule,button,1073807360
Control33=IDC_headtagging,button,1073807360
Control34=IDC_ruletype,edit,1350633601
Control35=IDC_LISTSUBPART,listbox,1353777409
Control36=IDC_addinfile,button,1342242816
Control37=IDC_numsentence,edit,1350631552
Control38=IDC_STATIC,static,1342308352
Control39=IDC_nextsentence,button,1342242816
Control40=IDC_previoussentence,button,1342242816
Control41=IDC_renumber,button,1342242816
Control42=IDC_catdeplist,combobox,1344340226
Control43=IDC_clearrulegeneration,button,1342242816
Control44=IDC_featuredisplay,button,1342242816
Control45=IDC_termrule,button,1342177289
Control46=IDC_markingrule,button,1342177289
Control47=IDC_longestmatch,button,1342242819
Control48=IDC_lastwordindex,button,1342242819
Control49=IDC_numvar,button,1342242816
Control50=IDC_seefiles,button,1342242816
Control51=IDC_STATIC,button,1342177287
Control52=IDC_deleterule,button,1342242816
Control53=IDC_appliedrules,listbox,1352728833
Control54=IDC_STATIC,static,1342308353
Control55=IDC_STATIC,static,1342308352
Control56=IDC_Layernumber,edit,1350633601
Control57=IDC_outof,static,1342308352
Control58=IDC_ListeVariables,button,1342242816
Control59=IDC_seerules,button,1342242816

[DLG:IDD_search]
Type=1
Class=CSearchstring
ControlCount=3
Control1=IDC_searchstring,edit,1350631552
Control2=IDC_find,button,1342242817
Control3=IDC_findnext,button,1342242816

[DLG:IDD_seerule]
Type=1
Class=Cseerule
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_rulenumber,edit,1350631552
Control5=IDC_limitlast,edit,1350631552

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_BUTTON32772
Command4=ID_XML_INPUT
Command5=ID_NTM_ONLY
Command6=ID_BUTTON32774
Command7=ID_BUTTON32775
Command8=ID_BUTTON32779
Command9=ID_BUTTONXML
Command10=ID_BUTTONTXT
Command11=id_createrule
CommandCount=11

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_PRINT_SETUP
Command2=ID_FILE_MRU_FILE1
Command3=ID_APP_EXIT
Command4=ID_VIEW_TOOLBAR
Command5=ID_VIEW_STATUS_BAR
Command6=ID_APP_ABOUT
CommandCount=6

[MNU:IDR_XIPWINTYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_ANALYZIS_RELOADGRAMMAR
Command3=ID_loadanothergrammar
Command4=ID_FILE_SAVE
Command5=ID_APP_EXIT
Command6=ID_ANALYSE_PARSE
Command7=ID_BUTTON32779
Command8=ID_PARSING_PARSETEXTTOXML
Command9=ID_PARSING_PARSEFILE
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_EDIT_OPTIONS
Command14=ID_SEARCHINRULE
Command15=ID_SEARCHAGAIN
Command16=ID_FINDRULENUMBER
Command17=id_createrule
Command18=IDC_editrule
Command19=IDC_seefiles
Command20=IDC_info
Command21=IDC_featuredisplay
Command22=ID_BUTTON32774
Command23=ID_BUTTON32775
Command24=ID_VIEW_GRAMMAR1
Command25=ID_VIEW_GRAMMAR2
Command26=ID_VIEW_GRAMMAR3
Command27=ID_VIEW_GRAMMAR4
Command28=ID_APP_ABOUT
Command29=id_help_scripting
CommandCount=29

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_SEARCHINRULE
Command3=ID_BUTTON32774
Command4=ID_FINDRULENUMBER
Command5=ID_BUTTON32779
Command6=ID_FILE_NEW
Command7=ID_FILE_OPEN
Command8=ID_ANALYSE_PARSE
Command9=ID_ANALYZIS_RELOADGRAMMAR
Command10=ID_FILE_SAVE
Command11=ID_BUTTON32775
Command12=id_createrule
Command13=ID_EDIT_PASTE
Command14=ID_EDIT_UNDO
Command15=ID_EDIT_CUT
Command16=ID_SEARCHAGAIN
Command17=ID_NEXT_PANE
Command18=ID_PREV_PANE
Command19=ID_EDIT_COPY
Command20=ID_EDIT_PASTE
Command21=ID_EDIT_CUT
Command22=ID_EDIT_UNDO
CommandCount=22

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_ntmversion,static,1350696960
Control8=IDC_whoami,static,1350697473

