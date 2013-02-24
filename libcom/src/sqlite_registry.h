/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Com registry implementation based on SQLite dabase.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __sqlite_registry_h_f79ce3aa_c10bde7a_9b45_4c66_a145_2c05ca9899bf
#define __sqlite_registry_h_f79ce3aa_c10bde7a_9b45_4c66_a145_2c05ca9899bf

#include <registry.h>

#include <memory>

namespace Com
{

  std::unique_ptr<ClassRegistry> CreateSqliteRegistry(const std::string& databasePath);

} // namespace Com

#endif // __sqlite_registry_h_f79ce3aa_c10bde7a_9b45_4c66_a145_2c05ca9899bf
