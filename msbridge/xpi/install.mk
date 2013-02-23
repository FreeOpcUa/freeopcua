all: msbridge
  mkdir plugins
  cp ../../mozopc/src/mozopc.xpt plugins/mozopc.xpt
  cp ../msopcbridge.dll plugins/msopcbridge.dll
  zip msopcbridge.xpi -xi install.rdf application.ini plugins/*
  rm -rvf plugins
  
msbridge:
  cd ..
  $(MAKE) /f msvc.mk $(MAKEFLAGS)
  cd $(MAKEDIR)

clean:
  del *.xpi

cleanall: clean
  cd ..
  $(MAKE) /f msvc.mk cleanall
  cd $(MAKEDIR)
