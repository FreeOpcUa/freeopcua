/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Daemon class.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/common/class_pointers.h>

namespace OpcUa
{

  class Daemon
  {
  public:
    DEFINE_CLASS_POINTERS(Daemon);

  public:
    Daemon();
    ~Daemon();

    void Daemonize(const std::string& str);
    void WaitForTerminate();
  };

} // namespace OpcUa

