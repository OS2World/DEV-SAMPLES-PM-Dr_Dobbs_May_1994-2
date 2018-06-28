#define INCL_WIN
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "prgrmr.ext"	/* Gpf environment prototypes */
#include "prgrmr.ids"	/* Gpf generated ID_s	      */
#include "prgrmr.h"	/* User Function prototypes   */

typedef char * STRING;
STRING myMakeStr(STRING);

/* MYRECORD type to implement a detail view in cnr */
typedef struct _MYRECORD {
   MINIRECORDCORE rec;
   STRING ProgName;
   STRING Project;
   STRING SchedPercent;
} MYRECORD, *PMYRECORD;

void UFCreateListBox(PGPFPARMS pGpfParms) {
   char ProgData[256];
   CNRINFO cnrInfo;         /* +to change a view */
   PMYRECORD pRec;          /* +a current record */
   RECORDINSERT recInsert;  /* +to insert a record */
   STRING ProgName,Project,SchedPercent;
   int iSchedPercent;
   FILE * ProgrammerFile = fopen("prgrmrs.dat","r");
   HWND cnr;
  /* Define the layout of the columns of the listbox           */
   UFDefineFields(pGpfParms);
   cnr = WinWindowFromID(pGpfParms->hwnd,ID_CONTPROGRAMMERS);
   if(ProgrammerFile == NULL) return;
  /* Let the user know this may take a second by changing  */
  /* the cursor to an hourglass.                           */
   WinSetPointer(HWND_DESKTOP,SPTR_WAIT);
   /* As long as there's more, read a record */
   while(fgets(ProgData,sizeof(ProgData),ProgrammerFile) ) {
      /* Break the record into its component fields */
      ProgName = strtok(myMakeStr(ProgData),",");
      Project  = strtok(NULL,",");
      SchedPercent = strtok(NULL,",");
      iSchedPercent = atoi(SchedPercent);
      /* + allocate a record */
      pRec = (PMYRECORD)WinSendMsg(cnr,CM_ALLOCRECORD,
	 MPFROMLONG(sizeof(MYRECORD)-sizeof(MINIRECORDCORE)),MPFROMLONG(1));
      /* Set appropriate icon */
      if     (iSchedPercent <= -10) pRec->rec.hptrIcon =
         WinLoadPointer(HWND_DESKTOP,NULLHANDLE,ID_FACEFROWN);
      else if(iSchedPercent >= 10) pRec->rec.hptrIcon =
         WinLoadPointer(HWND_DESKTOP,NULLHANDLE,ID_FACESMILE);
      else pRec->rec.hptrIcon =
         WinLoadPointer(HWND_DESKTOP,NULLHANDLE,ID_FACEOKAY);
      /* -+ Now load the data into the record */
      pRec->rec.pszIcon = ProgName;
      pRec->ProgName = ProgName;
      pRec->Project = Project;
      pRec->SchedPercent = SchedPercent;
      /* +Set perameters to insert the record */
      recInsert.cb                = sizeof(recInsert);
      recInsert.pRecordOrder      = (PRECORDCORE)CMA_END;
      recInsert.pRecordParent     = NULL;
      recInsert.fInvalidateRecord = FALSE;
      recInsert.zOrder            = CMA_TOP;
      recInsert.cRecordsInsert    = 1;
      /* -+Finally, load the Record into the container */
      WinSendMsg(cnr,CM_INSERTRECORD,MPFROMP(pRec),MPFROMP(&recInsert));
   } /* -+while(fgets(ProgData,...,ProgrammerFile) */
   fclose(ProgrammerFile);                       /* close file */
  /* Define the type of view in the container (just like the   */
  /* DRIVES object on the WPD desktop has Details, Icon, etc.  */
   cnrInfo.flWindowAttr = CV_DETAIL|CA_DETAILSVIEWTITLES;
   WinSendMsg(cnr,CM_SETCNRINFO,MPFROMP(&cnrInfo),
      MPFROMLONG(CMA_FLWINDOWATTR));
}

void UFDefineFields(PGPFPARMS pGpfParms) {
   PFIELDINFO pf, pfo;
   FIELDINFOINSERT fInsert;
   HWND cnr;

   cnr = WinWindowFromID(pGpfParms->hwnd,ID_CONTPROGRAMMERS);
   /* +Alloc detail info for 3 fields */
   pfo = (PFIELDINFO)WinSendMsg(cnr,CM_ALLOCDETAILFIELDINFO,
				MPFROMLONG(3),(MPARAM)0 );
   pf = pfo;
  /* Set parameters for Programmer Name */
   pf->flData = CFA_STRING|CFA_SEPARATOR; /* Default left justify */
   pf->flTitle = CFA_CENTER;              /* -+Center title */
   pf->pTitleData = "Programmer";
   pf->offStruct = FIELDOFFSET(MYRECORD,ProgName);
   pf = pf->pNextFieldInfo;
  /* Set parameters for Project Title */
   pf->flData = CFA_STRING|CFA_SEPARATOR;
   pf->flTitle = CFA_CENTER;
   pf->pTitleData = "Project";
   pf->offStruct = FIELDOFFSET(MYRECORD,Project);
   pf = pf->pNextFieldInfo;
  /* Set parameters for Schedule Percentage */
   pf->flData = CFA_STRING|CFA_SEPARATOR|CFA_CENTER;
   pf->flTitle = CFA_CENTER;
   pf->pTitleData = "% ahead of sched.";
   pf->offStruct = FIELDOFFSET(MYRECORD,SchedPercent);
   /* +fill  out the insertion info */
   fInsert.cb = sizeof(fInsert);
   fInsert.pFieldInfoOrder = (PFIELDINFO)CMA_END; /* insert to the end */
   fInsert.cFieldInfoInsert = 3;		  /* 3 columns */
   fInsert.fInvalidateFieldInfo = FALSE; /* do not invalidate immediately */
   WinSendMsg(cnr,CM_INSERTDETAILFIELDINFO,MPFROMP(pfo),MPFROMP(&fInsert));
}

void UFDlgOpen(char *filter, char *result) {
   FILEDLG fileDlg;
   memset((char*)&fileDlg,0,sizeof(FILEDLG));
   fileDlg.cbSize = sizeof(FILEDLG);
   fileDlg.fl = FDS_OPEN_DIALOG;
   strcpy(fileDlg.szFullFile,filter);
   WinFileDlg(HWND_DESKTOP,WinQueryFocus(HWND_DESKTOP),&fileDlg);
   strcpy(result,fileDlg.szFullFile);
}

void UFDlgSaveAs(char *filter, char *result) {
   FILEDLG fileDlg;
   memset((char*)&fileDlg,0,sizeof(FILEDLG));
   fileDlg.cbSize = sizeof(FILEDLG);
   fileDlg.fl = FDS_SAVEAS_DIALOG;
   strcpy(fileDlg.szFullFile,filter);
   WinFileDlg(HWND_DESKTOP,WinQueryFocus(HWND_DESKTOP),&fileDlg);
   strcpy(result,fileDlg.szFullFile);

}

STRING myMakeStr(STRING s) {
   STRING newString;
   if(s == NULL) return NULL;
   newString = (STRING)malloc(strlen(s) +1);
   if (newString != NULL) strcpy(newString, s);
   return newString;
}
