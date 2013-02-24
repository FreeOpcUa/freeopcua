/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Server class
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/opc_client/server.h>
#include <opccore/opc_client/errors.h>

#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/errors.h>
#include <opccore/managers/error_manager/manager.h>

#include <iostream>
#include <tr1/functional>

#include "../managers/common_managers/common_managers.h"

namespace
{

  class ServerEngineStarter
  {
  public:
    ServerEngineStarter()
      : AddonsManager(Common::GetAddonsManager())
    {
      RegisterServerManagers();
      Common::RegisterCommonManagers();
      AddonsManager->Start();
    }

    virtual ~ServerEngineStarter()
    {
      try
      {
        AddonsManager->Stop();
      }
      catch (const Common::Error& error)
      {
        // TODO trace error;
      }
    }
  private:
    Common::AddonsManager::SharedPtr AddonsManager;
  };

  class ServerImpl : public OpcClient::Server
  {
  public:
    ServerImpl();

    virtual OpcClient::Group::SharedPtr AddGroup(const std::string& name);
    virtual OpcClient::Group::SharedPtr GetGroupByName(const std::string& name) const;
    virtual OpcClient::Group::SharedPtr FindGroupByName(const std::string& name) const;
    virtual void RemoveGroup(const std::string& name);

    virtual std::vector<OpcClient::Group::SharedPtr> GetPublicGroups() const;
    virtual std::vector<OpcClient::Group::SharedPtr> GetPrivateGroups() const;

    virtual OpcClient::ServerStatus GetStatus() const;
    virtual Common::Error GetLastError() const;
    virtual std::string GetErrorString(unsigned error) const;

  private:
    virtual std::vector<OpcClient::Group::SharedPtr> GetGroups(bool privateOnly) const;

  private:
    const std::unique_ptr<ServerEngineStarter> EngineStarter;
    std::vector<OpcClient::Group::SharedPtr> Groups;
  };

  ServerImpl::ServerImpl()
    : EngineStarter(new ServerEngineStarter())
  {
  }

  OpcClient::Group::SharedPtr ServerImpl::AddGroup(const std::string& name)
  {
    if (FindGroupByName(name))
    {
      THROW_ERROR1(GroupAlreadyExist, name);
    }
    OpcClient::Group::SharedPtr group(OpcClient::CreateGroup());
    group->SetName(name);
    Groups.push_back(group);
    return group;
  }

  OpcClient::Group::SharedPtr ServerImpl::FindGroupByName(
    const std::string& name) const
  {
    for (std::vector<OpcClient::Group::SharedPtr>::const_iterator it = Groups.begin(); it != Groups.end(); ++it)
    {
      if ((*it)->GetName() == name)
      {
        return *it;
      }
    }
    return OpcClient::Group::SharedPtr();
  }
 
  OpcClient::Group::SharedPtr ServerImpl::GetGroupByName(
    const std::string& name) const
  {
    if (OpcClient::Group::SharedPtr group = FindGroupByName(name))
    {
      return group;
    }
    THROW_ERROR1(GroupNotFound, name);
  }

  void ServerImpl::RemoveGroup(
    const std::string& name)
  {
    for (std::vector<OpcClient::Group::SharedPtr>::iterator it = Groups.begin(); it != Groups.end(); ++it)
    {
      if ((*it)->GetName() == name)
      {
        Groups.erase(it);
        return;
      }
    }
    THROW_ERROR1(GroupNotFound, name);
  }

  bool IsGroupNotEqualByPrivate(const OpcClient::Group::SharedPtr& group, bool isPrivate)
  {
    return group->GetIsPrivate() != isPrivate;
  }

  std::vector<OpcClient::Group::SharedPtr> ServerImpl::GetGroups(bool isPrivate) const
  {
    std::vector<OpcClient::Group::SharedPtr> groups(Groups.begin(), Groups.end());
    groups.erase(
      std::remove_if(groups.begin(), groups.end(), std::bind(IsGroupNotEqualByPrivate, std::placeholders::_1, isPrivate)),
      groups.end());

    std::cout << std::endl << "isPrivate = " << isPrivate << "count = " << groups.size() << std::endl;
    return groups;
  }
  std::vector<OpcClient::Group::SharedPtr> ServerImpl::GetPublicGroups() const
  {
    return GetGroups(false);
  }

  std::vector<OpcClient::Group::SharedPtr> ServerImpl::GetPrivateGroups() const
  {
    return GetGroups(true);
  }

  OpcClient::ServerStatus ServerImpl::GetStatus() const
  {
    return OpcClient::ServerStatus();
  }

  Common::Error ServerImpl::GetLastError() const
  {
    ErrorManager::Manager::SharedPtr errorManager = Common::GetAddon<ErrorManager::Manager>(Common::ADDON_ID_ERROR_MANAGER);
    return errorManager->GetLastError();
  }

  std::string ServerImpl::GetErrorString(unsigned error) const
  {
    THROW_ERROR(NotImplemented);
    return std::string();
  }
}

namespace OpcClient
{
  Server::UniquePtr CreateServer()
  {
    return Server::UniquePtr(new ServerImpl);
  }
}

