SRCDIR = src
TOOLSDIR = ..\tools

XULRUNNERDIR = $(TOOLSDIR)\xulrunner-sdk

SOURCES = \
          $(SRCDIR)/impl/windows_utils.cpp \
          $(SRCDIR)/nsOPC.cpp \
          $(SRCDIR)/nsModule.cpp \
          $(SRCDIR)/OPCServer.cpp

INCLUDEDIR= /I$(XULRUNNERDIR)/include/xpcom /I$(XULRUNNERDIR)/include/nspr /I..\tools\boost

DEFINES = /D "XPCOM_EXPORTS" /D "XP_WIN" /D"WIN32" /D "WINDOWS" /D "_WIN32" /D "UNICODE"

FLAGS = /EHsc /c /INCREMENTAL /Zi /MT /Gm /W3

CC = cl

LINK = lib

OUTPUT=opcinterfaces.lib

all: idl
        mkdir -p .libs
	$(CC) $(DEFINES) $(SOURCES) $(INCLUDEDIR) $(FLAGS)
	$(LINK) *.obj /OUT:.\.libs\$(OUTPUT)

idl:
	$(XULRUNNERDIR)\bin\xpidl.exe -m typelib -o src/mozopc -I $(XULRUNNERDIR)\idl -w src/mozopc.idl
	$(XULRUNNERDIR)\bin\xpidl.exe -m header  -o src/mozopc -I $(XULRUNNERDIR)\idl -w src/mozopc.idl

clean:
	rm -rvf *.obj *.lib *.pdb *.idb .libs src\mozopc.xpt src\mozopc.h

check:
	cd tools
	$(MAKE) /f msvc.mk check
	cd $(MAKEDIR)