#!/bin/sh
# the next line restarts using wish \
exec wish8.4 "$0" "$@"

# gpsfeed+. A GPS simulator
#
# Provides simulated GPS data to various ports, either TCP, UDP or serial.
#
# Copyright (c) 2004-2005, Dimitrios C. Zachariadis
# All rights reserved.
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#    http://www.gnu.org/copyleft/gpl.html
#
set ::version "v0.58"
# the following is set for debugging
if {$tcl_platform(platform) eq "windows"} {
	if {![catch {package req dde}]} {
		dde server gpsdeed+
	}
}
package req Tk
package provide carto 0.3
# Creates the carto command handler for canvas widgets.
proc carto {w args} {
	namespace eval ::carto::${w} {
		variable var
		array set var [list \
			-autoscale "meet" \
			-detail 1 \
			-meets "x" \
			-pixelaspectratio 1 \
			-space cartesian \
			w 200 \
			h 200 \
			scale 1 \
			cxy {}
		]
	}
	upvar ::carto::${w}::var v
	if {![winfo exists $w]} {
		eval [list canvas $w -width 200 -height 200]
	}
	rename ::$w ::_$w
	::carto::dispatchConfig $w $args
	# the window dimensions returned by winfo include border and highlight
	# which we must subtract to get the actual width and height
	set brdrs [expr {2*([::_$w cget -highlightthickness]+[::_$w cget -borderwidth])}]
	scan [winfo geometry $w] "%dx%d" v(ww) v(wh)
	set v(ww) [expr {$v(ww)-$brdrs}]
	set v(wh) [expr {$v(wh)-$brdrs}]
	::_$w configure -xscrollincrement 1
	::_$w configure -yscrollincrement 1
	bind $w <Configure> {::carto::setScale %W}
	proc ::$w {cmd args} "return \[eval ::carto::Handler $w \$cmd \$args]"
	set w
}

namespace eval carto {
	proc Handler {w cmd args} {
		set debug 0
		upvar ::carto::${w}::var v
		switch -- $cmd {
			center {
				set vcx [expr {([::_$w canvasx 0]+$v(ww)/2.0)/($v(scale)*$v(-pixelaspectratio))}]
				set vcy [expr {-([::_$w canvasy 0]+$v(wh)/2.0)/$v(scale)}]
				if {$args=={}} {
					return [list $vcx $vcy $v(scale)]
				}
				if {[llength $args]!=3} {
					error "wrong # args: should be \"$w center x y scale\""
				}
				set redraw 0
				# Set center of cartoview and scale
				foreach {x y scale} $args {}
				# if scale has changed find the new viewbox width
				if {$scale!="-"} {
					set v(scale) $scale
					if {$debug} {puts "...set scale to $v(scale)"}
					set v(w) [expr {double($v(ww))/($v(scale)*$v(-pixelaspectratio))}]
					set v(h) [expr {double($v(wh))/$v(scale)}]
					# scale changed so force a redraw
					set redraw 1
				}
				# if x or y did not change, get them from current view
				if {$x=="-" || $x==""} {
					set x $vcx
				}
				if {$y=="-" || $y==""} {
					set y $vcy
				}
				# set viewbox according to the new center
				set x0 [expr {$x-$v(ww)/(2.0*$v(scale)*$v(-pixelaspectratio))}]
				set y0 [expr {$y+$v(wh)/(2.0*$v(scale))}]
				# Redraw or just scroll canvas to view
				if {$redraw} {
					if {$debug} {puts "...generating event $w <<CartoRedraw>>"}
					event generate $w <<CartoRedraw>>
				}
				setScroll $w $x0 $y0
			}
			cget {
				switch -- [lindex $args 0] {
					-autoscale {return $v(-autoscale)}
					-detail {return $v(-detail)}
					-meets {return $v(-meets)}
					-space {return $v(-space)}
					-pixelaspectratio {return $v(-pixelaspectratio)}
					default {eval {::_$w $cmd } $args}
				}
			}
			config -
			configure {eval [list dispatchConfig $w] $args}
			create {
				switch -- [lindex $args 0] {
					crosshair {
						if {[llength $args]<2} {
							error "wrong # args: should be \"$w create crosshair tag ?option value?\""
						}
						set tag [lindex $args 1]
						eval [list ::_$w create line -3000 0 3000 0  -tag [list $tag ${tag}x]] [lrange $args 2 end]
						eval [list ::_$w create line 0 -3000 0 3000  -tag [list $tag ${tag}y]] [lrange $args 2 end]
						foreach {x1 y1 x2 y2} [::_$w bbox $tag] {}
						set v($tag,offset) [list [expr {(abs($x1)+abs($x2))/2.0}] [expr {(abs($y1)+abs($y2))/2.0}]]
						# save tag in a list so that we can find it for redraw
					}
					default {eval [list ::_$w $cmd] $args}
				}
			}
			delete {
				set tag [lindex $args 0]
				if {[info exists v($tag,offset)]} {
					unset v($tag,offset)
				}
				eval [list ::_$w $cmd] $args
			}
			destroy {
				# destroy canvas widget and carto command
				namespace delete ::carto::${w}
				destroy ::$w
				rename ::$w {}
				rename ::_$w {}
			}
			forget {
				# destroy carto command but retain carto widget
				namespace delete ::carto::${w}
				rename ::$w {}
				rename ::_$w ::$w
			}
			Move {
				if {[llength $args]<3} {error "wrong # args: should be \"$w Move tagOrId x y\""}
				foreach {tag x y} $args {}
				# calculate tag's center coords
				foreach {bbx1 bby1 bbx2 bby2} [::_$w bbox $tag] {}
				if {![info exists bbx1]} {
					# couldn't find tag
					return
				}
				if {![info exists v($tag,offset)]} {
					# tag is not a symbol
					set cx0 $bbx1
					set cy0 $bby1
				} else {
					set cx0 [expr {($bbx1+[lindex $v($tag,offset) 0])}]
					set cy0 [expr {($bby1+[lindex $v($tag,offset) 1])}]
				}
				set cx [expr {$x*$v(scale)*$v(-pixelaspectratio)}]
				set cy [expr {-$y*$v(scale)}]
				::_$w move $tag [expr {$cx-$cx0}] [expr {$cy-$cy0}]
			}
			Plot {
				# accepts one of:
				# pathName Plot x y ?-tag tag? ?-mode [abs|rel]? ?-space [cartesian|polar]? ?-option option?
				# pathName Plot {x y ...} ?-option option?
				if {$debug} {puts "Plot...args=$args"}
				set arglen [llength $args]
				if {$arglen==0 || $arglen==2 || $arglen==4} {
					return;error "wrong # args: should be \"$w Plot tagOrId ?x y ... ?\""
				}
				foreach  {tag x1 y1 x2 y2} [lrange $args 0 4] {}
				if {[::_$w find withtag $tag]=={}} {
					# this is a new tag
					if {$x2=={}} {
						set x2 $x1
						set y2 $y1
					}
					::_$w create line \
						[expr {$x1*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y1*$v(scale)}] \
						[expr {$x2*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y2*$v(scale)}] \
						-tag $tag -fill grey
					if {$arglen>5} {
						set d [::_$w coords $tag]
						foreach {x y} [lrange $args 5 end] {
							lappend d  [expr {$x*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y*$v(scale)}]
						}
						::_$w coords $tag $d
					}
				} else {
					set d [::_$w coords $tag]
					foreach {x y} [lrange $args 1 end] {
						lappend d  [expr {$x*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y*$v(scale)}]
					}
					::_$w coords $tag $d
				}
			}
			lPlot {
				if {$debug} {puts "lPlot...args=$args"}
				if {[llength $args]<=5} {return;error "wrong # args: should be \"$w lPlot tagOrId ?x1 y1 x2 y2 ... ?\"" }
				set tag [lindex $args 0]
				if {[::_$w find withtag $tag]!={}} {
					foreach {x y} [lrange $args 1 end] {
						lappend d  [expr {$x*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y*$v(scale)}]
					}
				} else {
					foreach  {tag x1 y1 x2 y2} [lrange $args 0 4] {}
					lappend d  [expr {$x1*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y1*$v(scale)}]
					lappend d  [expr {$x2*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y2*$v(scale)}]
					::_$w create line $d -tag $tag -fill grey
					foreach {x y} [lrange $args 5 end] {
						lappend d  [expr {$x*$v(scale)*$v(-pixelaspectratio)}]  [expr {-$y*$v(scale)}]
					}
				}
				::_$w coords $tag $d
			}
			setview {
				set args [join $args]
				set vx0 [expr {[::_$w canvasx 0]/($v(scale)*$v(-pixelaspectratio))}]
				set vy0 [expr {-[::_$w canvasy 0]/$v(scale)}]
				if {$args=={}} {
					return [list $vx0 $vy0 [expr {$v(ww)/($v(scale)*$v(-pixelaspectratio))}] [expr {$v(wh)/$v(scale)}]]
				}
				if {[llength $args]!=4} {
					error "wrong # args: should be \"$w setview x y w h\""
				}
				set redraw 0
				foreach {x0 y0 wi hi} $args {}
				# do we need this ?
				if {$x0=="-"} {
					set x0 $vx0
				}
				if {$y0=="-"} {
					set y0 $vy0
				}
				# if viewbox width changed force redraw
				if {$wi!="-"} {
					set v(w) $wi
					set redraw 1
				}
				# if viewbox height changed force redraw
				if {$hi!="-"} {
					set v(h) $hi
					set redraw 1
				}
				# Redraw or just scroll canvas to view
				if {$redraw} {
					setScale $w
				}
				setScroll $w $x0 $y0
			}
			symbolize {
				if {[llength $args]!=1 && [llength $args]!=3} {
					error "wrong # args: should be \"$w symbolize tag ?offsetx offsety?\""
				}
				set symbol [lindex $args 0]
				if {[$w find withtag $symbol]=={}} {
					error "tag $symbol does not exist"
				}
				if {[llength $args]==1} {
					foreach {x1 y1 x2 y2} [::_$w bbox $symbol] {}
					set v($symbol,offset) [list [expr {($x2-$x1)/2.0}] [expr {($y2-$y1)/2.0}]]
				} else {
					set v($symbol,offset) [lrange $args 1 end]
				}
			}
			default {eval [list ::_$w $cmd] $args}
		}
	}

	#dispatch configure pairs
	proc dispatchConfig {w args} {
		set debug 0
		if {$debug} {puts "dispatchConfig called by [expr {[info level]>1?[lindex [info level -1] 0]:[list user]}]"}
		# strip possible ::_ so that namespace is referenced properly
		regexp {^(::_)*(.*)} $w m pre wg
		upvar ::carto::${wg}::var v
		if {[llength $args]==0} {
			return [concat [eval {::_$w configure}] \
				[list [list -space $v(-space)]] \
				[list [list -pixelaspectratio $v(-pixelaspectratio)]] \
				[list [list -autoscale $v(-autoscale)]] \
				]
		}
		foreach {opt val} $args {
			switch -- $opt {
				-autoscale {
					if {$val=={}} {return $v(-autoscale)}
					if {![regexp -- {meet|slice|none} $val]} {
						error "unknown option $val: should be meet, slice or none"
					}
					set v(-autoscale) $val
				}
				-detail {
					if {$val=={}} {return $v(-detail)}
					if {![string is int $val]} {
						error "expected integer but got $val"
					}
					set v(-detail) $val
				}
				-space {
					if {$val=={}} {return $v(-space)}
					if {![regexp -- {meet|slice|none} $val]} {
						error "unknown option $val: should be cartesian or polar"
					}
					set v(-space) $val
				}
				-pixelaspectratio {
					if {$val=={}} {return $v(-pixelaspectratio)}
					if {![string is double $val]} {
						error "expected real but got $val"
					}
					set v(-pixelaspectratio) $val
				}
				default {eval {::_$w configure} $opt $val}
			}
		}
	}

	proc setScale wg {
		set debug 0
		if {$debug} {puts "setScale called by [expr {[info level]>1?[lindex [info level -1] 0]:[list user]}]"}
		upvar ::carto::${wg}::var v
		scan [winfo geometry $wg] "%dx%d" v(ww) v(wh)
		set brdrs [expr {2*([::_$wg cget -highlightthickness]+[::_$wg cget -borderwidth])}]
		set v(ww) [expr {$v(ww)-$brdrs}]
		set v(wh) [expr {$v(wh)-$brdrs}]
		set aspectratio [expr {double($v(ww))/$v(wh)}]
		switch -regexp -- $v(-autoscale) {
			meet {
				# max(viewbox) -> min(window)
				if {$v(wh)!=0 && $v(w)!=0 && $v(h)!=0} {
					if {double($v(w))/$v(h)>$aspectratio} {
						set v(-meets) "x"
						set v(scale) [expr {double($v(ww))/$v(w)}]
					} else {
						set v(-meets) "y"
						set v(scale) [expr {double($v(wh))/$v(h)}]
					}
				}
			}
			slice {
				# min(viewbox) -> max(window)
				if {$v(wh)!=0 && $v(w)!=0 && $v(h)!=0} {
					if {double($v(w))/$v(h)>$aspectratio} {
						set v(-meets) "y"
						set v(scale) [expr {double($v(wh))/$v(h)}]
					} else {
						set v(-meets) "x"
						set v(scale) [expr {double($v(ww))/$v(w)}]
					}
				}
			}
			none {
				# no event is generated
				return
			}
			default {
				return
			}
		}
#		set v(w) [expr {double($v(ww))/$v(scale)}]
#		set v(h) [expr {double($v(wh))/$v(scale)}]
		if {$debug} {puts "...event generate $wg <<CartoRedraw>>"}
		event generate $wg <<CartoRedraw>>
		if {$debug} {puts "...set scale to $v(scale)"}
	}

	proc setScroll {w x0 y0} {
		upvar ::carto::${w}::var v
		::_$w xview scroll [expr {round($x0*$v(scale)*$v(-pixelaspectratio)-[::_$w canvasx 0])}] u
		::_$w yview scroll [expr {-round($y0*$v(scale)+[::_$w canvasy 0])}] u
	}

	proc screentocarto {w x y} {
		upvar ::carto::${w}::var v
		return [list \
				[expr {($x+[::_$w canvasx 0])/($v(scale)*$v(-pixelaspectratio))}] \
				[expr {-($y+[::_$w canvasy 0])/$v(scale)}]]
	}
	proc tocarto {w x y} {
		upvar ::carto::${w}::var v
		return [list [expr {$x/($v(scale)*$v(-pixelaspectratio))}] [expr {-$y/$v(scale)}]]
	}
	proc tocanvas {w x y} {
		upvar ::carto::${w}::var v
		return [list [expr {$x*$v(scale)*$v(-pixelaspectratio)}] [expr {-$y*$v(scale)}]]
	}
	proc centertoview {w args} {
	}
	proc viewtocenter {w args} {
	}
}
#---------------------------------------------------------------------------------------
# Simple Sample httpd/1.0 server in 250 lines of Tcl
# Stephen Uhler / Brent Welch (c) 1996 Sun Microsystems
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.

# This is a working sample httpd server written entirely in TCL with the
# CGI and imagemap capability removed.  It has been tested on the Mac, PC
# and Unix.  It is intended as sample of how to write internet servers in
# Tcl. This sample server was derived from a full-featured httpd server,
# also written entirely in Tcl.
# Comments or questions welcome (stephen.uhler@sun.com)

# Httpd is a global array containing the global server state
#  root:	the root of the document directory
#  port:	The port this server is serving
#  listen:	the main listening socket id
#  accepts:	a count of accepted connections so far

# HTTP/1.0 error codes (the ones we use)
array set HttpdErrors {
	204 {No Content}
	400 {Bad Request}
	404 {Not Found}
	503 {Service Unavailable}
	504 {Service Temporarily Unavailable}
	}
