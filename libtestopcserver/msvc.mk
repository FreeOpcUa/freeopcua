OUTPUT = testopcserver.dll

SRCDIR = src

TOOLSDIR = ..\..\tools

MOZOPCDIR = ..\..\mozopc\src

XULRUNNERDIR = $(TOOLSDIR)\xulrunner-sdk

BOOSTDIR = $(TOOLSDIR)\boost

SOURCES = \
          dllmain.cpp\
          lib.cpp\
          class_data.cpp

         
LIBRARIES = \
            kernel32.lib user32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
            crmf.lib\
            embed_base_s.lib\
            embed_base_standalone.lib\
            js3250.lib\
            mozreg_s.lib\
            nspr4.lib\
            nss3.lib\
            nssutil3.lib\
            plc4.lib\
            plds4.lib\
            smime3.lib\
            ssl3.lib\
            unicharutil_external_s.lib\
            xpcomglue.lib\
            xpcomglue_s.lib\
            opcinterfaces.lib\
            opcinfrastructure.lib\

LIBRARIESDIRS = /LIBPATH:$(TOOLSDIR)\xulrunner-sdk\lib /LIBPATH:$(INTERFACESDIR)\.. /LIBPATH:..\..\mozopc\.libs /LIBPATH:..\infrastructure\.libs

INCLUDEDIR= /I$(MOZOPCDIR) /I$(XULRUNNERDIR)\include\xpcom /I$(XULRUNNERDIR)\include\string /I$(XULRUNNERDIR)\include\nspr /I$(BOOSTDIR)

DEFINES = \
          /D"_WIN32_WINNT=0x500" \
          /D"WINVER=0x0500" \
          /D"DEBUG" \
          /D"XP_WIN" \
          /D"WIN32" \
          /D"WINDOWS" \
          /D"_WIN32" \
          /D"UNICODE" \
          /D "_WINDLL"

FLAGS = /EHsc /LD /INCREMENTAL /Zi /MT /Gm /W3

LINK = link

all:
        mkdir -p .libs
	$(CPP) $(SOURCES) $(INCLUDEDIR) $(DEFINES) $(FLAGS) /link $(LIBRARIESDIRS) $(LIBRARIES) /OUT:.libs\$(OUTPUT) /DEF:..\..\mozopc\src\mozopc.def 

depends:
        cd ..\..\mozopc
        $(MAKE) /f msvc.mk $(MAKEFLAGS)
        cd $(MAKEDIR)

        cd ..\infrastructure
        $(MAKE) /f msvc.mk $(MAKEFLAGS)
        cd $(MAKEDIR)

DEL_MASKS = *.{obj,lib,exp,pdb,idb,ilk,manifest}
clean:
  rm -rvf  $(DEL_MASKS) .libs tests/$(DEL_MASKS) tests/$(TEST_OUTPUT) test_opc_server_app.exe.out

#clean all dependences
cleanall: clean
        cd ..\..\mozopc
	$(MAKE) /f msvc.mk clean
        cd $(MAKEDIR)

        cd ..\infrastructure
	$(MAKE) /f msvc.mk clean
        cd $(MAKEDIR)
