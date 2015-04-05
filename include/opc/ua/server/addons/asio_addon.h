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

#pragma once


#include <opc/common/addons_core/addon.h>


namespace boost
{
  namespace asio
  {

    class io_service;

  }
}

namespace OpcUa
{
  namespace Server
  {

    const char AsioAddonId[] = "async";


    class AsioAddon : public Common::Addon
    {
    public:
      DEFINE_CLASS_POINTERS(AsioAddon);

    public:
      virtual boost::asio::io_service& GetIoService() = 0;
    };


    class AsioAddonFactory : public Common::AddonFactory
    {
    public:
      Common::Addon::UniquePtr CreateAddon() override;
    };

  }
}
