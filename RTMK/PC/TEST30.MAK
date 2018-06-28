.AUTODEPEND

#		*Translator Definitions*
CC = bcc +TEST30.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BORLANDC\LIB
INCLUDEPATH = C:\BORLANDC\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 rtmk.obj \
 test.obj

#		*Explicit Rules*
test30.exe: test30.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0s.obj+
rtmk.obj+
test.obj
test30
		# no map file
emu.lib+
maths.lib+
cs.lib
|


#		*Individual File Dependencies*
rtmk.obj: test30.cfg rtmk.c 

test.obj: test30.cfg test.c 

#		*Compiler Configuration File*
test30.cfg: test30.mak
  copy &&|
-v
-vi-
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
| test30.cfg


