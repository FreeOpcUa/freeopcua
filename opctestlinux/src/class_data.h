// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __TESTOPC_COM_SERVER_CLASS_DATA_H__
#define __TESTOPC_COM_SERVER_CLASS_DATA_H__

#include <string>

namespace OPCTest
{
  class CoClassData;

  void InitializeClassData(int argc, char** argv);
  const CoClassData& GetClassData();

  class CoClassData
  {
  public:
   CoClassData(const std::string& filename,
//               const std::string& name,
               const std::string& guid
//               const std::string& contractId,
//               const std::string& progId
              );

    virtual ~CoClassData();

    virtual std::string GetFilename() const;
//    virtual std::string GetName() const;
    virtual std::string GetClsId() const;
//    virtual std::string GetContractId() const;
//    virtual std::string GetProgId() const;
  
  private:
    const std::string Filename;
//    const std::string ClassName;
    const std::string ClassGuid;
//    const std::string ClassContractId;
//    const std::string ClassProgId;
  };
}

#endif // __TESTOPC_COM_SERVER_CLASS_DATA_H_