array set Httpd {
	bufsize	32768
	sockblock	0
	default	index.html
}
# Generic error response.
set HttpdErrorFormat {
	<title>Error: %1$s</title>
	Error: %3$s - %2$s
}
# convert the file suffix into a mime type
# add your own types as needed
array set HttpdMimeType {
	{}		text/plain
	.txt	text/plain
	.htm	text/html
	.html	text/html
	.css	text/css
	.gif	image/gif
	.ico	image/x-icon
	.jpg	image/jpeg
	.png	image/png
	.xbm	image/x-xbitmap
	.svg	image/svg+xml
	.svgz	image/svg+xml
	.cgi	application/x-httpd-cgi
}
# Start the server by listening for connections on the desired port.
proc Httpd_Server {root {port 80} {myaddr {}}} {
	global Httpd
	array set Httpd [list root $root]
	if {![info exists Httpd(port)]} {
		set Httpd(port) $port
		if {[string length $myaddr]} {
			set err [catch {socket -server HttpdAccept -myaddr $myaddr $port} Httpd(listen)]
			if {!$err} {
				set Httpd(name) $myaddr
			}
		} else {
			set err [catch {socket -server HttpdAccept $port} Httpd(listen)]
			if {!$err} {
				set Httpd(name) 127.0.0.1;#[info hostname]
			}
		}
		if {$err} {
			unset Httpd(listen)
			return ""
		}
		set Httpd(accepts) 0
	}
	return $Httpd(port)
}
# Accept a new connection from the server and set up a handler
# to read the request from the client.
proc HttpdAccept {newsock ipaddr port} {
	global Httpd
	upvar #0 Httpd$newsock data
	incr Httpd(accepts)
	#collect all new sockets
	fconfigure $newsock -blocking $Httpd(sockblock) \
		-buffersize $Httpd(bufsize) \
		-translation {auto crlf}
	set data(ipaddr) $ipaddr
	set data(left) 50
	fileevent $newsock readable [list HttpdRead $newsock]
}
# read data from a client request
proc HttpdRead { sock } {
	upvar #0 Httpd$sock data
	set readCount [gets $sock line]
	if {![info exists data(state)]} {
		if [regexp {(POST|GET) ([^?]+)\??([^ ]*) HTTP/(1.[01])} \
			$line x data(proto) data(url) data(query) data(version)] {
			set data(state) mime
		} elseif {[string length $line] > 0} {
			HttpdError $sock 400
			Httpd_Log $sock Error "bad first line:$line"
			HttpdSockDone $sock
		} else {
			# Probably eof after keep-alive
			HttpdSockDone $sock
		}
		return
	}
	# string compare $readCount 0 maps -1 to -1, 0 to 0, and > 0 to 1
	set state [string compare $readCount 0],$data(state),$data(proto)
	switch -- $state {
		0,mime,GET	-
		0,query,POST	{ HttpdRespond $sock }
		0,mime,POST	{ set data(state) query }
		1,mime,POST	-
		1,mime,GET	{
			if [regexp {([^:]+):[ 	]*(.*)}  $line dummy key value] {
				set data(mime,[string tolower $key]) $value
			}
		}
		1,query,POST	{
			set data(query) $line
			HttpdRespond $sock
		}
		default {
			if [eof $sock] {
				Httpd_Log $sock Error "unexpected eof on <$data(url)> request"
			} else {
				Httpd_Log $sock Error "unhandled state <$state> fetching <$data(url)>"
			}
			HttpdError $sock 404
			HttpdSockDone $sock
		}
	}
}
# Respond to the query.
proc HttpdRespond sock {
	global Httpd gps
	upvar #0 Httpd$sock data
	set out ""
	set type ""
	set mypath [HttpdUrl2File $Httpd(root) $data(url)]
	if {[string length $mypath] == 0} {
		HttpdError $sock 400
		Httpd_Log $sock Error "$data(url) invalid path"
		HttpdSockDone $sock
		return
	}
	switch -- $data(url) {
		/gpsfeed {
			set query [regexp -all -inline -- {\s*([^=\s]*?)\s*=\s*([^;\s]*?)\s*;*} $data(query)]
			set contentType "text/plain"
			foreach {m var value} $query {
				if {$var eq "type"} {set type $value}
				if {$value eq ""} {
					append out $var=[expr {[catch {set gps($var)}]?"":$gps($var)}] ";"
				}
			}
			if {$type ne ""} {
				switch -- $type {
					xml {
						set out [SubstVars $::prefs(httpxml)]
						set contentType "text/xml"
					}
					default {
					}
				}
			} elseif {$out eq ""} {
				set out "lon=$gps(lon);lat=$gps(lat);alt=$gps(alt);bear=$gps(bear);speed=$gps(speed);"
			}
			puts $sock "HTTP/1.0 200 Data follows"
			puts $sock "Date: [HttpdDate [clock seconds]]"
			puts $sock "Last-Modified: [HttpdDate $gps(epoch)]"
			puts $sock "Connection: Keep-Alive"
			puts $sock "Content-Type: $contentType"
			puts $sock "Content-Length: [string length $out]"
			puts $sock ""
			puts $sock $out
			flush $sock
			HttpdSockDone $sock
		}
		default {
			if {![catch {open $mypath} in]} {
				puts $sock "HTTP/1.0 200 Data follows"
				puts $sock "Date: [HttpdDate [clock seconds]]"
				puts $sock "Last-Modified: [HttpdDate [file mtime $mypath]]"
				puts $sock "Connection: Keep-Alive"
				puts $sock "Content-Type: [HttpdContentType $mypath]"
				puts $sock "Content-Length: [file size $mypath]"
				puts $sock ""
				fconfigure $sock -translation binary -blocking $Httpd(sockblock)
				fconfigure $in -translation binary -blocking 1
				flush $sock
				fcopy $in $sock -command [list HttpdCopyDone $in $sock]
			} else {
				HttpdError $sock 404
				Httpd_Log $sock Error "$data(url) $in"
				HttpdSockDone $sock
			}
		}
	}
}
proc HttpdCopyDone {in sock bytes {errorMsg {}}} {
	close $in
	HttpdSockDone $sock [expr {[string length $errorMsg] > 0}]
}
proc HttpdContentType {path} {
	global HttpdMimeType
	set type text/plain
	catch {set type $HttpdMimeType([file extension $path])}
	return $type
}
proc HttpdError {sock code} {
	upvar #0 Httpd$sock data
	global HttpdErrors HttpdErrorFormat
	append data(url) ""
	set message [format $HttpdErrorFormat $code $HttpdErrors($code)  $data(url)]
	puts $sock "HTTP/1.0 $code $HttpdErrors($code)"
	puts $sock "Date: [HttpdDate [clock clicks]]"
	puts $sock "Content-Length: [string length $message]"
	puts $sock ""
	puts $sock $message
}
# Generate a date string in HTTP format.
proc HttpdDate {clicks} {
	return [clock format $clicks -format {%a, %d %b %Y %T %Z}]
}
# Log an Httpd transaction.
# This should be replaced as needed.
proc Httpd_Log {sock reason args} {
	global httpdLog httpClicks
	if {[info exists httpdLog]} {
		if ![info exists httpClicks] {
			set last 0
		} else {
			set last $httpClicks
		}
		set httpClicks [clock clicks]
		catch {
			puts $httpdLog "[clock format [clock seconds]] ([expr $httpClicks - $last])\t$sock\t$reason\t[join $args { }]"
		}
	}
}
# Convert a url into a pathname.
# This is probably not right.
proc HttpdUrl2File {root url} {
	global HttpdUrlCache Httpd
	if {![info exists HttpdUrlCache($url)]} {
		lappend pathlist $root
		set level 0
		foreach part  [split $url /] {
			set part [HttpdCgiMap $part]
			if [regexp {[:/]} $part] {
				return [set HttpdUrlCache($url) ""]
			}
			switch -- $part {
				.  { }
				.. {incr level -1}
				default {incr level}
			}
			if {$level <= 0} {
				return [set HttpdUrlCache($url) ""]
			}
			lappend pathlist $part
		}
		set file [eval file join $pathlist]
		if {[file isdirectory $file]} {
			set file [file join $file $Httpd(default)]
		}
		set HttpdUrlCache($url) $file
	}
	return $HttpdUrlCache($url)
}
# Decode url-encoded strings.
proc HttpdCgiMap {data} {
	regsub -all {([][$\\])} $data {\\\1} data
	regsub -all {%([0-9a-fA-F][0-9a-fA-F])} $data  {[format %c 0x\1]} data
	return [subst $data]
}
proc bgerror {msg} {
	global errorInfo
	puts stderr "bgerror: $msg\n$errorInfo"
}
# Close a socket.
proc HttpdSockDone { sock {close 0}} {
	upvar #0 Httpd$sock data
	global Httpd
	if {!$close && ($data(left) > 0) && 
		(([info exists data(mime,connection)] && 
		([string tolower $data(mime,connection)] == "keep-alive")) || 
		($data(version) >= 1.1))} {
		set close 0
	} else {
		set close 1
	}
	if [info exists data(cancel)] {
		after cancel $data(cancel)
	}
	catch {close $data(infile)}
	if {$close} {
		catch {close $sock}
		unset data
	} else {
		# count down transations
		set left [incr data(left) -1]
		# Reset the connection
		flush $sock
		set ipaddr $data(ipaddr)
		unset data
		array set data [list linemode 1 version 0 left $left ipaddr $ipaddr]
		# Close the socket if it is not reused within a timeout
		set data(cancel) [after 10000 [list HttpdSockDone $sock 1]]
		fconfigure $sock -blocking 0 -buffersize $Httpd(bufsize) -translation {auto crlf}
		fileevent $sock readable [list HttpdRead $sock]
		fileevent $sock writable {}
	}
}
#--------------------------------------------------------------------------------
# gpsdeed+ starts here
#
#
package req udp
set PI 3.1415926535897931
set dtor [expr {$PI/180.0}]
set rtod [expr {180.0/$PI}]
set mps [expr {1852.0/3600}]

set prefs(serial) 0
set prefs(tcp) 1
set prefs(udp) 0
set prefs(mcast) 0
set prefs(http) 1
set prefs(serialport) "COM1:"
set prefs(httproot) ""
set prefs(httpport) 80
set prefs(tcpport) 2222
set prefs(udpaddress) "224.5.1.21"
set prefs(udpport) 1365
set prefs(ttl) 2
set prefs(aLat) 38.0
set prefs(aLon) 24.0
set prefs(anAlt) 0.0
set prefs(aRange) 0.5
set prefs(skip) 10
set prefs(tail) 3600
set prefs(serialspeed) 4800
set prefs(period) 2
set prefs(timecorrection) -0
set prefs(shape) "Spiral"
set prefs(fpath) "./"
set prefs(fname) "gps.txt"
set prefs(zoommode) 0
set prefs(showmap) 1
set prefs(showconf) 1
set prefs(gpsmode) "simulate"
set prefs(signal) 1
set prefs(trigger) GPRMC
set prefs(sentences) {GPGGA GPRMC GPGSV usersentence}
set prefs(GPGGA) 1
set prefs(GPGSV) 0
set prefs(GPGLL) 0
set prefs(GPRMC) 1
set prefs(GPZDA) 0
set prefs(HCHDG) 0
set prefs(PGRMZ) 0
set prefs(usersentence) 0
set prefs(userNMEA) {{GPZDA,$time,$msec,$DD,$MO,$YYYY,03,00} \
					{TEST,yourUnit,$lonf1,$ew,$latf1,$ns,yourName,$time} {GPRMC,$time,$fix,$latf1,$ns,$lonf1,$ew,$knots,$bear,$date,$magnvar,$magnew,$magnfix} {GPSTPV,$epoch.$msec,?,$lat,$lon,,$alt,,$speed,,$bear,,,,A}}
set prefs(httpxml) {{<?xml version="1.0" ?>} \
					{<circle id="fix" cx="$lon" cy="$lat" r="0.001" fill="red" stroke="black" stroke-width="0.0005"/>}}
set prefs(cpanel) 0
set prefs(save) 1
set prefs(showdata) 1
set prefs(showtime) 1
set prefs(showcoords) 1
set prefs(showstatus) 1
set prefs(showcross) 1
set prefs(showfix) 1
set prefs(showtrack) 1
set prefs(statustimer) 2000
set prefs(maingeometry) "214x228+98+211"
set prefs(confgeometry) "353x228+325+211"
set prefs(background) white
set prefs(trackcolor) #a0a0a0
set prefs(crosscolor) #bbbbff
set prefs(datacolor) red
set prefs(statuscolor) red
set recvChs 12
set gps(epoch) [clock seconds]
set gpsTick ""
set udpOn 0
set playing 0
set state "Stopped"
set status ""
set sockets {}
set interfaceErr ""
set outdata ""
set outlines 0
set Info(stick) 0

set cPanel(names) {{ General} {    Colors} {    Display} { Connection} {    Serial / IP} {    Http} { Mode} {    Simulation} { NMEA} { Monitor}}
set cPanel(panels) {genf colorf dispf intf ipf httpf modef simuf nmeaf monif}
set disablable {$::confTop.scb $::confTop.modsim $::confTop.modfwd $::confTop.modfile $::confTop.filee $::confTop.fileb $::confTop.trige  $::confTop.tcpcb $::confTop.tcpcb $::confTop.udpcb $::confTop.mcstcb $::confTop.serpe $::confTop.serconfe $::confTop.tcppe $::confTop.remaddre $::confTop.rempe $::confTop.ttle $::confTop.httpcb  $::confTop.httppe $::confTop.httpre}

