/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Named Object
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#if !defined(___NAMED_OBJECT__H___)
#define ___NAMED_OBJECT__H___

#include <opccore/common/class_pointers.h>
#include <opccore/common/object_id.h>
#include <memory>
#include <string>

namespace Gefest
{
  class NamedObject
  {
  public:
    DEFINE_CLASS_POINTERS(NamedObject);

  public:
    NamedObject()
      : ID(Common::GenerateNewID())
    {
    }

    virtual ~NamedObject()
    {
    }

    //----------------------------------------------------------------
    /// @brief Getting name of the object
    virtual std::string GetName() const
    {
      return Name;
    }

    //----------------------------------------------------------------
    /// @brief Setting name of the object
    /// @param name New name of the object
    virtual void SetName(const std::string& name)
    {
      Name = name;
    }

    //----------------------------------------------------------------
    /// @brief Geting id othe object
    virtual Common::ObjectID GetID() const
    {
      return ID;
    }

  private:
    typedef NamedObject SelfType;

    NamedObject(const SelfType&);
    SelfType& operator=(const SelfType&);

  private:
    std::string Name;
    const Common::ObjectID ID;  ///!< ID of the object globally unique inside one programm
  };

  template <class NamedObjectDerived>
  class NamedObjectEqualByID
  {
  public:
    NamedObjectEqualByID(const Common::ObjectID& id)
      : ID(id)
    {
    }

    bool operator()(const std::shared_ptr<NamedObjectDerived>& object) const
    {
      return object->GetID() == ID;
    }
 
    bool operator()(const std::pair<std::string, std::shared_ptr<NamedObjectDerived> >& object) const
    {
      return object.second->GetID() == ID;
    }
 
  private:
    const Common::ObjectID& ID;
  };
}

#endif // ___NAMED_OBJECT__H___

