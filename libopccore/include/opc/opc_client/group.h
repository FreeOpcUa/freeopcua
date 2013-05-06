/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Group interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __group_h_488eca0d_e5b1_4514_9dbd_66a510f7a8d6
#define __group_h_488eca0d_e5b1_4514_9dbd_66a510f7a8d6

#include <opc/opc_client/items_manager.h>
#include <opc/common/class_pointers.h>
#include <opc/common/noncopyable.h>

namespace OpcClient
{
  class GroupProperties 
    : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(GroupProperties);

  public:
    virtual bool IsActive() const = 0;
    virtual void SetIsActive(bool isActive) = 0;
    
    virtual unsigned GetUpdateRate() const = 0;
    virtual void SetUpdateRate(unsigned rate) = 0;

    virtual unsigned GetTimeBias() const = 0;
    virtual void SetTimeBias(unsigned bias) = 0;

    virtual float GetDeadband() const = 0;
    virtual void SetDeadband(float deadband) = 0; 
  };

  class Group
    : public ItemsManager
  {
  public:
    DEFINE_CLASS_POINTERS(Group);

  public:
    /// @TODO it's may be better move name and private members to properties class.
    virtual std::string GetName() const = 0;
    virtual void SetName(const std::string& newName) = 0;

    virtual void SetIsPrivate(bool value) = 0;
    virtual bool GetIsPrivate() const = 0;

    virtual GroupProperties::SharedPtr GetProperties() const = 0;
  };

  Group::UniquePtr CreateGroup();
}

#endif // __group_h_488eca0d_e5b1_4514_9dbd_66a510f7a8d6