image create photo logo_img -data {
R0lGODlhWgAhAPcAAHOAWHOAc3OAenOFh3OPjnOTlXOYlXOdnHmKh3mmonyP
jn6PjoGAeoGAgYGTlYSAeoSFgYSFh4SKh4S1t4eAc4eAeoePjoqFeoqKh4qP
joqTlYqYlYqYnIyFc4yiooy+vY+Peo+Th4+dlY+inI+ioo/DxJKFbJKTh5KY
h5KdlZKinJKmopKmqZKrsJLIxJWFbJWKbJWKc5WPepWTgZWilZWinJWmnJWr
opXMy5iKZpiPepiYgZidjpiijpiwqZi1sJi1t5jR0pqKZpqYepqwqZq1sJq5
t5q+vZ2TbJ2dgZ2rlZ21sJ25sJ2+t52+vZ3DxJ3W0p3W2KCKX6CPX6CPZqCT
bKCYc6CdeqCmjqC1qaC5sKC+t6DDxKDIy6Db2KOKX6OPX6OTZqOTbKOdc6Oi
eqOrjqO1oqO5qaO+sKPIxKPR0qPb36aTZqaYbKaieqaigaa1nKa5oqa5qaa+
qabIvabMxKbRy6bWy6bW0qiPWKidbKirh6i5nKi+qajDsKjIt6jRxKjWy6jb
2Kjb36jp5quPUauYZquic6umequmgaurgaurh6uwjqu1lau5nKvDqavRvavR
y6vb0qvf36vk5qvp7a6wga6wjq6+nK7IsK7Mt67Rva7WxK7b0q7f0q7f2K7k
367p5q7p7bGTUbGdX7GdZrG5lbHby7Hf0rHk2LHp37Hp5rHu5rHu7bSTUbSY
UbSYWLSdX7SiX7SrbLSrc7SrerSwerSwgbS1h7S5jrS+nLTDlbTIorTIqbTM
sLTWvbTfywAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAABaACEAQAj/AP8JHEiwoMGD
CBMqXMjwCMOHEAeayTEnokJcrjhZZGFR4BEIDDJgePDPDgUKHTqcEHhHhgkT
MbIMHJKj4J8xUr60eTQwUZ5Chi4NNAVrVKxbsTpy7Mi0qdOEAgguFRjgqdWr
TIFg3coV4lSvDZwI/PGgSBoKNgQu6bDl3w6YaAhiqTnwihQ4Yb6EgfTPUZ5F
hwrlcfRvUyFTA2W5Eqjr1StXjmch/PpQzQgNG1YQDNG2q+cAEwZOBeC59NYg
AQCkNmC6NVfKrg0OESIEyVVdrnxtZHqEAYMHDypwaXIyZQcUAl/O2PGCzpkc
0GknEWhFypQvX9wIZJSnu2CBmEaN/yLlqjzBXq56KYS9kESDLgKBl6SQQiCN
DmkI9ngR999cglVIEcdAfDWSByIDafRPIaQMBMtiA/3yyi/rPWXEAyLMR0Ns
WwkAwAAIpFYChyQu9AEBUhFEWoksFjRBVQKN1uKMk9FoY4035iiajmYkUQZW
tSh1IyA5DLFVLq5osltTQKzgw0AU+EAEDUQMVAcPMsCgg0Bn0JQEFnIMBAcZ
b/BBUCaIHCLUQJmUMoossUA4EC0LsbdQAxHAd+ENXFBwARP/nNCBHXWYwINA
dQgUSBI5RDLQHHf9w8cXbAiUhx7/aGJIIQKNAstAjwnUiy2KyTLLLjhGxEED
A6nwQBpdUL5Qg0BMdKCFQGjEYMIOA/03EBVSSCFGHwPt0V0ejAykSSGYJCbn
P7y4wkuFFnXRgAQrQCCfSR2EEEIHIPxDRAw9KBFDcwIxSpAf1pEJxhcadaeH
IpsiZol4pZT37EN2QoTBBfPV95AZWOjoggMdedDAwsCpoaGODwEQ2j9LBZAA
xCSuGONAOERV0BMthCzyyCSXbPLJKKesMskLKDCyBSMHQDLGnmlM8UAHHEBz
bAWsyNEBNu/MYb9Ct0b0PwEBADs=
}
image create photo fix_img -data {
R0lGODlhCwALAPcAAAAAAAAAVQAAqgAA/wAkAAAkVQAkqgAk/wBJAABJVQBJ
qgBJ/wBtAABtVQBtqgBt/wCSAACSVQCSqgCS/wC2AAC2VQC2qgC2/wDbAADb
VQDbqgDb/wD/AAD/VQD/qgD//yQAACQAVSQAqiQA/yQkACQkVSQkqiQk/yRJ
ACRJVSRJqiRJ/yRtACRtVSRtqiRt/ySSACSSVSSSqiSS/yS2ACS2VSS2qiS2
/yTbACTbVSTbqiTb/yT/ACT/VST/qiT//0kAAEkAVUkAqkkA/0kkAEkkVUkk
qkkk/0lJAElJVUlJqklJ/0ltAEltVUltqklt/0mSAEmSVUmSqkmS/0m2AEm2
VUm2qkm2/0nbAEnbVUnbqknb/0n/AEn/VUn/qkn//20AAG0AVW0Aqm0A/20k
AG0kVW0kqm0k/21JAG1JVW1Jqm1J/21tAG1tVW1tqm1t/22SAG2SVW2Sqm2S
/222AG22VW22qm22/23bAG3bVW3bqm3b/23/AG3/VW3/qm3//5IAAJIAVZIA
qpIA/5IkAJIkVZIkqpIk/5JJAJJJVZJJqpJJ/5JtAJJtVZJtqpJt/5KSAJKS
VZKSqpKS/5K2AJK2VZK2qpK2/5LbAJLbVZLbqpLb/5L/AJL/VZL/qpL//7YA
ALYAVbYAqrYA/7YkALYkVbYkqrYk/7ZJALZJVbZJqrZJ/7ZtALZtVbZtqrZt
/7aSALaSVbaSqraS/7a2ALa2Vba2qra2/7bbALbbVbbbqrbb/7b/ALb/Vbb/
qrb//9sAANsAVdsAqtsA/9skANskVdskqtsk/9tJANtJVdtJqttJ/9ttANtt
Vdttqttt/9uSANuSVduSqtuS/9u2ANu2Vdu2qtu2/9vbANvbVdvbqtvb/9v/
ANv/Vdv/qtv///8AAP8AVf8Aqv8A//8kAP8kVf8kqv8k//9JAP9JVf9Jqv9J
//9tAP9tVf9tqv9t//+SAP+SVf+Sqv+S//+2AP+2Vf+2qv+2///bAP/bVf/b
qv/b////AP//Vf//qv///yH5BAEAAP8ALAAAAAALAAsAQAhGAP/92/ZIjRk1
j7YJ3KZGoEOBahg+FPEwYsOHECU6pOjwIkGDCBUKvBXRIcNb/yaJxLhtkpmN
Ijj+M6MS48JJ/0iuPPkvIAA7
}
image create photo simul_img -data {
R0lGODlhDwANAPcAAMDAwJmZ/2Zm/2ZmzDMzzAAAzAAAmQAAgAAAZgAAAAAA
AAAAAAAAAAAAAAAAAAAAAAIAAAAQAAAAjPsdd5L7HXeo8xIAAAABAKD0EgAA
ABQA5hf1dyQAAABIDRQAAAAUAOhPFAB49BIAAAAAAMD2EgAFkPd38NX2d///
///mF/V3eBf1d7IX9XcAAAAAAAAAAOD5EgDk9BIAM6rnd+T6EgCctRgA8LUY
AOD5EgAEAQAAAAAAAEmp53eX4ed3AAAAACD1EgAaIKdwAAAAAAAAAACctRgA
AAAAAOD5EgAEAQAAAAAAAAAAFACF9PV3ofT1dwgGFABAAKUAAQAAAAEAAABg
9RIAljnnd54553ectRgAKQAAAJfh53ectRgAQAClAAAAAAAw9RIAAAAAAGT7
EgAJSOl3ODDod/////+eOed3EpRBd0QApQABAAAAAAAAAEQApQAAAAAANzvU
dwDg/X/g9RIAZzvUd6z1EgBLO9R3AQQAADD2YwBE9mMAMPZjAAAAAAAAAAAA
FAAAAAEAAAAAAAAAAAAAABAAAAAAAAAAAQAAAJT1EgABAAAAJPoSAL/t1ndw
T9l3/////0s71HdrVNR3AAAAALI4NHeCAkYAAQQAAAIAAADY9kcARPZjAAEA
AADAQdR3AQQAAAAAAACCAkYADZ7UdzT2EgDuAAAAMQEAAAAAAAAgxGQAAgAA
ABIAAACAtRgAbwAAAAD6EgAEAAAAqPYSAD4FRwCo9hIABfoSAAQAAACo9hIA
hPYSAM/5RgCo9hIABfoSAEQApQAAAAAAAQAAAOD5EgDM3UQAqPYSAKQYSAAJ
+hIAAPoSACQAAAAgAQAAqPYSAAAAAACLZkp3AAAAAACb1QDoTxQAAAAUAKz0
EgDuADEBDPcSAAWQ93fg1fZ3/////7IX9XfUpud3AAAUAAgAFADlpud3AQAA
AAAAAAAAAAAAAAAAAAAAAABE2kQADlAUAGTKFgASUBQACvoSAP/////oTxQA
ntpEAA5QFABz0EQA6E8UACH5BAEAAAAALAAAAAAPAA0AQAhgAAEIFBigYICB
CAMMICDQgEMDAw8CUNiwYgGGCCMalJhR4QCEBD5qFAnxAAIABjBKDCBgIUYA
AwZwJNgyZsyME1sKLAARQMiRFRGcTAkU5UADBSLmXIhz4kyWNWMK4BgQADs=
}
image create photo playfile_img -data {
R0lGODlhEAAQAPcAAFZPq4GEA4GEQ4GEg7qvG8LEvPsEA/v6A/v6kwAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAQABAAQAhsAP8JFEBQoMGD
AgMgWMhwYQCDAxIyNECx4j8AFxFeBIDxIEeMAQiIHGmAwEOEChsiMIDgZESN
MDd2NPgx5sADOAkKCHDyYMqGFB/W/PdzogGhHCWqdAjRIM+nTw++tEkzo8AB
AzACwMqVqs2AADs=
}
image create photo nofix_img -data {
R0lGODlhDwAPAPcAAGg2mmg2z5tqz5uUxJudz6iDr68DOq8dZLsDL7tqhMK/
xM+UxM+dz8+25M+/us/Iz8/I+tV7pNsML+J7eugDD/wDBAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAPAA8AQAhrAP8JHEiwoEAA
EP5VWBhBIAQABB8mHLiwwgSCCxsIAMBRwESDDzZ2fKCQIEKBCwws/PeQoAMJ
KwdKNEizps2CBAIQgMCTwIGYAgUQwLgwgQCTExHEbDlQwICKBAkcpViBQgGe
EAgAYHCzZkAAOw==
}
image create photo icon_img -data {
R0lGODlhIAAgAPcAAAQEBHoEevr6+gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAgACAAQAh8AP8JHEiwYEEA
BhMqXPgPIcOHCx1CnBhgoMSKEydKzJhxI0eFGBsODPlRoceSEVFqVAnxJMuD
L0EGmEnyIICbOHPqDKDzJs+eKGnOjJnQJVGRRw0aJbo0ZtOXT1lGVTkVZdWS
VykKdFhTZciLRL+OPIqRa1KLZ2GmRcswIAA7
}
image create photo more_img -data {
R0lGODlhDgANAPcAAAhFou/m1fp9QgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAA0AQAgzAAEIHEiw4L9/
AhECUHgw4cGHEBsWnEgwokWGFyE6zCiR48ONHEFmFHnRYcKTCzF6BBAQADs=
}
image create photo import_img -data {
R0lGODlhDgANAPcAAARFogR9gefm1gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAA0AQAhBAAEIFPjvX4CD
BwsCKMjQIMIAChsWfAjx38CLGAdKdIgwIkOKDxduNMiQoESKHhuitLhxpciP
IFm2TGjx5UiFAQEAOw==
}
image create photo play_img -data {
R0lGODlhEgASAPcAAASCghaMjCCRkSmWljKWljKbmzKgoDygoEWlpU+qqliv
r2G0tGu5uXS+vn3Dw4fIyJDNzZrS0qPX16zc3Kzh4bbh4b/m5sjr69Lw8Nv1
9QAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAASABIAQAh+AP8JHEiwYEEL
ACAIdAAAwMAFDg1KnDiwAYABECpQcBAAQISJBQA0IJgAwAKKAi1A+IiyQICJ
Jg0KODAxQAKKExgcQNDAAsqfEyV0bAigAE4ANwsSeGkQAIOBDB4MNGC0oEiB
EwAIGIhga0GGCgsqADBhYsmLAhqOBMq27b+AADs=
}
image create photo stop_img -data {
R0lGODlhDgANAPcAAAR/gfr8/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAA0AQAgtAP8JHEiwIICD
CBMCGKiwYcGHDxsqZCgRIcWKCwViPHixYkeJHx1q3Bhy4r+AADs=
}
image create photo drop_img -data {
R0lGODlhDgANAPcAAARIogR+gvr7/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAA0AQAg5AAEIHEiw4L+D
CBEGOCgwocOF/wpKnEiRoMOEEBtePJgRwEWPDCOC3IhQ48aOJP+hPBlgocmU
AAICADs=
}
image create photo config_img -data {
R0lGODlhEAAOAPcAAARIogR+gvr7/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAQAA4AQAhRAP8JHPgvgMGD
BQMAIMjw38KBDwMUnChR4MGHDQVibCixIkGDCw0O7OiQ4MaGJxl6HAkSgMuQ
Ez8qpEhTYceZHDWaTJgQ4caQN32afEmUKMGAADs=
}
image create photo autozoom_img -data {
R0lGODlhDwAPAPcAAARIogR+gvr7/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAPAA8AQAhKAP8JHPgPAMGD
Bg8OTCjQYIAA/yA+LEgQgMWLGC0uzJixosKFHidKhMhQYcmLH0si/Ngw5EGS
DTVCJEhSI8WZA2GCZKnSY0yLAQEAOw==
}
image create photo pause_img -data {
R0lGODlhDgANAPcAAAR/gfr8/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAA0AQAg0AP8JHEiw4D8A
CAEITKhwIMOFCQ1KLPjwYESHFytCRLixYcePGDlaFAlS48iGJiumzHgxIAA7
}
image create photo general_img -data {
R0lGODlhDgA1APcAAAOBgQqGhhGGhhGLixmPjyCUlCeZmS+dnTaiojanpz6n
p0WsrEysrEywsFS1tVu6umK6umK+vmLDw2rDw3HIyHHNzXnNzYDR0YfW1o/b
25bf353k5J3p6aXp6azp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOADUAQAj/AP8JHEiw4L8L
AAIYAGDgnwIAAwg2APDAQYEEACAYzGChI4UHAA4YXACggoMDEgA0MGjQgAKW
MAtOpFDQAYAJEgHoBOCAJcmdOl8atFBAp4ELLCk0WMqUZsEFCP5BKPDvANWY
WP/ZBKrTqcADAAQqFIiBZ8ELaDHAREBA60sCImFikKA26z8MA+oSpLCg74IG
XgkqCOD2X4AEcikkeJCYAoUIBwIgrbkzQwQAkwky4AogwkjOO3EStLAwIQK9
AjMk1CgQJOGBYA2W7SmQJM6xVQEELs3w34OrBi+gLjgBIoEBBG4aPNDQwUsD
cQsScLA1ul2DDQZo1xmAcdYDQgn+SQTq+XpwzsP/QS+MoGHBDAEabI1o/jPX
8DEBRDXoAIHT9FUNcEEBDsC3X0EBSHAZBQJYJxAD4CkgoQQwoWXhhQRFgMCG
HHKIVEAAOw==
}
image create photo gpsfeed+_img -data {
R0lGODlhQQAOAPcAAAOBgQqGhhGLixmPjyCUlCeUlCeZmS+dnTaiojanpz6n
p0WsrEysrEywsEy1tVS1tVu6umK6umK+vmrDw3HIyHnNzYDR0YfW1o/b25bf
353k5KXp6azp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAABBAA4AQAj/AP8JHEiwoMGD
DxRMOMjQ4AMKDQdaADDg3wUBACD8QwDggECM/ygYWPgvAACBAAQIHAAAwz8I
CC78SylwgQKUAQRyRDAQA4AGB00i4JhxI4AACAwAMGARAIEHBwAw+Kd0qYUJ
ThcEUPlvQUcFSwUWWAoWAE8LFCB0pACRYFUAHXUCsBCxrt27ePMSPKrX4AWz
BAM8MNgAAEkJRTnS/VcYIka4CQbSnLnUAAEACyYGMGCA5QQKmAVO5ElBgoOl
EiQYtCABA2LViiUsaCsQwoIGFQY+UC0QQwMFGgdWWLCAt0AKCyL8e9i3+cDL
A4A6/8vz7gEBCioYgPjAwAUE2AeKmBQQXWBVA3QndL45EAKB8gMVCDiAtfo/
n9IJIl4gVyPHwf9cdgFWB+S22ExcgfQPVqo5VRMAF/xFQG2AXQbXhS4J9EBR
yyUGQG5dfXicAiZxNdlRqaVmwUQEpNhaBQCwB6N9+B1U1lIeCmCSVAKZRABG
0k0GI0WTbbhVWP+cdtRl9jVUwWYCRUWXYs5VqddVE8gkkZVcRhQQADs=
}
image create photo nmea_img -data {
R0lGODlhDgArAPcAAAOBgRGLixmPjxmUlCCUlCeZmS+dnTaiojanpz6np0Ws
rEywsEy1tVS1tVu6umK+vmrDw3HIyHnNzYDR0YfW1o/b25bf353k5KXp6azu
7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOACsAQAjvAP8JHEiw4D8C
ABIqVFhhYAGFAhREMHgQQIECAgAMKBAAQEOBCAEckEBRoIEGJUsaSIjAoIEA
MGN29DhwQYKbOHESJCBA4AMAE/45oElwAQCBC2Z+/MdR5sKlFAYkRJmy6kAK
DlYqNOCAQkEKYClEUJCRqMCwaMHu7PnvZ9ChSyNMNLi0pkWFBeYKnIBAYQCq
VkuGXMiwZISZDwwOBSAgqMHBMmEGfkz4KUWsWs2GDMDAcUkICWYCILBgZ+XC
kwcOrryUp0+gQjWzdRu7NdsIBLzCHXjAQeAJCfwCNmjUZ8i6/4yWBWCApOqE
zVNTDAgAOw==
}
image create photo simulation_img -data {
R0lGODlhDgBKAPcAAAOBgQqGhhGLixmPjxmUlCCUlCeZmS+dnS+iojaiojan
pz6np0WsrEywsEy1tVS1tVu6umK6umK+vmLDw2rDw3HIyHnNzYDNzYDR0YfW
1o/b25bb25bf353k5KXp6azp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAEoAQAj/AP8JHEiwoMAK
ABIWEAiAQUEDEAsIAJDAYIaEGAEsNGggoYABIA3+mwhAwj8AEEQOyIhxgMED
GB2KxECzJgaROAteAPBAYAIAFP4VCDDwIksAJzf+M5qxp0GSGS0exaj038qM
RDlC3Grg3wGZSwE0GPgz50CEIiMk9OmRoAWx/95uDEDArN27BNHa1Yu34MW+
gO1iQHoyLgANgYsCsDAQAuGCGiJLRuy34cCOODsmXIBzgecEBSxXZqkAJwaw
GRgXTJgSYYCfFQVSAFCBIeEDWQUGAIBgd0rWBitcMMt1q1ew/yZLlnr0boMD
IrmGfixSg4UCDkRS2E4BQmiRD8KHeQ+KU4L48AYHH5DAnfzDlGaZRk0sUEOA
+/jxdyUoQTXOtwoc4NJKKqU0wAQPUCfQRQEI4GABN9nVwGEFvSXAVhNB5F9f
8mGUFH2hBTAUAJwVhFAGA81GGUGhDXDARCWallOHa9nlwH4FNaBjAwvsJlIF
QFZgAYr0BQQAOw==
}
image create photo mode_img -data {
R0lGODlhDgAmAPcAAAOBgRGLixmPjxmUlCCUlCeZmS+dnTaioj6np0WsrEys
rEywsFS1tVu6umK+vmLDw2rDw3HIyHnNzYDNzYDR0YfW1o/b25bb25bf353k
5KXp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOACYAQAjdAP8JHEiwoMEK
DQwAAEBgYAQAFQY+AGBwAgALAyVQLPhQIMWOBv9FUDBgIYAACCQYlACBQsiX
MGMaxPiygsmbCxsKtLmwAIOIMwMIHTrUAMEAAA4sKACgQEgEOBuEZHoTQUiF
AQogJVCgQAKCDXC6NLjwX1MARg12Xcv2a8gKDg7kHMhz4YEHNRPeNNCA5r+6
OBnKDEnh5oCQFh0MhBoS7UABGw0yQCoAAkwGBAIQkHowqUALBCITFMBgYAKn
BQHj1EnQAQPLLwEIWACAwcmVAFxSDBtSQsOFaQcXDAgAOw==
}
image create photo http_img -data {
R0lGODlhDgAfAPcAAAOBgQqGhhGLixmPjyCUlCeZmS+dnTaioj6np0WsrEys
rEywsFS1tVu6umK6umK+vmrDw3HIyHnNzYDR0YfW1o/b25bf353k5KXp6azu
7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAB8AQAidAP8JHEiw4D8K
ABIqVEjA4EAFABwCkDDwQQCHBAcIwIhAowABAQAIOICxJEEIDSBgRLhwYQGM
AwaYnDlh4kAGEQtKEFmgpwAAPSm2bCkxwAIHCQBcnMnUJMuhABo6jMmUqsGn
Q6U6hPBSJ9SEFCsSrBBhKlSZAxFOuJlT4IK3cOMSjAABgoEADCAwCNDA4M6w
/xq0JfhgYQDATQUGBAA7
}
image create photo serial-ip_img -data {
R0lGODlhDgBAAPcAAAOBgSeLizaUlEWdnVSiomKsrGq1tXG6unnDw4fIyI/R
0ZbW1p3b26Xk5Kzp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAEAAQAj/AP8JFIhgoMGD
/w4UFKgA4UAFCxwKBECxIgAFAwAYBDCgY8eFEkOKRGixpEaGAA4IJACg4IGL
KBP8g5lyZEeECjgW0MhS5kGTFh0iKCCAYgACG4FSlAggQAIEAAQYLCqgYYCK
AwxeNbATKVWDCK5WVImwaAKOTR0CbShSgQG2BxGAlGiAgAIBBaCSHck35FEE
EfsKHsw3J9mMIRF8RHh1YVGkAxcAgAwVKcvAOQsIvLoSQOB/UI1OlIoQgUK/
AQwAIOr5oOGZEFs6REqxKuGfJuHSrYm7JMaThItO/ifAAELJDWXznmry822l
S29LF8m6KQHAfYeSnj4SOvDB3h2+Dv6HGCcA4+S/G1Q9dnBAADs=
}
image create photo ip_img -data {
R0lGODlhDgASAPcAAAOBgQqGhhGLixmPjyCUlCeZmS+dnTaioj6np0WsrEyw
sFS1tWK6umrDw3nNzYDR0YfW1ofb24/b25bf36Xp6azp6azu7gAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOABIAQAhZAP8JHEiwYMEA
BAQ2AODAoMOCECJKlPgQAoCLAAwQVFCgo0ePDwYewEjyYkODAwIIXMDwocuX
BEeWxHgSZsyZNB0KSPhvYU2CBgAcWBAAgMsICRAwsMnUYEAAOw==
}
image create photo serial_img -data {
R0lGODlhDgAnAPcAAAOBgQqGhhGLixmPjyCUlCeUlCeZmS+dnTaioj6np0Ws
rEysrEywsEy1tVS1tVu6umK+vmrDw3HIyHnNzYDR0YfW1ofb24/b25bf353k
5KXp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOACcAQAjsAP8JHEiw4EAB
AAAEsPDPgAGDBC4QADCAgsF/CBxqNEDhwcOBDAAQGBAAoQSDIgVGAHDgnwMA
FQZOZCAgwcSYBRso2HkgpcEHCRn8U/DgIsKESJESrBAAQNGVCSMQpCiwqUAD
AAxGcADhokAAQgmcBCC1IIUJaNN6JTiB7MULcOMKvDBVqgCpTg1KoLu2r1cH
FgVOMIg1wVcACAIEmGoYo9uVOP/1JDDx4cQCBiecXMs0qYOLecX+y1uw5wTK
iv32PZp081oJAAarNpo0oYQEWWcTVDxRMkyDBxYMNUyaIOuELXUbJKm4efN/
AQEAOw==
}
image create photo connection_img -data {
R0lGODlhDgBQAPcAAAOBgQqGhhGLixmPjxmUlCCUlCeZmS+dnS+iojaiojan
pz6np0WsrEysrEywsEy1tVS1tVu6umK+vmrDw3HIyHnNzYDR0YfW1pbb25bf
353k5KXp6azp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAFAAQAj/AP8JHEiwoEAJ
AABEGJgQAkEBASZcoDAAgEGDFgBQuFgQwAWDACr8C/BRI0gJAy+YLCgBA8GN
IB0KrBCSo0ALBxhcDBCgIAYAAghmTJgQpUEISJPKtMm0qVOmFww8ZcrAwAIL
HAdoHUDAAEyDDogSXUgQwAKDCyxOZbiUpsiLVa9ynEBXAgQBOkGKBdDgooC/
ARISuLgXQE+DCfYWuBh2ZEIHFxMHICAAwICLEMwKBJCAcOGEZDcXjqr25l6j
BSFKpFiaoIHXsKU2jbByrUEJDhxMuEibc+7EAHYzzEswLcHAb00fHjhBwEeB
ApYShLDAwXODFSFUkFCZcOiMyQfSihSL2rbovdc5qrxYoH2Byh7NMxzd1IHs
ghXyV5iQOLxABgAy0IB0BTkQGF4caUYSbVh1JFVEic31jwIM+FccUQckiMBA
DII0QIMGxGdQfgIJd9QAf0Fk2GJlLUWBiEJZIOOM/2CQXgI45tjZA50VRJ11
WQGgHXetMfRdTfidJt986DlV5HliLflUQAA7
}
image create photo colors_img -data {
R0lGODlhDgAvAPcAAAOBgQqGhhGLixmPjxmUlCCUlCeZmS+dnTaiojanpz6n
p0WsrEysrEywsFS1tVu6umK+vmLDw2rDw3HIyHnNzYDR0YfW1o/b25bf353k
5KXp6azp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOAC8AQAj/AP8JHEiwoMAE
AABUECgh4UKBFgAgGDgggEGBBhICaHCxo8EJAP6BXEAhZMGIBQo2TECwQgCN
KS86mEmTpoSBFQA8GJjR40AEJg1WeOjzgMaEASgQBHmzoAABPi8y/Rm0oAMB
AAY0NQihq1evE3BuHFigKsGMCRVctDCgLVYABy4usAghYdiTcF/+CwA1alQL
Dxo4uFvQaAAFDhqgvTCw4daBTwkC6DuwblyCDSxCJkyQggSiBl8iUAzXYEPG
Ah+YFQj0aAS/HXMePWoAdkECkwu85Kiyat2LuAkcwMrAJ+iCsmf7LVD7Iup/
FyBcBJASQAACmgk2/LcAgOfV/xQkCXTwr4BS2wMDAgA7
}
image create photo display_img -data {
R0lGODlhDgA0APcAAAOBgQqGhhGLixmPjxmUlCCUlCeZmS+dnTaioj6np0Ws
rEysrEywsEy1tVS1tVu6umK+vmrDw3HIyHnNzYDR0YfW1o/b25bb25bf353k
5KXp6azp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOADQAQAj/AP8JHEiw4EAH
AAIoePBAQQAADghKAGChwsMFBA0A2MiRYwWDDkKKHEmQQQAGBggcADDBoAQE
HA1AMPgPgAEKOHNqzGigp89/KAkGIDAAAIWVNG0OVMDSpYABFP49+DhQgAKC
FRwCoMrTp1cDBAEcMLizYAUHCRI44DrQgtu3FgTGHQhAQk0BNe3SbElzKViB
DAr0dVBUQIK+EBIrXhw2Qs0KE/USTAAhgmXJAhN0DJCAb9+CDT6LFtixNICw
CAzCJDgggEEAAwrCLJ16dFLTFcraJigggAWNGueGnTARY12DETaCVRBxN0EE
AqJv7A1A8GvHdD0P1NwxtvPXpjl+KR/4EADe1jQD2DXwoCb2sKWtj37/76nA
B0UTXi1Z3gD9zxAUsBFbtgUEADs=
}
image create photo monitor_img -data {
R0lGODlhDgA2APcAAAOBgRGLixmPjxmUlCCUlCeZmS+dnTaioj6np0WsrEys
rEywsFS1tVu6umK6umK+vmrDw3HIyHnNzYDNzYDR0YfW1o/b25bb25bf353k
5KXp6azu7gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAP///yH5BAEAAP8ALAAAAAAOADYAQAj/AP8JHEiw4MAK
DQwAWAjgAISBEQBIGMgAgEGCFiQIeFiwAsOP/wAYIIjgI4AHBj2atHgRooIB
Cwu0nCmQAoICCVpaEMBTAAEDFQxGDFCgaAAARScOPJqgwQGkNAei7LiSoUyB
KkVOTVk1JsEHJilcBCAgQgUIAAJcLMq2aNR/Jd/K/ZeA5VyCEYMKBDsTAoMI
cwkQuNsxgOHDR6FCTABzYQAEgAda+Figgd6OFTJXcPBUsUAEoEOHZtBxAQLS
LdM+kFBRgEEJAMQKbGCXIO2Psgljrfo2IleTanXvXhnyKuHEJl0PpBCbYm3b
BIhGNojUwj8KRw0yn1734oSPyoX/CtvJM7HxmXF1BwQAOw==
}

