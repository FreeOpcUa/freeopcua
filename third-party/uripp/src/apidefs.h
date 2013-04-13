/*
Copyright (c) 2009 zooml.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef apidefs_h
#define apidefs_h
#ifdef _WIN32
// Symbols to define for the preprocessor when compiling the code:
//    URIPP_STATIC: to create static library
//    URIPP_EXPORTS: to create DLL library
//    nothing: when using (importing) the library in an application
#  ifdef URIPP_STATIC
#    define URIPP_API
#    define URIPP_TMPL
#  else
#    ifdef URIPP_EXPORTS
#      define URIPP_API __declspec(dllexport)
#      define URIPP_TMPL
#    else
#      define URIPP_API __declspec(dllimport)
#      define URIPP_TMPL extern
#    endif
#  endif
#  pragma warning (disable: 4251)
#  pragma warning (disable: 4793)
// Convert from linux API.
#  define strcasecmp _stricmp
#else
#  define URIPP_API
#  define URIPP_TMPL
#endif /* _WIN32 */
#endif
