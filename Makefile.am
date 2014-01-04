# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

SUBDIRS = \
  address_space \
  binary_protocol \
  builtin_server \
  endpoint_services \
  exe \
  services_registry \
  standard_namespace \
  tcp_server \
  xml_address_space_loader

EXTRA_DIST = \
  debian make_deb.sh

clean-local:
	rm -rvf opcuaserver*$(VERSION)*

dist-deb: dist
	./make_deb.sh

