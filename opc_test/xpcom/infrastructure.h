// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __INFRASTRUCTURE_TEST_CASE_H__
#define __INFRASTRUCTURE_TEST_CASE_H__

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <memory>
#include <string>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/plugin/DynamicLibraryManager.h>
#include <cppunit/Message.h>


class DynamicLibrary
{
public:
  DynamicLibrary(const std::string& libraryFilename);
  virtual ~DynamicLibrary();

  template <typename FuncType> 
  FuncType findFunction(const std::string& funcName)
  {
    return reinterpret_cast<FuncType>(find(funcName));
  }
private:
  void* find(const std::string& funcName);

private:
  void* Library;
};

class InfrastructureTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(InfrastructureTest);
  // XPCOM tests
  CPPUNIT_TEST(xpcom);

  // Windows tests
#ifdef _WIN32
  CPPUNIT_TEST(comRegisterServer);
  CPPUNIT_TEST(comUnregisterServer);
#endif // _WIN32

  CPPUNIT_TEST_SUITE_END();

public:
  virtual void setUp();
  virtual void tearDown();
  
private:
  void xpcom();

#ifdef _WIN32
  // Regestering COM server in registry
  void comRegisterServer();

  // Unregestering COM server
  void comUnregisterServer();
#endif // _WIN32

private:
  std::unique_ptr<DynamicLibrary> lib;
};

#endif // __INFRASTRUCTURE_TEST_CASE_H__
