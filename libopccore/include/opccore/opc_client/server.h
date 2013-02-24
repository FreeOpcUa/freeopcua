/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Server class
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __server_h_c7469dd7_c591_474e_91f9_2944b15a5c5c
#define __server_h_c7469dd7_c591_474e_91f9_2944b15a5c5c

#include <opccore/opc_client/group.h>

#include <opccore/common/exception.h>
#include <opccore/common/noncopyable.h>
#include <string>
#include <time.h>
#include <vector>

/// @brief Function that should register opc server managers in Common::AddonsManager.
/// @note it should be realized in concrete dll.

extern "C" void RegisterServerManagers();

namespace OpcClient
{
  enum ServerState
  {
    STATE_RUNNING = 0U,
    STATE_FAILED = 1U,
    STATE_NOCONFIG = 2U,
    STATE_SUSPENDED = 3U,
    STATE_TEST = 4U,
    STATE_COMM_FAULT = 5U,
  };

  struct ServerStatus
  {
    time_t StartTime;
    time_t CurrentTime;
    time_t LastUpdateTime;

    ServerState State;
    unsigned GroupCount;
    unsigned BandWidth;

    unsigned MajorVersion;
    unsigned MinorVersion;
    unsigned BuildNumber;
    std::string VendorInfo;

  public:
    ServerStatus()
      : StartTime(0)
      , CurrentTime(0)
      , LastUpdateTime(0)
      , State(STATE_RUNNING)
      , GroupCount(0)
      , BandWidth(~0)
      , MajorVersion(0)
      , MinorVersion(0)
      , BuildNumber(0)
    {
    }
  };

  class Server
    : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(Server);

  public:
    /// @brief add new group
    /// @param name name of the new group
    /// @return pointer to new added group
    /// @throws GroupAlreadyExist when group already exist 
    virtual Group::SharedPtr AddGroup(const std::string& name) = 0;

    /// @brief Get group by name
    /// @param name name of requested group
    /// @return non nul pointer to found group
    /// @throws GroupNotFound if group not found
    virtual Group::SharedPtr GetGroupByName(const std::string& name) const = 0;

    /// @brief find group by name
    /// @param name name of the requested group
    /// @return non nul pointer to the found group or nul pointer if group not found
    virtual Group::SharedPtr FindGroupByName(const std::string& name) const = 0;

    /// @brief Remove group by name
    /// @param name name of the group to be removed
    /// @throws GroupNotFound if requested group not found
    virtual void RemoveGroup(const std::string& name) = 0;

    /// @brief Get list of public groups
    virtual std::vector<Group::SharedPtr> GetPublicGroups() const = 0;
    /// @brief Get list of private groups
    virtual std::vector<Group::SharedPtr> GetPrivateGroups() const = 0;

    /// @brief get status of server
    virtual ServerStatus GetStatus() const = 0;
    /// @brief get last error that was happened during server work
    virtual Common::Error GetLastError() const = 0;
    /// @bried Get description of error by it code.
    virtual std::string GetErrorString(unsigned error) const  = 0;
  };

  Server::UniquePtr CreateServer();

} // namespace OpcClient

#endif // __server_h_c7469dd7_c591_474e_91f9_2944b15a5c5c