set gpsvars {HDOP DGPSdt DGPSid msec DD MO YYYY alt altu bear date ew fix ft-alt fu gheight gheightu knots lat lata latd latm latmi lats latf1 lon lona lond lonm lonmi lons lonf1 magnew magnfix magnvar maxsat mew mnorth nfix nrline nrmsg ns nsat sel time}
# create all gps variables, as found in NMEA sentences, and initiate them to ""
proc emptyGps {} {
	global gps
	foreach var $::gpsvars {
		set gps($var) ""
	}
	set gps(nfix) 0
	set gps(nsat) 0
	set gps(maxsat) 0
	set gps(fix)  "V"
	set gps(sel) "V"
}
emptyGps
# Set random initial satellite positions (elevations and azimuths).
# These are going to change as the satellites move in orbit
for {set i 1} {$i <= 32} {incr i} {
	set gps(el${i}_0) [expr {int(180.0*(rand() - 0.5))}]
	set gps(az${i}_0) [expr {int(360.0*rand())}]
};unset i
# Decode a gps sentence
proc Decode d {
	global gps
	set snt ""
	if {[regexp -- \
		{^\$(GPRMC),([0-9]{6})*,(A|V),([0-9]{2})*(([0-9]{2})\.([0-9]{4}))*,(N|S)*,([0-9]{3})*(([0-9]{2})\.([0-9]{4}))*,(E|W)*,(.*?),(.*?),([0-9]{6})*,(.*?),(.*?),(.*?)\*(.*)$} \
			$d m snt \
			gps(time) gps(fix) \
			gps(latd) gps(latm) gps(latmi) gps(latmf) gps(ns) \
			gps(lond) gps(lonm) gps(lonmi) gps(lonmf) gps(ew) \
			gps(knots) gps(bear) gps(date) \
			gps(magnvar) gps(magnew) gps(modi) cs] \
			|| [regexp -- \
		{^\$(GPGGA),([0-9]{6})*,([0-9]{2})*(([0-9]{2})\.([0-9]{4}))*,(N|S)*,([0-9]{3})*(([0-9]{2})\.([0-9]{4}))*,(E|W)*,(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\*(.*)$} \
			$d m snt \
			gps(time) \
			gps(latd) gps(latm) gps(latmi) gps(latmf) gps(ns) \
			gps(lond) gps(lonm) gps(lonmi) gps(lonmf) gps(ew) \
			gps(nfix) gps(nsat) gps(HDOP) \
			gps(alt) gps(altu) \
			gps(gheight) gps(gheightu) \
			gps(DGPSdt) gps(DGPSid) cs] \
			|| [regexp -- {^\$(GPGSV),([0-9-]*),([0-9-]*),([0-9-]*)(,.*)\*(.*)} \
				$d m snt nrmsg nrline gps(maxsat) rest cs]} {
		# form decimal fix
		switch -- $snt {
			"GPRMC" -
			"GPGGA" {
				if {$gps(time) eq ""} {
					# everything is empty, there is nothing to do.
					return
				}
				set ::gps(epoch) [clock scan "[string range $gps(date) 4 5][string range $gps(date) 2 3][string range $gps(date) 0 1] [string range $gps(time) 0 1]:[string range $gps(time) 2 3]:[string range $gps(time) 4 5]"]
				set gps(lon) [expr {double("$gps(lond)\.0") + double("$gps(lonm)")/60.0}]
				set gps(lon) [expr {($gps(ew)=="E")?$gps(lon):$gps(lon)*(-1)}]
				set gps(lons) [expr {"0.$gps(lonmf)" *60.0}]
				set gps(lat) [expr {double("$gps(latd)\.0") + double("$gps(latm)")/60.0}]
				set gps(lat) [expr {($gps(ns)=="N")?$gps(lat):$gps(lat)*(-1)}]
				set gps(lats) [expr {"0.$gps(latmf)" *60.0}]
			}
			"GPGSV" {
				set nrsat [expr {($nrline-1)*4}]
				set ii 1
				foreach {m sid el az sn} [regexp -all -inline -- \
					{,([0-9-]*),([0-9-]*),([0-9-]*),([0-9-]*)} $rest] {
					set gps(el[expr {$nrsat+$ii}]) $el
					set gps(az[expr {$nrsat+$ii}]) $az
					set gps(sn[expr {$nrsat+$ii}]) $sn
					incr ii
				}
			}
		}
	}
}
proc diopt {lat0 lon0 lat1 lon1} {
	global dtor rtod PI
	set lat0 [expr {$dtor*$lat0}]
	set lat1 [expr {$dtor*$lat1}]
	set lon0 [expr {$dtor*$lon0}]
	set lon1 [expr {$dtor*$lon1}]
	set L [expr {$lon0-$lon1}]
	# the following lines give errors
	set arc [expr {sin($lat0)*sin($lat1)+cos($lat0)*cos($lat1)*cos($L)}]
	if {[catch {set D [expr {acos($arc)}]}]} {
		set D 0
	}
	if {[expr {abs(abs($lat0) - $PI/2)}]<0.00000001} {
		#lat0 is at the pole
		set B [expr {($lat0>0)?$PI:0.0}]
	} elseif {$D>0.0} {
		set v [expr {(sin($lat1) - sin($lat0)*cos($D))/(cos($lat0)*sin($D))}]
		# correct inacuracies due to floating point truncation
		set v [expr {abs($v)>1.0?($v<0?-1:1.0):$v}]
		set B [expr {(abs($D)<0.00000001)?0.0:acos($v)}]
	} else {set B 0.0}
	# D is distance travelled in knots
	set D [expr {$rtod*$D*60.0}]
	# B is the bearing in degrees
	set B [expr {$rtod*$B}]
	if {$L>0.0} {set B [expr {360 - $B}]}
	return "$D $B"
}
proc sendData {} {
	global sockets prefs gps
	set prefs(period) [expr {$prefs(period)?int($prefs(period)):1}]
	if {$prefs(gpsmode) ne "forward"} {
		set ::gpsTick [after [expr {$prefs(period)*1000+$prefs(timecorrection)}] sendData]
	}
	set ::out ""
	set line "\n"
	if {$::state=="Paused"} {
		# flash play button red
		.l1.playb configure -selectcolor #ff8888
		after 200 {
			.l1.playb configure -selectcolor white
		}
	} 
	if {$prefs(gpsmode) eq "simulate"} {
		makeNMEAout
	} elseif {$prefs(gpsmode) eq "forward"} {
		set ::out $::outdata
		set ::outdata ""
		foreach line [split $::out \n] {
			#if {[regexp -line -- "^\\$$prefs(trigger).*$" $::out line]} {
				Decode $line
			#}
		}
	} elseif {($prefs(gpsmode) eq "playback") && ($::pbfile ne "")} {
		# we are in playback file mode. Look for a "trigger output"  NMEA sentense, i.e.GPRMC
		while {![string match "\$$prefs(trigger)*" $line]} {
			if {[eof $::pbfile]} {
				Stop
				return
			}
			set line [gets $::pbfile]
			# is this an NMEA sentense ? (should start with a $)
			if {[string match "\$*" $line]} {
				append ::out "$line\n"
			}
			Decode $line
		}
	}
	# send gpsdata to all connected sockets
	if {$prefs(tcp)} {
		foreach sock [lrange $sockets 1 end] {
			if {[catch {
				puts -nonewline $sock $::out
				flush $sock
			}]} {
				# tried to send data but got socket error.Close socket
				catch {close $sock}
			}
		}
	}
	# output to serial port if connected
	if {$prefs(serial) && ($prefs(gpsmode) ne "forward") && ($::fcomm ne "")} {
		puts -nonewline $::fcomm $::out
		flush $::fcomm
	}
	# output to udp address and port if user selected
	if {$prefs(udp) & $::udpOn} {
		# one sentence per udp packet
		foreach line [split $::out \n] {
			puts -nonewline $::u $line
			#flush $::u
		}
	}	
	$::confTop.monit delete 0.0 end
	$::confTop.monit insert end $::out
	$::confTop.monit tag add all 0.0 end
	$::confTop.monit tag configure all -lmargin2 10
	event generate . <<Draw>>
}
proc makeNMEAout {} {
	global out prefs gps
	if {$::state eq "Playing"} {MakeGpsData} \
	else {
		emptyGps
	}
	foreach s $prefs(sentences) {
		#set s [lindex $i 0]
		if {[info exists prefs($s)] && $prefs($s)} {
			append out [${s}proc]
		}
	}
}
proc BreakTime {t} {
	set gps(HH) [clock format $t -format %H]
	set gps(MM) [clock format $t -format %M]
	set gps(SS) [clock format $t -format %S]
	set gps(DD) [clock format $t -format %d]
	set gps(MO) [clock format $t -format %m]
	set gps(YYYY) [clock format $t -format %Y]
}
# Finds the distance D and the angle C from a location lat0,lon0 to lat1,lon1
proc MakeGpsData {} {
	global PI out gps prefs
	set out ""
	set time0 $gps(epoch)
	set gps(msec) [string range [clock clicks -milli] end-2 end]
	set gps(epoch) [clock seconds]
	set lat0 $gps(lat)
	set lon0 $gps(lon)
	set gps(time) [clock format $gps(epoch) -format %H%M%S]
	set gps(date) [clock format $gps(epoch) -format %d%m%y]
	BreakTime $gps(epoch)
	# make sure critical user defined values are non empty and double
	foreach var {prefs(aLon) prefs(aLat) prefs(anAlt) prefs(aRange)} {
		#	better 0 than empty
		if {[set $var]==""} {set $var 0.0}
		# better double than integer
		set $var [expr {double([set $var])}]
	}
	# the following call make lat lon alt (lla_xxx returns list of lat $lat lon $lon alt $alt)
	array set gps [lla_$prefs(shape) $gps(epoch) $prefs(aLat) $prefs(aLon) $prefs(anAlt) $prefs(aRange) [expr {$prefs(skip)+1}]]
	# make sure -90<=lat<=90 deg, and correct EW hemisphere
	if {$gps(lat)>90 || $gps(lat)<-90} {
		if {$gps(lat)>90} {
			set gps(lat) [expr {90-fmod($gps(lat),90)}]
		} else {
			set gps(lat) [expr {-90-fmod($gps(lat),90)}]
		}
		# this actually belongs to the other hemisphere
		set gps(lon) [expr {$gps(lon)+180}]
	}
	# make sure -180<=lon<=180 deg
	set gps(lon) [expr {$gps(lon)<=-180?180+fmod($gps(lon),180.0):$gps(lon)>180.0?-180+fmod($gps(lon),180):double($gps(lon))}]
	if {$gps(lat)>=0} {
		set gps(lata) $gps(lat)
		set gps(ns) "N"
	} else {
		set gps(lata) [expr {abs($gps(lat))}]
		set gps(ns) "S"
	}
	if {$gps(lon)>=0} {
		set gps(lona) $gps(lon)
		set gps(ew) "E"
	} else {
		set gps(lona) [expr {abs($gps(lon))}]
		set gps(ew) "W"
	}
	set gps(latd) [expr {int($gps(lata))}]
	set gps(latmi) [expr {int(($gps(lata)-$gps(latd))*60.0)}]
	set gps(lats) [expr {($gps(lata)-$gps(latd))*3600.0-$gps(latmi)*60}]
	set gps(latf1) "[format %02.0f $gps(latd)][format %07.4f [expr {60.0*($gps(lata) - int($gps(lata)))}]]"
	set gps(lond) [expr {int($gps(lona))}]
	set gps(lonmi) [expr {int(($gps(lona)-$gps(lond))*60.0)}]
	set gps(lons) [expr {($gps(lona)-$gps(lond))*3600.0-$gps(lonmi)*60}]
	set gps(lonf1) "[format %03.0f $gps(lond)][format %07.4f [expr {60.0*($gps(lona) - int($gps(lona)))}]]"
	if {[info exists lat0] && $lat0 != ""} {
		# find distance (mi) and bearing (deg), i.e. diopteusis
		set dio [diopt $lat0 $lon0 $gps(lat) $gps(lon)]
		set gps(dist) [lindex $dio 0]
		set dt [expr {$gps(epoch)-$time0}]
		if {$dt} {
			set gps(knots) [format %0.1f [expr {$gps(dist)*3600.0/$dt}]]
			set gps(speed) [format %.2f [expr {$gps(knots)*$::mps}]]
		} else {
			set gps(knots) 0.0
		}
		set gps(bear) [format %0.1f [lindex $dio 1]]
	} else {
		set gps(knots) ""
		set gps(bear) ""
	}
	set gps(magnvar) 5
	set gps(magnew) E
	set gps(magnfix) A
	set gps(HDOP) 5.6
	set gps(altu) M
	set gps(gheight) 34.5
	set gps(gheightu) M
	#set gps(sel) A
	# maxsat is the number of visible satellites
	set gps(maxsat) 0
	# gps(visible) is a list of visible satellites number and elevation
	set gps(visible) ""
	# do calculations for 32 satellites
	for {set i 1} {$i <= 32} {incr i} {
		# set the elevation for each satellite
		set satelv [expr {90*sin($PI*((10*$gps(el${i}_0)+$gps(epoch))%3600)/1800)}]
		if {!$prefs(signal)} {
			# user accepts no signal condition
			if {$satelv>30.0} {
				# Arbitrarily, we consider a satellite visible if its elevation > 30deg
				incr gps(maxsat)
				lappend gps(visible) [list $i $satelv]
			}
		} else {
			# user prevents no signal condition. Make all elevations positive
			set satelv [expr {abs($satelv)}]
			incr gps(maxsat)
			lappend gps(visible) [list $i $satelv]
		}
		set gps(el$i) $satelv
		set gps(az$i) [expr {((10*$gps(az${i}_0)+$gps(epoch))%3600)/10}]
		if {$::state eq "Playing"} {
			set gps(sn$i) [expr {($satelv>5)?[expr {60.0*sin($PI*($satelv-30)/180)}]:0}]
		} else {
			set gps(sn$i) 0
		}
	}

	if {$gps(maxsat)>=3 && ($::state eq "Playing")} {
		set gps(fix) A
		set gps(nfix) 1
		set gps(nsat) [expr {($gps(maxsat)>3)?4:3}]
	} else {
		emptyGps
	}
}

# Shape circle
proc lla_Circle {t alat alon analt arange sk} {
	global PI
	set v [expr {$PI*($t%360)*$sk/180}]
	set lon [expr {$alon+$arange*cos($v)}]
	set lat [expr {$alat+$arange*sin($v)}]
	set alt [format %.1f [expr {1000.0+1000.0*sin($v)}]]
	return [list lat $lat lon $lon alt $alt]
}
# Shape spiral
proc lla_Spiral {t alat alon analt arange sk} {
	global PI
	set v [expr {$PI*($t%360)*$sk/180}]
	set r [expr {$arange*(($t*$sk)%3600)/3600}]
	set lon [expr {$alon+$r*cos($v)}]
	set lat [expr {$alat+$r*sin($v)}]
	set alt [format %.1f [expr {1000.0+1000.0*sin($v)}]]
	return [list lat $lat lon $lon alt $alt]
}
# Shape square
proc lla_Square {t alat alon analt arange sk} {
	global PI
	set T 3600
	set dT [expr {double(($t*$sk)%$T)/$T}]
	set rad [expr {2*$PI*$dT}]
	if {$rad>7*$PI/4 || $rad<=$PI/4} {
		set lon [expr {$alon+$arange}]
		set lat [expr {$alat+$arange*sin(2*$rad)}]
	}
	if {$rad>$PI/4 && $rad<=3*$PI/4} {
		set lon [expr {$alon+$arange*sin(2*$rad)}]
		set lat [expr {$alat+$arange}]
	}
	if {$rad>3*$PI/4 && $rad<=5*$PI/4} {
		set lon [expr {$alon-$arange}]
		set lat [expr {$alat-$arange*sin(2*$rad)}]
	}
	if {$rad>5*$PI/4 && $rad<=7*$PI/4} {
		set lon [expr {$alon-$arange*sin(2*$rad)}]
		set lat [expr {$alat-$arange}]
	}
	set alt [format %.1f [expr {1000.0+1000.0*sin($rad)}]]
	return [list lat $lat lon $lon alt $alt]
}
proc GPRMCproc {} {
	global gps
	set line [join [list "GPRMC" $gps(time) $gps(fix) $gps(latf1) $gps(ns) $gps(lonf1) $gps(ew) $gps(knots) $gps(bear) $gps(date) $gps(magnvar) $gps(magnew) $gps(magnfix)] ","]
	return "\$$line*[makeChksum $line]\n"
}

proc GPGSVproc {} {
	global PI gps
	set lout ""
	#set sats [expr {($gps(maxsat)>$::recvChs)?$::recvChs:$gps(maxsat)}]
	set sats $gps(maxsat)
	set nrmsg [expr {$sats/4 + (($sats%4==0)?0:1)}]
	set nrline 1
	set count 0
	set line ""
	set visible [lrange [lsort -real -decr -index 1 $gps(visible)] 0 [expr {$::recvChs - 1}]]
	set visible [lsort -int -index 0 $gps(visible)]
	foreach pair $visible {
		incr count
		set i [lindex $pair 0]
		append line ",[format %02.0f $i],[format %02.0f $gps(el$i)],[format %03.0f $gps(az$i)],[format %02.0f $gps(sn$i)]"
		if {$count==4} {
			set line "GPGSV,$nrmsg,$nrline,$sats$line"
			append lout "\$$line*[makeChksum $line]\n"
			set line ""
			incr nrline
			set count 0
		}
	}
	if {$count>0 && $count<4} {
		set line "GPGSV,$nrmsg,$nrline,$sats$line"
		append lout "\$$line*[makeChksum $line]\n"
	}
	return $lout
}
proc GPGGAproc {} {
	global gps
	set line [join [list "GPGGA" $gps(time) $gps(latf1) $gps(ns) $gps(lonf1) $gps(ew) $gps(nfix) [format %02.0f $gps(nsat)] $gps(HDOP) $gps(alt) $gps(altu) $gps(gheight) $gps(gheightu) $gps(DGPSdt) $gps(DGPSid)] ","]
	return "\$$line*[makeChksum $line]\n"
}
# substitute user variables in user NMEA sentence
proc SubstVars {t {s {}}} {
	global gps
	set out ""
	set i -1
	foreach line [split $t \n] {
		incr i
		# change user variables to gps(..)
		set line [regsub -all -- {\$([^\.\$,\s\"<>]*)} $line {$gps(\1)}]
		# execution of commands is not allowed. Eliminate []
		set line [regsub -all -- {(\[|\])} $line {}]
		if {![catch {subst $line} line] && ($line ne {})} {
			append out $line $s
		} else {
			set ::userErrLine $i
		}
	}
	# forget the last \n
	return  [string trimright $out]
}
proc usersentenceproc {} {
	global gps prefs
	set out ""
	foreach line [split [SubstVars $prefs(userNMEA) \n] \n] {
		append out "\$$line*[makeChksum $line]\n"
	}
	return  $out
}
proc makeChksum {line} {
	set xor 0
	foreach char [split $line ""] {
		binary scan $char "c" dec
		set xor [expr {$xor ^ $dec}]
	}
	return [format %X $xor]
}
proc check {line} {
	regexp {\$(.*)\*(..)} $line m line1 chksum
	if {$chksum==[makeChksum $line1]} {return 0} else {return 1}
}
# Stack newly connected sockets in a list so that we can close them on exit.
proc AcceptConnect {sock addr port} {
	global sockets
	puts "Accept $sock from $addr port $port"
	lappend sockets $sock
	fconfigure $sock -buffering line -blocking 0 -translation {auto crlf}
	# Set a separate event to read what clients send us.
	fileevent $sock readable [list GetClientData $sock]
}
proc GetClientData {sock} {
	global sockets
	if {[eof $sock] || [catch {gets $sock line}]} {
		# end of file or abnormal connection drop
		if {[catch {close $sock}]} {
			puts "Client socket died."
		} else {
			puts "Close $sock"
		}
		set sockets [lremove $sockets $sock]
	} else {
		if {[string compare $line "quit"] == 0} {
			# A desperate users wants to terminate us
			# So, close all connections and exit
			ExitProc
		}
		if {[regexp {^(.+?)\s+(.+?)\s+(.*$)} $line m cmd path prot]} {
			switch -regexp -- $prot {
				{HTTP} {
					# the user agent is a browser
					switch -- $path {
						"/exit" {
							# A desperate users with a browser wants to terminate us
							# So, close all connections and exit
							ExitProc
						}
						default {
							if {[string match "GET" $cmd]} {
								if {[catch {
									puts $sock "Welcome to gpsdeed+ $::version  \n"
										}]} {
									puts "Connection reset by $sock"
								}
							}
						}
					}
				}
				default {
					# What to send to others, in reply to their message?
				}
			}
		}
	}
}
proc ToSerial {} {
}
proc FromSerial {} {
	global prefs outdata outlines
	set err [catch {gets $::fcomm} line]
	while {$line ne ""} {
		incr outlines
		# is this an NMEA sentense ? (should start with a $)
		if {!$err && [string match "\$*" $line]} {
			append outdata "$line\n"
			sendData
			set outlines 0
		}
		set err [catch {gets $::fcomm} line]
	}
}
proc StartSocketServer {} {
	global sockets prefs
	if {[catch {lappend sockets [socket -server AcceptConnect $prefs(tcpport)]}]} {
		append ::interfaceErr "Cannot start TCP server\n"
		append ::status $::interfaceErr
		return 0
	}
	return 1
}
proc StartUdpServer {} {
	global prefs
	set ::u [udp_open]
	set ::udpOn [catch {
		fconfigure $::u -blocking 0 -buffering none -translation {auto crlf} -ttl $prefs(ttl) \
			-remote [list $prefs(udpaddress) $prefs(udpport)]
	}]
	if {$::udpOn} {append ::interfaceErr "Cannot open UDP socket\n"}
	if {$prefs(mcast)} {
		set err [catch {
			fconfigure $::u -mcastadd $prefs(udpaddress) -ttl $prefs(ttl)
		}]
		if {$err} {append ::interfaceErr "Cannot activate Multicast\n"}
	}
	append ::status $::interfaceErr
	set ::udpOn [expr {!$::udpOn}]
	return $::udpOn
}
proc StartSerialServer {} {
	global prefs
	if {![catch {open $prefs(serialport) w+} ::fcomm]} {
		# serial port opened, file descriptor in fcomm
		set ::serialErr ""
		set mode "$prefs(serialspeed),n,8,1"
		catch {fconfigure $::fcomm -mode $mode -buffering line -blocking 0 -translation {auto crlf} } ::modeErr
		if {$prefs(gpsmode) eq "forward"} {
			catch {fileevent $::fcomm readable FromSerial} ::reventErr
			#catch {fileevent $::fcomm writable ToSerial} ::teventErr
			if {($::reventErr ne "")} {
				append ::interfaceErr "Error forwarding serial port\n"
				append ::status $::interfaceErr
				return
			}
		}
	} else {
		# we cannot connect to the serial port
		set ::serialErr $::fcomm
		set ::fcomm ""
		append ::interfaceErr "Cannot open serial port\n"
		append ::status $::interfaceErr
	}
}
proc StartHttpServer {} {
	global prefs
	set httpdLog stderr
	if {[Httpd_Server [file join $prefs(httproot)] $prefs(httpport)] eq ""} {
		append ::interfaceErr "Could not start Http\n"
		append ::status $::interfaceErr
	}
	puts stderr "Starting Tcl httpd server on 127.0.0.1 port $prefs(httpport)"
}
proc StopSocketServer {} {
	global sockets
	foreach socket $sockets {
		catch {flush $socket}
		catch {close $socket}
		set sockets [lremove $sockets $socket]
	}
}
proc StopUdpServer {} {
	catch {
		#fconfigure $::u -mcastdrop $prefs(udpaddress)
		close $::u
	}
	set ::udpOn 0
}
proc StopSerialServer {} {
	global prefs
	if {[info exists ::fcomm] && $::fcomm!=""} {
		fileevent $::fcomm readable {}
		catch {close $::fcomm}
	}
}
proc StopHttpServer {} {
	global Httpd
	catch {close $Httpd(listen)}
	catch {unset Httpd(port)}
	catch {unset Httpd(listen)}
	catch {unset Httpd(accepts)}
}
# Start the feed, or pause it
proc Start {} {
	global sockets prefs
	set ::interfaceErr ""
	switch -- $::state {
		"Paused" {
			# clicked play while paused
			set ::state "Playing"
		}
		"Playing" {
			# clicked pause while playing
			set ::state "Paused"
		}
		"Stopped" {
			if {$prefs(gpsmode)=="playback"} {
			if {![catch {open [file join $prefs(fpath) $prefs(fname)]} ::pbfile]} {
					fconfigure $::pbfile -blocking 0
				} else {
					set ::pbfile ""
					append ::interfaceErr "Cannot open file :\n$prefs(fname)\n"
					append ::status $::interfaceErr
				}
			}
			# initialize map
			foreach var {prefs(aLon) prefs(aLat) prefs(aRange)} {set $var [expr {double([set $var])}]}
			#StopSocketServer
			#StopUdpServer
			#StopSerialServer
			if {$prefs(tcp)} {StartSocketServer}
			if {$prefs(udp)} {StartUdpServer}
			if {$prefs(http)} {StartHttpServer}
			if {$prefs(serial) || $prefs(gpsmode) eq "forward"} {StartSerialServer}
			if {$::interfaceErr ne ""} {
				Stop
				set ::playing 0
				return
			}
			set ::state "Playing"
			if {$prefs(gpsmode) ne "forward"} {
				set ::gpsTick [after 0 sendData]
			}
		}
	}
}
# stop the feed
proc Stop {} {
	global sockets prefs
	after cancel $::gpsTick
	StopSocketServer
	StopSerialServer
	StopUdpServer
	StopHttpServer
	if {$prefs(gpsmode) eq "playback"} {catch {close $::pbfile}}
	set ::state "Stopped"
}	
proc lremove {args} {
	array set opts {-all 0 pattern -exact}
	while {[string match -* [lindex $args 0]]} {
	switch -glob -- [lindex $args 0] {
		-a*	{ set opts(-all) 1 }
		-g*	{ set opts(pattern) -glob }
		-r*	{ set opts(pattern) -regexp }
		--	{ set args [lreplace $args 0 0]; break }
		default {return -code error "unknown option \"[lindex $args 0]\""}
	}
	set args [lreplace $args 0 0]
	}
	set l [lindex $args 0]
	foreach i [join [lreplace $args 0 0]] {
	if {[set ix [lsearch $opts(pattern) $l $i]] == -1} continue
	set l [lreplace $l $ix $ix]
	if {$opts(-all)} {
		while {[set ix [lsearch $opts(pattern) $l $i]] != -1} {
		set l [lreplace $l $ix $ix]
		}
	}
	}
	return $l
}
# Validate input to avoid error alert
proc validateDbl {w v var} {
	if {[string is double $v] && ($v ne "")} {
		# set value to preferences
		set ::prefs($var) [expr {double("$v")}]
		$w configure -bg $::Color(Entry)
	} else {
		$w configure -bg red
	}
	return 1
}
proc validateInt {w v var} {
	if {[string is integer $v] && ($v ne "") && ($v ne 0)} {
		# set value to preferences
		set ::prefs($var) [expr {int("$v.0")}]
		$w configure -bg $::Color(Entry)
	} else {
		$w configure -bg red
	}
	return 1
}
proc saveGeometry {} {
	scan [winfo geometry .c] "%dx%d" ::cw ::ch
	set ::cw [expr {$::cw-$::brdrs}]
	set ::ch [expr {$::ch-$::brdrs}]
}
proc setColor {c v} {
	if {$c eq ""} {return 1}
	set err [catch {
		switch -- $v {
			background {.c configure -bg $c}
			trackcolor {.c itemconfig l1 -fill $c}
			datacolor {.c itemconfig display -fill $c}
			statuscolor {.c itemconfig status -fill $c}
			crosscolor {
				.c itemconfig fixx -fill $c
				.c itemconfig fixy -fill $c
			}
		}
	}]
	if {!$err} {set ::prefs($v) $c}
	return 1
}	
proc ChooseColor {v} {
	set color [tk_chooseColor -initialcolor $::color($v) -title "Choose color"]
	if {$color ne ""} {
		setColor $v $color
		set ::color($v) $color
	}
}
# Show configuration pane
proc ShowConf {top} {
	global prefs
	if {$prefs(showconf)} {
		#BuildConf $top
		ShowPanel $top $prefs(cpanel)
		$top.optlb selection set $prefs(cpanel)
		wm geometry $top $prefs(confgeometry)
		wm deiconify $top
		update
	} else {
		set prefs(confgeometry) [wm geometry $top]
		wm withdraw $top
	}
	# initialize validatable variables
	foreach var {aLon aLat aRange period skip tail} {
		set ::$var $prefs($var)
	}
	# initialize color variables
	foreach entry {background trackcolor crosscolor datacolor statuscolor} {
		set ::color($entry) $prefs($entry)
	}
}
proc ShowPanel {root p} {
	global prefs cPanel
	set base [expr {$root eq "."?"":$root}]
	pack forget $root.[lindex $cPanel(panels) $prefs(cpanel)]
	pack forget $root.[lindex $cPanel(panels) $prefs(cpanel)]si
	set align [list -in $root -anchor nw -side left -padx 0 -pady 0 -expand 1 -fill both]
	$root.usert delete 0.0 end
	$root.usert insert end $prefs(userNMEA)
	$root.usert tag add all 0.0 end
	$root.usert tag configure all -lmargin2 10
	$root.httpxt delete 0.0 end
	$root.httpxt insert end $prefs(httpxml)
	$root.httpxt tag add all 0.0 end
	eval pack $base.[lindex $cPanel(panels) $p]si -side right -anchor n -padx 1 -pady 10
	eval pack $base.[lindex $cPanel(panels) $p] $align
	set prefs(cpanel) $p
}
# Toggle map
proc Showmap {} {
	global prefs
	#if {$prefs(showconf)} {return}
	if {!$prefs(showmap)} {
		saveGeometry
		pack forget .c
	} else {
		.c configure -width $::cw -height $::ch
		pack .c -fill both -expand 1 
	}
	wm geometry . {};update
}
proc ShowFile {} {
	global prefs
	set file [tk_getOpenFile -initialdir $prefs(fpath) -title "Open GPS file"]
	if {$file ne ""} {
		set prefs(fpath) [file dir $file]
		set prefs(fname) [file tail $file]
	}
}
proc BuildConf {root} {
	set base [expr {$root eq "."?"":$root}]
	#option add *Button.font {Helvetica 6}
	toplevel $root
	wm geometry $root $::prefs(confgeometry)
	wm withdraw $root
	wm title $root "gpsfeed+ configuration"
	wm protocol $root WM_DELETE_WINDOW "ExitConf"
	listbox $root.optlb -height 6 -width 12 \
		-listvariable ::cPanel(names)
	pack $root.optlb -anchor nw -side left -expand 0 -fill y
	bind $root.optlb <<ListboxSelect>>  {
		::ShowPanel $::confTop  [$::confTop.optlb curselection]
	}
	# -General
	label $base.genfsi -text "General" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image general_img
	frame $base.genf -padx 15 -pady 15
		frame $base.genf0
		label $base.genlogo -image logo_img
		label $base.genver -text "$::version" -padx 5 -fg #808080
		message $base.genm1 -text "\n(C) 2004-2005, D.C.Zachariadis.\
			\nLicense : GPL\n\
			\nhttp://sf.net/projects/gpsfeed" \
			-width 250 -padx 0 -pady 0

		checkbutton $base.savecb -text "Save preferences on exit" \
			-variable ::prefs(save)
		#
		pack $base.genf0 -in $base.genf -anchor nw
		pack $base.genlogo $base.genver -in $base.genf0 -side left
		pack $base.genm1 -in $base.genf -anchor nw
		pack $base.savecb -in $base.genf -anchor nw -side bottom
		# -Colors
	label $base.colorfsi -text "Colors" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image colors_img
	frame $base.colorf -padx 15 -pady 15
		labelframe $base.colorfa -text "" -pady 0 -relief flat
		frame $base.colorf1
		frame $base.colorf2
		frame $base.colorf3
		frame $base.colorf4
		frame $base.colorf5
		label $base.backgl -text "Map" -width 14 -anchor w
		entry $base.backge -width 7 \
			-validate all -validatecommand {setColor %P "background"} \
			-textvariable ::color(background)
		button $base.backgb -text "..." -image more_img \
			-command {ChooseColor background}
		label $base.trackcl -text "Track" -width 14 -anchor w
		entry $base.trackce -width 7 \
			-validate all -validatecommand {setColor %P "trackcolor"} \
			-textvariable ::color(trackcolor)
		button $base.trackcb -text "..." -image more_img \
			-command {ChooseColor trackcolor}
		label $base.crossl -text "Cross hair" -width 14 -anchor w
		entry $base.crosse -width 7 \
			-validate all -validatecommand {setColor %P "crosscolor"} \
			-textvariable ::color(crosscolor)
		button $base.crossb -text "..." -image more_img \
			-command {ChooseColor crosscolor}
		label $base.datal -text "Data" -width 14 -anchor w
		entry $base.datae -width 7 \
			-validate all -validatecommand {setColor %P "datacolor"} \
			-textvariable ::color(datacolor)
		button $base.datab -text "..." -image more_img \
			-command {ChooseColor datacolor}
		label $base.statusl -text "Status line" -width 14 -anchor w
		entry $base.statuse -width 7 \
			-validate all -validatecommand {setColor %P "statuscolor"} \
			-textvariable ::color(statuscolor)
		button $base.statusb -text "..." -image more_img \
			-command {ChooseColor statuscolor}
	#
		pack $base.colorfa -in $base.colorf -anchor nw
		pack $base.colorf1 $base.colorf2 $base.colorf3 $base.colorf4 $base.colorf5 \
			-in $base.colorfa -anchor nw
		pack $base.backgl $base.backge -in $base.colorf1 \
			-side left -anchor w
		pack $base.backgb -in $base.colorf1 -side left -anchor w -padx 5
		pack $base.trackcl $base.trackce -in $base.colorf2 \
			-side left -anchor w
		pack $base.trackcb -in $base.colorf2 -side left -anchor w -padx 5
		pack $base.crossl $base.crosse -in $base.colorf3 \
			-side left -anchor w
		pack $base.crossb -in $base.colorf3 -side left -anchor w -padx 5
		pack $base.datal $base.datae -in $base.colorf4 \
			-side left -anchor w
		pack $base.datab -in $base.colorf4 -side left -anchor w -padx 5
		pack $base.statusl $base.statuse -in $base.colorf5 \
			-side left -anchor w
		pack $base.statusb -in $base.colorf5 -side left -anchor w -padx 5
	# -Display
	label $base.dispfsi -text "Display" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image display_img
	frame $base.dispf -padx 15 -pady 15
		labelframe $base.dispfa -text "" -relief flat
		labelframe $base.dispfb -text "Onscreen data fields" -relief groove
			frame $base.dispf1
			frame $base.dispf2
			frame $base.dispf3
			frame $base.dispf4
			frame $base.dispf5
			checkbutton $base.showtrack -text "Track" -width 6 -anchor w \
				-variable ::prefs(showtrack) -command Display
			checkbutton $base.showfix -text "Fix" -width 6 -anchor w \
				-variable ::prefs(showfix) -command Display
			checkbutton $base.showcross -text "Crosshair" -width 0 -anchor w \
				-variable ::prefs(showcross) -command Display
			checkbutton $base.showstatus -text "Status" -width 6 -anchor w \
				-variable ::prefs(showstatus) -command Display
			checkbutton $base.showdata -text "Data" -width 6 -anchor w \
				-variable ::prefs(showdata) -command Display
			checkbutton $base.showtime -text "Time" -width 11 -anchor w \
				-variable ::prefs(showtime) -command Display
			checkbutton $base.showcoord -text "Coordinates" -width 11 -anchor w \
				-variable ::prefs(showcoords) -command Display
			checkbutton $base.showalt -text "Altitude" -width 11 -anchor w \
				-variable ::prefs(showalt) -command Display
			checkbutton $base.showbear -text "Bearing" -width 11 -anchor w \
				-variable ::prefs(showbear) -command Display
			checkbutton $base.showknots -text "Knots" -width 11 -anchor w \
				-variable ::prefs(showknots) -command Display
			checkbutton $base.shownrsat -text "Visible sats" -width 11 -anchor w \
				-variable ::prefs(shownrsat) -command Display
	#
		pack $base.dispfa -in $base.dispf -anchor nw -fill x 
		pack $base.dispf1 -in $base.dispfa -anchor nw
		pack $base.dispf2 -in $base.dispfa -anchor nw
		pack $base.dispfb -in $base.dispfa -anchor nw -fill none -padx 0 -pady 10
		pack $base.dispf3 -in $base.dispfb -anchor nw
		pack $base.dispf4 -in $base.dispfb -anchor nw
		pack $base.dispf5 -in $base.dispfb -anchor nw
			pack $base.showstatus $base.showdata -in $base.dispf1 \
				-anchor nw -side left
			pack $base.showtrack $base.showfix $base.showcross -in $base.dispf2 \
				-anchor nw -side left
			pack $base.showtime $base.showcoord -in $base.dispf3 -anchor nw -side left
			pack $base.showalt $base.showbear -in $base.dispf4 -anchor nw -side left
			pack $base.showknots $base.shownrsat -in $base.dispf5 -anchor nw -side left
		# -Output interfaces
	label $base.intfsi -text "Connection" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image connection_img
	frame $base.intf -padx 15 -pady 15
		frame $base.inttf
		checkbutton $base.scb -text "Serial" \
			-variable ::prefs(serial)
		checkbutton $base.tcpcb -text "TCP" \
			-variable ::prefs(tcp)
		checkbutton $base.udpcb -text "UDP" \
			-variable ::prefs(udp)
		checkbutton $base.mcstcb -text "Multicast" -padx 20 \
			-variable ::prefs(mcast)
		checkbutton $base.httpcb -text "Http" \
			-variable ::prefs(http)
		pack $base.inttf -in $base.intf -anchor nw
		pack $base.scb $base.tcpcb $base.udpcb $base.mcstcb $base.httpcb -in $base.inttf \
			-anchor nw -padx 0 -pady 0
		# -Serial
	label $base.serfsi -text "Serial" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image serial_img
	frame $base.serf -padx 15 -pady 15
	# -IP
	label $base.ipfsi -text "IP" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image serial-ip_img
	frame $base.ipf -padx 15 -pady 15
		labelframe $base.serfa -text "Serial parameters" -pady 0
		frame $base.serf1
		frame $base.serf2
		label $base.serpl -text "Port :" -width 17 -anchor w
		entry $base.serpe -width 12 \
			-textvariable ::prefs(serialport)
		label $base.serconfl -text "Speed :" -width 17 -anchor w
		entry $base.serconfe -width 12 \
			-textvariable ::prefs(serialspeed)

		labelframe $base.iptcpf -text "TCP parameters" -pady 0
		labelframe $base.ipudpf -text "UDP parameters"
		frame $base.ipf3
		frame $base.ipf4
		frame $base.ipf5
		frame $base.ipf6
		frame $base.ipf7
		label $base.tcppl -text "Port :" -width 17 -anchor w
		entry $base.tcppe -width 12 \
			-textvariable ::prefs(tcpport)
		label $base.remaddrl -text "Remote addr :" -width 17 -anchor w
		entry $base.remaddre -width 12 \
			-textvariable ::prefs(udpaddress) 
		label $base.rempl -text "Remote port :" -width 17 -anchor w
		entry $base.rempe -width 12 \
			-textvariable ::prefs(udpport)                                         
		label $base.ttll -text "Time to live (TTL) :" -width 17 -anchor w
		entry $base.ttle -width 12 \
			-textvariable ::prefs(ttl)
		#
		pack $base.serfa -in $base.ipf -anchor nw
		pack $base.serf1 $base.serf2 -in $base.serfa
		pack $base.serpl $base.serpe -in $base.serf1 -anchor nw -side left
		pack $base.serconfl $base.serconfe -in $base.serf2 -anchor nw -side left

		pack $base.iptcpf $base.ipudpf -in $base.ipf \
			-anchor nw -pady 0 -fill none
		pack $base.ipf3 -in $base.iptcpf -fill none
		pack $base.ipf5 $base.ipf6 $base.ipf7 -in $base.ipudpf -fill x
		pack $base.tcppl $base.tcppe -in $base.ipf3 -anchor nw -side left
		pack $base.remaddrl $base.remaddre -in $base.ipf5 -anchor nw -side left
		pack $base.rempl $base.rempe -in $base.ipf6 -anchor nw -side left
		pack $base.ttll $base.ttle -in $base.ipf7 -anchor nw -side left
	# -Http
	label $base.httpfsi -text "Http" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image http_img
	frame $base.httpf -padx 15 -pady 2
		canvas $base.httpf.sp1 -height 13 -width 0
		frame $base.httpf1
		frame $base.httpf2
		label $base.httppl -text "Http port :" -width 12 -anchor w
		entry $base.httppe -width 6 \
			-textvariable ::prefs(httpport)
		label $base.httprl -text "Http root dir :" -width 12 -anchor w
		entry $base.httpre -width 15 \
			-textvariable ::prefs(httproot)
		label $base.httpxl -text "XML reply :" -width 12 -anchor w
		text $base.httpxt -tabs 20
		bind $root.httpxt <KeyRelease> {
			set prefs(httpxml) [string trimright [$::confTop.httpxt get 0.0 end]]
		}
	#
		pack $base.httpf.sp1 $base.httpf1 $base.httpf2 -in $base.httpf \
			-anchor nw -fill x
		pack $base.httpxl -in $base.httpf -anchor nw
		pack $base.httpxt -in $base.httpf -anchor nw -expand 1 -fill both
		pack $base.httppl $base.httppe -in $base.httpf1 -side left -anchor nw
		pack $base.httprl -in $base.httpf2 -side left -anchor nw
		pack $base.httpre -in $base.httpf2 -side left -anchor nw -fill x -expand 1
	# -NMEA
	label $base.nmeafsi -text "NMEA" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image nmea_img
	frame $base.nmeaf -padx 15 -pady 2
		canvas $base.nmeaf.sp1 -height 13 -width 0
		frame $base.nmeafa
		labelframe $base.sentef -text "Sentences"
		frame $base.nmeaf1
		frame $base.nmeaf2
		frame $base.nmeaf3
		checkbutton $base.nosig -text "Prevent no signal" -padx 5 -anchor nw \
			-variable ::prefs(signal)
		label $base.perl -text "Period :" -padx 5
		entry $base.pere -width 3 \
			-textvariable ::period \
			-validate all -validatecommand {validateInt %W %P "period"}
		checkbutton $base.ggacb -text "GPGGA" -width 6 -anchor nw \
			-variable ::prefs(GPGGA)
		checkbutton $base.gllcb -text "GPGLL" -width 6 -anchor nw -state disabled \
			-variable ::prefs(GPGLL)
		checkbutton $base.hchdgcb -text "HCHDG" -width 6 -anchor nw -state disabled \
			-variable ::prefs(HCHDG)
		checkbutton $base.rmccb -text "GPRMC" -width 6 -anchor nw \
			-variable ::prefs(GPRMC)
		checkbutton $base.zdacb -text "GPGSV" -width 6 -anchor nw \
			-variable ::prefs(GPGSV)
		checkbutton $base.pgrmzcb -text "PGRMZ" -width 6 -anchor nw -state disabled \
			-variable ::prefs(PGRMZ)
		checkbutton $base.usercb -text "User defined" -width 10 -anchor nw \
			-variable ::prefs(usersentence)
		text $base.usert -wrap word -width 40
		bind $root.usert <KeyRelease> {
			set prefs(userNMEA) [string trimright [$::confTop.usert get 0.0 end]]
		}
		#
		pack  $base.nmeaf.sp1 $base.nmeafa -in $base.nmeaf -anchor nw -fill both
		pack $base.sentef -in $base.nmeaf -anchor nw -fill both -expand 1
		pack $base.nmeaf1 $base.nmeaf2 $base.nmeaf3 -in $base.sentef -anchor nw
		pack $base.nosig $base.perl $base.pere -in $base.nmeafa -side left -anchor w
		#pack  $base.naml $base.name -in $base.nmeafa -side left -anchor nw
		pack $base.ggacb $base.gllcb $base.hchdgcb -in $base.nmeaf1 -anchor nw -side left
		pack $base.rmccb $base.zdacb $base.pgrmzcb -in $base.nmeaf2 -anchor nw -side left
		pack $base.usercb -in $base.sentef -anchor nw
		pack $base.usert -in $base.sentef -anchor nw -expand 1 -fill both
	# -GPS mode
	label $base.modefsi -text "GPS Mode" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image mode_img
	frame $base.modef -padx 15 -pady 15
		frame $base.modgpsf
		frame $base.filef -padx 0
		frame $base.trigf -padx 0
		radiobutton $base.modfwd -text "Serial to IP" \
			-variable ::prefs(gpsmode) -value "forward"
		radiobutton $base.modsim -text "Simulation" \
			-variable ::prefs(gpsmode) -value "simulate"
		radiobutton $base.modfile -text "Playback" \
			-variable ::prefs(gpsmode) -value "playback"
		label $base.spacer1 -width 3 -padx 0
		label $base.spacer2 -width 3 -padx 0
		label $base.filel -text "File :" -width 7 -anchor w -padx 0
		entry $base.filee -width 10 \
			-textvariable ::prefs(fname)
		button $base.fileb -text "..." -image import_img \
			-command {ShowFile}
		label $base.trigl -text "Trigger :" -width 7 -anchor w -padx 0
		entry $base.trige -width 10 \
			-textvariable ::prefs(trigger)
		pack $base.modgpsf -in $base.modef \
			-anchor nw -fill none
		pack $base.modgpsf -in $base.modef
		pack $base.modsim $base.modfile -in $base.modgpsf -anchor nw
		pack $base.filef $base.trigf -in $base.modgpsf -anchor nw
		pack $base.spacer1 $base.filel $base.filee -in $base.filef -side left
		pack $base.fileb -in $base.filef -side left -padx 5
		pack $base.spacer2 $base.trigl $base.trige -in $base.trigf -side left
		pack $base.modfwd -in $base.modgpsf -anchor nw
	# -Simulation
	label $base.simufsi -text "Simulation" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image simulation_img
	frame $base.simuf -padx 15 -pady 15
		frame $base.simuf0
		frame $base.simufa
		frame $base.lonf
		frame $base.latf
		frame $base.rangf
		frame $base.skipf
		frame $base.tailf
		label $base.lonl -text "Longitude :" -width 10 -anchor w
		entry $base.lone -width 10 \
			-textvariable ::aLon \
			-validate all -validatecommand {validateDbl %W %P "aLon"}
		label $base.latl -text "Latitude :" -width 10 -anchor w
		entry $base.late -width 10 \
			-textvariable ::aLat \
			-validate all -validatecommand {validateDbl %W %P "aLat"}
		label $base.rangl -text "Range :" -width 10 -anchor w
		entry $base.rangee -width 10 \
			-textvariable ::aRange \
			-validate all -validatecommand {validateDbl %W %P "aRange"}
		label $base.skipl -text "Skip :" -width 10 -anchor w
		entry $base.skipe -width 10 \
			-textvariable ::skip \
			-validate all -validatecommand {validateInt %W %P "skip"}
		label $base.taill -text "Track tail :" -width 10 -anchor w
		entry $base.taile -width 10 \
			-textvariable ::tail \
			-validate all -validatecommand {validateInt %W %P "tail"}
		frame $base.shapef0
		radiobutton $base.circrb -text Circle \
			-variable ::prefs(shape) -value "Circle"
		radiobutton $base.squrb -text Square \
			-variable ::prefs(shape) -value "Square"
		radiobutton $base.spirb -text Spiral \
			-variable ::prefs(shape) -value "Spiral"
		radiobutton $base.randrb -text Random -state disabled \
			-variable ::prefs(shape) -value "Random"
		# -Parameters
		labelframe $base.randf -text "Parameters"
		frame $base.randfa
		frame $base.randfb
		frame $base.randf1
		frame $base.randf2
		frame $base.randf3
		frame $base.randf4
		label $base.rphil -text "Phi :" -width 5 -anchor w
		entry $base.rphie -width 7 -state disabled \
			-textvariable ::prefs(rphi)
		label $base.rdistl -text "Dist :" -width 5 -anchor w
		entry $base.rdiste -width 7 -state disabled \
			-textvariable ::prefs(rdist)
		label $base.ravgl -text "Avg :" -width 5 -anchor w
		entry $base.ravge -width 7 -state disabled \
			-textvariable ::prefs(ravg)
		label $base.rlookl -text "Look :" -width 5 -anchor w
		entry $base.rlooke -width 7 -state disabled \
			-textvariable ::prefs(rlook)
		#
		pack $base.simuf0 -in $base.simuf -anchor nw
			pack $base.shapef0 -in $base.simuf0 -anchor nw -side left
				pack $base.simufa -in $base.simuf0 -anchor nw -side left -padx 10
					pack $base.lonf $base.latf $base.rangf $base.skipf $base.tailf -in $base.simufa -anchor nw -fill x
						pack $base.lonl $base.lone -in $base.lonf -side left
						pack $base.latl $base.late -in $base.latf -side left
						pack $base.rangl $base.rangee -in $base.rangf -side left
						pack $base.skipl $base.skipe -in $base.skipf -side left
						pack $base.taill $base.taile -in $base.tailf -side left
					pack $base.squrb $base.circrb $base.spirb -in $base.shapef0 -anchor nw
					pack $base.randrb -in $base.shapef0 -anchor nw
		# -Random
		#pack $base.randf -in $base.simuf -anchor nw
		#	pack $base.randfa -in $base.randf -anchor nw -side left -pady 3 -padx 5
		#		pack $base.randf1 $base.randf2 $base.randf3 $base.randf4 -in $base.randfa \
		#			-anchor nw -fill x -padx 5
		#			pack $base.rphil $base.rphie -in $base.randf1 -anchor nw -side left
		#			pack $base.rdistl $base.rdiste -in $base.randf2 -anchor nw -side left
		#			pack $base.ravgl $base.ravge -in $base.randf3 -anchor nw -side left
		#	pack $base.randfb -in $base.randf -anchor nw -side left -pady 3 -padx 5
		#		pack $base.randf4 -in $base.randfb \
		#			-anchor nw -fill x -padx 0
		#			pack $base.rlookl $base.rlooke -in $base.randf4 -anchor nw -side left
	# -Monitor
	label $base.monifsi -text "Monitor" -wrap 0 -height 150 -anchor n \
		-font {Tahoma 12 bold} -image monitor_img
	frame $base.monif -padx 15 -pady 2
		text $base.monit -wrap word -width 40
		#
		pack $base.monit -in $base.monif -anchor nw -expand 1 -fill both
}

proc ExitConf {} {
	set ::prefs(showconf) 0
	wm withdraw $::confTop
}
proc Display {} {
	global prefs gps
	if {$prefs(showfix)} {
		if {$::state eq "Playing" && ($gps(fix) eq "A")} {
			.c itemconfig fixm -image fix_img
		} else {
			.c itemconfig fixm -image nofix_img
		}
	} else {
		.c itemconfig fixm -image {}
	}
	if {$prefs(showcross)} {
		.c itemconfig fixx -fill $prefs(crosscolor)
		.c itemconfig fixy -fill $prefs(crosscolor)
		.c raise fixm fixy
	} else {
		.c itemconfig fixx -fill $prefs(background)
		.c itemconfig fixy -fill $prefs(background)
		.c raise l1 fixy
	}
	if {$prefs(showtrack)} {
		.c itemconfig l1 -fill $prefs(trackcolor)
	} else {
		.c itemconfig l1 -fill $prefs(background)
		.c lower l1 
	}
	if {$prefs(showdata)} {
		$::confTop.showtime configure -state normal
		$::confTop.showcoord configure -state normal
		$::confTop.showalt configure -state normal
		$::confTop.showbear configure -state normal
		$::confTop.showknots configure -state normal
		$::confTop.shownrsat configure -state normal
		if {$prefs(showtime)} {
			set display [clock format $gps(epoch) -format "%Y.%m.%d %H:%M:%S"]
		}
		if {$prefs(showcoords)} {
			append display "\n"
			if {$gps(fix) eq "A"} {
				append display [ format "%03.0f\xb0%02.0f'%05.2f\"$gps(ew)\n %02.0f\xb0%02.0f'%05.2f\"$gps(ns)"  $gps(lond) $gps(lonmi) $gps(lons) $gps(latd) $gps(latmi) $gps(lats)
					]
			} else {
				append display "- - -  - - - no fix"
			}
		}
		if {$prefs(showalt)} {
			append display "\nAlt  : "
			if {$gps(alt) ne ""} {append display [format "%-.1f $gps(altu)" $gps(alt)]}
		}
		if {$prefs(showbear)} {
			append display "\nBear : "
			if {$gps(bear) ne ""} {append display [format "%-.1f\xb0" $gps(bear)]}
		}
		if {$prefs(showknots)} {
			append display "\nSpeed: "
			if {$gps(knots) ne ""} {append display [format "%-.1f knots\n       %-.1f kmph" $gps(knots) [expr {$gps(knots)*1.852}]]
			} else {append display "\n"}
		}
		if {$prefs(shownrsat)} {
			append display "\nSatel: "
			if {$gps(maxsat) ne ""} {append display [format "%-.0f" $gps(maxsat)]}
		}
	} else {
		set display ""
		$::confTop.showtime configure -state disabled
		$::confTop.showcoord configure -state disabled
		$::confTop.showalt configure -state disabled
		$::confTop.showbear configure -state disabled
		$::confTop.showknots configure -state disabled
		$::confTop.shownrsat configure -state disabled
	}
	.c itemconfig display -text $display
	.c coords display [expr {[.c canvasx 1]+2}] [expr {[.c canvasy 1]+$::ch}]
}
proc Draw {} {
	global prefs gps
	if {$::playing==0} {return}
	# d is the distance travelled between last 2 fixes
	set d [expr {$gps(knots)==""?0.0:$gps(knots)}]
	# calculate kmph
	set kmph [expr {$d*1.852/$prefs(period)}];
	if {$gps(fix) eq "A"} {
		if {$prefs(showtrack)} {.c itemconfig fixm -image fix_img}
		foreach {x1 y1 x2 y2} [concat [lrange $::coo end-1 end] $gps(lon) $gps(lat)] {}
		.c Move fix $gps(lon) $gps(lat)
		lappend ::coo $gps(lon) $gps(lat)
		eval .c lPlot l1 $::coo
		# limit trail
		if {[llength $::coo]>int($prefs(tail))%3600} {
			set ::coo [lrange $::coo end-[expr {$prefs(tail)*2-1}] end]
		}
		set size [expr {[.c cget -meets]=="x" ? $::cw : $::ch}]
		.c center - - [expr {$size*1000.0*($prefs(skip)+1)/($kmph+0.1)}]
	}
	if {$prefs(zoommode)} {
		# do autozoom stuff
		if {$gps(fix) eq "A"} {.c center $gps(lon) $gps(lat) -}
	} else {
		.c setview [expr {$prefs(aLon)-$prefs(aRange)*1.1}] [expr {$prefs(aLat)+$prefs(aRange)*1.1}] [expr {2.2*$prefs(aRange)}] [expr {2.2*$prefs(aRange)}]
	}
	Display
	update
}
proc Redraw {} {
	global prefs gps
	if {$::playing==0 || $gps(fix)=="" || $gps(fix)=="V"} {return}
	scan [winfo geometry .c] "%dx%d" ::cw ::ch
	set ::cw [expr {$::cw-$::brdrs}]
	set ::ch [expr {$::ch-$::brdrs}]
	if {!$prefs(showmap) || !$::playing} {return}
	eval .c lPlot l1 $::coo
	.c Move fix $gps(lon) $gps(lat)
	if {$prefs(zoommode)} {
		.c center $gps(lon) $gps(lat) -
	} else {
		.c setview [expr {$prefs(aLon)-$prefs(aRange)*1.1}] [expr {$prefs(aLat)+$prefs(aRange)*1.1}] - -
	}
}
proc SetStatus args {
	.c coords status [expr {[.c canvasx 1]+5}] [expr {[.c canvasy 1]+5}]
	if {$::status eq ""} {.c itemconfig status -text "";return}
	.c itemconfig status -text $::status
	after $::prefs(statustimer) {set ::status ""}
}
proc StateGui args {
	global prefs
	switch -- $::state {
		"Stopped" {
			.l1.playb configure -image play_img -text "Stopped"
			foreach widget $::disablable {
				eval $widget configure -state normal
			}
			.c itemconfig fixm -image {}
		}
		"Paused" {
			if {$prefs(showfix)} {.c itemconfig fixm -image nofix_img}
			.l1.playb configure -text "Paused" -image pause_img
		}
		"Playing" {
			if {$prefs(showfix)} {.c itemconfig fixm -image fix_img}
			.l1.playb configure -text "Playing" -image play_img
			foreach widget $::disablable {
				eval $widget configure -state disabled
			}
		}
	}
	update
}
proc Setzoom {} {
	global prefs gps
	if {$gps(fix)=="" || $gps(fix)=="V"} {return}
	if {$prefs(zoommode)==1} {
		.c center $gps(lon) $gps(lat) -
	} else {
		.c setview [expr {$prefs(aLon)-$prefs(aRange)*1.1}] [expr {$prefs(aLat)+$prefs(aRange)*1.1}] [expr {2.2*$prefs(aRange)}] [expr {2.2*$prefs(aRange)}]
	}
	.c coords display [expr {[.c canvasx 1]+5}] [expr {[.c canvasy 1]+$::ch}]
	Redraw
}
proc ToggleStick {} {
	if {$::Info(stick)} {
		wm overrideredirect . 0
	} else {
		wm overrideredirect . 1
	}
	set ::Info(stick) [expr {!$::Info(stick)}]
}
proc FetchFile {} {
		return  [join [split [tk_getOpenFile -title "Open file"  -initialdir $::prefs(fpath) -initialfile ""] \\] /]
}
proc ReadPrefs {} {
	global prefs
	if {[file exists "gpsfeed+.cfg"]} {
		set f [open "gpsfeed+.cfg" r]
		while {![eof $f]} {
			set line [gets $f]
			if {$line!={}} {
				catch {array set prefs $line}
			}
		}
		close $f
	}
	# we must format the user NMEA sentences
	set prefs(userNMEA) [join $prefs(userNMEA) \n]
	set prefs(httpxml) [join $prefs(httpxml) \n]
}
proc SavePrefs {} {
	global prefs
	if {!$prefs(save)} {
		set f [open "gpsfeed+.cfg" a+]
		puts $f "save 0"
		close $f
		return
	}
	set prefs(maingeometry) [wm geometry .]
	if {$prefs(showconf)} {
		set prefs(confgeometry) [wm geometry $::confTop]
	}
	set prefs(userNMEA) [split $prefs(userNMEA) \n]
	set prefs(httpxml) [split $prefs(httpxml) \n]
	set f [open "gpsfeed+.cfg" w]
	foreach pref [lsort [array names prefs]] {
		puts $f "$pref [list $prefs($pref)]"
	}
	close $f	
}
proc ExitProc {} {
	StopSocketServer
	StopUdpServer
	SavePrefs
	exit
}

array set Color {Entry #ffffff Text #ffffff Options #ffffff bg #b0f0f0 topbg #e0f8f8 fg #008080 bd #b0d0d0}

wm focusmodel . passive
wm minsize . 144 30
wm resizable . 1 1
wm title . "gpsfeed+"
wm protocol . WM_DELETE_WINDOW "ExitProc"

#if {$tcl_platform(platform) ne "windows"} {
	#wm iconphoto . -default icon_img
	option add *Font {Helvetica 8 bold}
	option add *Listbox*Font {Helvetica 8}
	option add *Entry*Font {Helvetica 8}
	option add *Text*Font {Helvetica 8}
	option add *Labelframe*Font {Helvetica 9}
#} else {
#	option add *Font {Verdana 7 bold}
#	option add *Listbox*Font {Arial 8}
#	option add *Entry*Font {Arial 8}
#	option add *Text*Font {Arial 8}
#	option add *Labelframe*Font {Arial 9}
#}
option add *selectBorderWidth 0
option add *selectBackground $::Color(fg)
option add *selectForeground #ffffff
option add *highlightThickness 0
option add *background $::Color(topbg)
option add *foreground $::Color(fg)
option add *activeBackground #ffffff
option add *relief flat
option add *overRelief flat
option add *conf*padx 0
option add *conf*pady 0

option add *conf*borderWidth 0
option add *conf*Button*anchor nw
option add *conf*background $::Color(bg)
option add *conf*highlightBackground $::Color(bd)
option add *conf*highlightColor $::Color(fg)
option add *conf*background $::Color(bg)
option add *conf*disabledBackground $::Color(bg)
option add *overRelief groove
option add *Label*foreground $::Color(fg)
option add *Labelframe*borderWidth 0
option add *Button*borderWidth 2
option add *Button*background $::Color(bg)
option add *Checkbutton*borderWidth 2
option add *Radiobutton*borderWidth 2
option add *Checkbutton*foreground $::Color(fg)
option add *Entry*width 6
option add *Entry*background $::Color(Entry)
option add *Entry*foreground #000000
option add *Entry*highlightThickness 1
option add *Text*borderWidth 2
option add *Text*background $::Color(Text)
option add *Text*foreground #000000
option add *Text*highlightThickness 1
option add *Listbox*background $::Color(Text)
option add *Listbox*highlightThickness 1

option add *l1*activeBackground $::Color(topbg)
option add *l1*relief flat
option add *l1*offRelief flat
option add *l1*indicatorOn 0
option add *l1*selectColor #ffffff
option add *l1*Width 20
option add *l1*Height 20
# -Toolbar
frame .l1 -width 0 -relief flat -highlightthickness 1 -highlightbackground $::Color(bd)
radiobutton .l1.playb -text "Playing" -textvariable state -image play_img \
	-variable ::playing -value 1 -command Start
radiobutton .l1.stopb -text "Stop" -image stop_img \
	-variable ::playing -value 0 -command Stop 
checkbutton .l1.confb -text "Config" -image config_img \
	-variable ::prefs(showconf) -command {ShowConf $::confTop}
checkbutton .l1.mapb -text "Map" -image drop_img \
	-variable ::prefs(showmap) -command Showmap
#radiobutton .l1.pbb -text "Playback" -image playfile_img \
#	-variable ::prefs(gpsmode) -value "playback"
#radiobutton .l1.simb -text "Simulate" -image simul_img \
#	-variable ::prefs(gpsmode) -value "simulate"
checkbutton .l1.autoz -text "Time zoom" -image autozoom_img \
	-variable ::prefs(zoommode) -command Setzoom
	
pack .l1.playb -anchor nw -expand 0 -fill none -side left
pack .l1.stopb -anchor nw -expand 0 -fill none -side left
pack .l1.autoz -anchor nw -expand 0 -fill none -side left
pack .l1.mapb -anchor nw -expand 0 -fill none -side left
pack .l1.confb -anchor nw -expand 0 -fill none -side left
#pack .l1.sp1 -anchor nw -expand 0 -fill none -side left
#pack .l1.pbb -anchor nw -expand 0 -fill none -side right
#pack .l1.simb -anchor nw -expand 0 -fill none -side right
pack .l1 -anchor nw -expand 0 -fill x -padx 0 -side top

set coo [list $prefs(aLon) $prefs(aLat) $prefs(aLon) $prefs(aLat)]
ReadPrefs

pack [carto .c -bg $::prefs(background) -width 140 -height 140 -highlightbackground $::Color(bd) -highlightthickness 1] \
	-fill both -expand 1
.c configure -autoscale meet
.c create line 0 0 0 0 -tag l1 -fill $::prefs(trackcolor)
.c create crosshair fix -fill $::prefs(crosscolor)
.c create image 0 0 -tag {fix fixm} -image fix_img
.c create text 0 0 -tag display -fill $::prefs(datacolor) -font {Courier 9} -anchor sw
.c create text 5 5 -tag status -fill $::prefs(statuscolor) -font {Courier 9} -anchor nw
bind .c <<CartoRedraw>> Redraw
bind . <<Draw>> Draw
bind . <Button-3> ToggleStick
Setzoom
wm geometry . $prefs(maingeometry)
wm deiconify .
update
set confTop .conf
BuildConf $::confTop
if {$prefs(showconf)} {ShowConf $::confTop}
set ::brdrs [expr {2*([.c cget -highlightthickness]+[.c cget -borderwidth])}]
scan [winfo geometry .c] "%dx%d" ::cw ::ch
set ::cw [expr {$::cw-$::brdrs}]; set ::ch [expr {$::ch-$::brdrs}]
Display
trace variable ::status w SetStatus
trace variable ::state w StateGui
focus -force .