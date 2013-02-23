TOOLSDIR = ..\tools

MOZOPCDIR = ..\mozopc\src

XULRUNNERDIR = $(TOOLSDIR)\xulrunner-sdk

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
	    cppunit.lib

#..\mozopc\tools\

LIBRARIESDIRS = /LIBPATH:$(TOOLSDIR)\xulrunner-sdk\lib /LIBPATH:$(INTERFACESDIR)\.. /LIBPATH:..\mozopc /LIBPATH:$(TOOLSDIR) /LIBPATH:..\infrastructure /LIBPATH:$(TOOLSDIR)\boost\stage\lib

INCLUDEDIR= /I$(TOOLSDIR)\cppunit\include \
            /I$(MAKEDIR)\
            /I$(MOZOPCDIR) \
            /I$(XULRUNNERDIR)\include\xpcom \
            /I$(XULRUNNERDIR)\include\string \
            /I$(XULRUNNERDIR)\include\nspr \
            /I$(TOOLSDIR) /I$(TOOLSDIR)\boost 

SOURCES = $(MOZOPCDIR)\impl\windows_utils.cpp \
          class_data.cpp \
          infrastructure.cpp \
          test.cpp

DEFINES = /D"WIN32" /D "WINDOWS" /D "_WIN32" /D "UNICODE" 

CLASS_DATA = /D"CLASS_NAME=test_opc_server" /D"CLASS_GUID=ed73f3ef-abeb-4677-9b3b-49e83c122ff3" /D"CLASS_CONTRACT_ID=@treww.org/test_opc_server;1" /D"CLASS_PROG_ID=treww.test_opc_server.1"

FLAGS = /EHsc /Zi /MT /Gm /W3

CC = cl

OUTPUT = test_opc_server.exe

################################################################################
#compiling OPC server test tool
all: cppunitlib
	$(CC) $(DEFINES) $(CLASS_DATA) $(SOURCES) $(INCLUDEDIR) $(FLAGS) /link /OUT:$(OUTPUT) $(LIBRARIESDIRS) $(LIBRARIES) 2>&1 | tee $(OUTPUT).out 

################################################################################
# compiling cppunit
cppunitlib:
	cd $(TOOLSDIR)
        nmake /f cppunit.mk
	cd $(MAKEDIR)

################################################################################
# compiling opc servers to be tested
servers:
        cd ..\msbridge
        $(MAKE) /f msvc.mk
        cd $(MAKEDIR)

        cd ..\servers\test_server
        $(MAKE) /f msvc.mk
        cd $(MAKEDIR)

################################################################################
# check opc servers

XULRUNNER_BINDIR = $(TOOLSDIR)\xulrunner-sdk\bin

check: all
        cp -f $(OUTPUT) $(XULRUNNER_BINDIR)
        cp -f ..\msbridge\.libs\msopcbridge.dll $(XULRUNNER_BINDIR)
        cp -f ..\servers\test_server\.libs\testopcserver.dll $(XULRUNNER_BINDIR)
        cd $(XULRUNNER_BINDIR)
	$(OUTPUT) --filename=msopcbridge.dll --name="OPC_Bridge" --contract-id="@treww.org/msopcbridge;1" --guid="B6BA1A1C-2547-4439-8E4B-9FAF97EFC272" --disable-com-tests
	$(OUTPUT) --filename=testopcserver.dll --name="test_opc_server" --contract-id="@treww.org/test_opc_server;1" --prog-id="test_opc_server.mozopc.1" --guid="ed73f3ef-abeb-4677-9b3b-49e83c122ff3"
	rm -f $(OUTPUT) msopcbridge.dll testopcserver.dll
	cd $(MAKEDIR)
	rm -f $(OUTPUT).out


################################################################################
# cleaning all compiling staff
DEL_MASKS = *.{obj,lib,exp,pdb,idb,ilk,manifest}
clean:
  rm -rvf  $(DEL_MASKS) $(OUTPUT) test_opc_server_app.exe.out


################################################################################
# clean all dependences
cleanall: clean
        cd ..\..\mozopc
	$(MAKE) /f msvc.mk clean
        cd $(MAKEDIR)

        cd ..\infrastructure
	$(MAKE) /f msvc.mk clean
        cd $(MAKEDIR)
