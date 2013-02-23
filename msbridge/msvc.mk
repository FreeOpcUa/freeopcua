OUTPUT = msopcbridge.dll

SRCDIR = src

TOOLSDIR = ..\tools

MOZOPCDIR = ..\mozopc\src

XULRUNNERDIR = $(TOOLSDIR)\xulrunner-sdk

INTERFACESDIR = ..\mozopc\src


SOURCES = \
          $(SRCDIR)/class_data.cpp\
          $(SRCDIR)/dllmain.cpp\
          $(SRCDIR)/nsIMozOPC.cpp\
          $(SRCDIR)/OPCServer.cpp\
          $(SRCDIR)/OPCGroup.cpp
         
LIBRARIES = \
            nspr4.lib\
            xpcomglue.lib\
            xpcomglue_s.lib\
            opcinterfaces.lib

LIBRARIESDIRS = /LIBPATH:$(TOOLSDIR)/xulrunner-sdk/lib /LIBPATH:$(INTERFACESDIR)/.. /LIBPATH:..\mozopc\.libs

INCLUDEDIR= /I$(MOZOPCDIR) /I$(XULRUNNERDIR)\include\xpcom /I$(XULRUNNERDIR)\include\string /I$(XULRUNNERDIR)\include\nspr /I$(TOOLSDIR) /I$(INTERFACESDIR)

DEFINES = \
          /D "_WIN32_WINNT=0x500"\
          /D "WINVER=0x0500"\
          /D "DEBUG"\
          /D "XPCOM_EXPORTS"\
          /D "XP_WIN"\
          /D "WIN32"\
          /D"WINDOWS"\
          /D"_WIN32"\
          /D"UNICODE"

FLAGS = /EHsc /MT /LD /Zi /link $(LIBRARIESDIRS) $(LIBRARIES) /OUT:.\.libs\$(OUTPUT) /DEF:..\mozopc\src\nsopc.def

LINK = link

all: idl
        mkdir -p .libs
	$(CPP) $(SOURCES) $(INCLUDEDIR) $(DEFINES) $(FLAGS)

depends:
        cd ../mozopc
        $(MAKE) /f msvc.mk $(MAKEFLAGS) $(CLASSDATA)
        cd $(MAKEDIR)

idl:
  $(XULRUNNERDIR)\bin\xpidl.exe -m typelib -o src\msopcbridge -I $(XULRUNNERDIR)\idl -w src\msopcbridge.idl
  $(XULRUNNERDIR)\bin\xpidl.exe -m header  -o src\msopcbridge -I $(XULRUNNERDIR)\idl -w src\msopcbridge.idl


CLEAN_MASK = *.{obj,lib,exp,pdb,ilk,manifest}

clean:
  rm -rvf .libs $(CLEAN_MASK) src\msopcbridge.h src\msopcbridge.xpt


cleanall: clean
        cd ../mozopc
  $(MAKE) /f msvc.mk clean
        cd $(MAKEDIR)
