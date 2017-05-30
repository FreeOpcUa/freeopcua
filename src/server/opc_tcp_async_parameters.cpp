/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/


#include "opc_tcp_async_parameters.h"

namespace OpcUa
{
namespace Server
{

AsyncOpcTcp::Parameters GetOpcTcpParameters(const Common::AddonParameters & addonParams)
{
  AsyncOpcTcp::Parameters result;

  for (const Common::Parameter & param : addonParams.Parameters)
    {
      if (param.Name == "debug")
        { result.DebugMode = param.Value == "false" || param.Value == "0" ? false : true; }
    }

  return result;
}

} // namespace UaServer
} // namespace OpcUa
