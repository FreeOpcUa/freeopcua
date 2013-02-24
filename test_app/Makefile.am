# @author Alexander Rykovanov 2011
# @email rykovanov.as@gmail.com
# @brief Addons manager realization
# @license GNU GPL
#
# Distributed under the GNU GPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/gpl.html)
#


sources = \
 application.ini \
 chrome.manifest\
 chrome/chrome.manifest\
 chrome/content/mozopc/mozopc.js\
 chrome/content/mozopc/mozopc.xul\
 chrome/content/mozopc/new_group.js\
 chrome/content/mozopc/new_group.xul\
 chrome/content/mozopc/new_items.js\
 chrome/content/mozopc/new_items.xul\
 chrome/content/mozopc/new_server.js\
 chrome/content/mozopc/new_server.xul\
 chrome/content/mozopc/utils.js\
 chrome/content/mozopc/write_value.js\
 chrome/content/mozopc/write_value.xul\
 chrome/locale/en-US/mozopc.dtd\
 chrome/locale/en-US/mozopc.properties\
 chrome/locale/ru/mozopc.dtd\
 chrome/locale/ru/mozopc.properties\
 chrome/skin/mozopc.css\
 chrome/skin/mozopc.xml\
 chrome/skin/img/clock.png\
 chrome/skin/img/comp.png\
 chrome/skin/img/del.png\
 chrome/skin/img/down.png\
 chrome/skin/img/find.png\
 chrome/skin/img/i.png\
 chrome/skin/img/new.png\
 chrome/skin/img/open.png\
 chrome/skin/img/plus.png\
 chrome/skin/img/right.png\
 chrome/skin/img/save_as.png\
 chrome/skin/img/save.png\
 components/components.manifest\
 defaults/preferences/mozopc-prefs.js

binary_files = \
 components/libgpsopcserver.so \
 components/libtestopcserver.so \
 components/mozopc.xpt

result = mozopc_test_app.zip

CLEANFILES = $(result) $(binaryfiles)

XULRUNNER_BIN_DIR = $(XULRUNNER_SDKDIR)/bin
XULRUNNER_IDL_DIR = $(XULRUNNER_SDKDIR)/idl

xul_output_dir = $(abs_top_builddir)/test_app/.libs

all:
	test -e $(xul_output_dir) || mkdir $(xul_output_dir) ;
	test -e $(xul_output_dir)/chrome || cp -rf $(abs_top_srcdir)/test_app/chrome $(xul_output_dir) ;
	test -e $(xul_output_dir)/defaults || cp -rf $(abs_top_srcdir)/test_app/defaults $(xul_output_dir);
	test -e $(xul_output_dir)/components || mkdir -p $(xul_output_dir)/components;
	test -e $(xul_output_dir)/application.ini || cp -f $(abs_top_srcdir)/test_app/application.ini $(xul_output_dir)/application.ini;
	test -e $(xul_output_dir)/chrome.manifest || cp -f $(abs_top_srcdir)/test_app/chrome.manifest $(xul_output_dir)/chrome.manifest;
	test -f -$(xul_output_dir)/components/components.manifest || echo -n > $(xul_output_dir)/components/components.manifest;
	if test -e $(abs_top_builddir)/servers/gps/dll/xpcom/.libs/libgpsopcserver.so ; then \
	test -e $(xul_output_dir)/components/libgpsopcserver.so || cp -f $(abs_top_builddir)/servers/gps/dll/xpcom/.libs/libgpsopcserver.so $(xul_output_dir)/components/libgpsopcserver.so; \
	echo "binary-component libgpsopcserver.so ABI=Linux_x86-gcc3" > $(xul_output_dir)/components/components.manifest ;\
	fi
	if test -e $(abs_top_builddir)/servers/test_server/dll/xpcom/.libs/libtestopcserver.so ; then \
	test -e $(xul_output_dir)/components/libtestopcserver.so || cp -f $(abs_top_builddir)/servers/test_server/dll/xpcom/.libs/libtestopcserver.so $(xul_output_dir)/components/libtestopcserver.so; \
	echo "binary-component libtestopcserver.so ABI=Linux_x86-gcc3" >> $(xul_output_dir)/components/components.manifest; \
        fi
	test -e $(xul_output_dir)/components/mozopc.xpt || $(XULRUNNER_BIN_DIR)/xpidl -m typelib -o $(xul_output_dir)/components/mozopc -I $(XULRUNNER_IDL_DIR) -w $(top_srcdir)/xpcom/src/mozopc.idl
	echo "interfaces mozopc.xpt"  >> $(xul_output_dir)/components/components.manifest 
	test -e $(xul_output_dir)/$(result) || ( cd $(xul_output_dir) && zip -9 $(result) $(sources) $(binary_files) && cd $(abs_top_build_dir) );

clean:
	rm -rf $(builddir)/.libs

distclean: clean
	rm -f Makefile

run:
	$(abs_top_srcdir)/test_app/run.sh $(XULRUNNER_BIN_DIR) $(xul_output_dir)

debug:
	$(abs_top_srcdir)/test_app/run_debug.sh $(XULRUNNER_BIN_DIR) $(xul_output_dir)

