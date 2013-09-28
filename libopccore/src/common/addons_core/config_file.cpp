/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#include <opc/common/addons_core/config_file.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

using boost::property_tree::ptree;

namespace
{

  Common::ParametersGroup GetGroup(const std::string& name, const ptree& groupTree)
  {
    Common::ParametersGroup group(name);

    if (groupTree.empty())
    {
      return group;
    }

    BOOST_FOREACH(const ptree::value_type& child, groupTree)
    {
      if (child.second.empty())
      {
        group.Parameters.push_back(Common::Parameter(child.first, child.second.data()));
        continue;
      }
      group.Groups.push_back(GetGroup(child.first, child.second));
    }

    return group;
  }

  void AddParameter(Common::AddonParameters& params, const std::string& name, const ptree& tree)
  {
    if (tree.empty())
    {
      params.Parameters.push_back(Common::Parameter(name, tree.data()));
      return;
    }
    params.Groups.push_back(GetGroup(name, tree));
  }

} // namespace


Common::ModulesConfiguration Common::ParseConfiguration(const std::string& configPath)
{
  ptree pt;
  read_xml(configPath, pt);
  ModulesConfiguration configuration;
  const boost::optional<ptree&> modules = pt.get_child_optional("config.modules");
  if (modules)
  {
    BOOST_FOREACH(const ptree::value_type& module, modules.get())
    {
      if (module.first != "module")
      {
        continue;
      }

      Common::ModuleConfiguration moduleConfig;
      moduleConfig.ID = module.second.get<std::string>("id");
      moduleConfig.Path = module.second.get<std::string>("path");
      if (boost::optional<const ptree&> dependsOn = module.second.get_child_optional("depends_on"))
      {
        BOOST_FOREACH(const ptree::value_type& depend, dependsOn.get())
        {
          if (depend.first != "id")
          {
            continue;
          }
          moduleConfig.Dependencies.push_back(depend.second.data());
        }
      }

      if (boost::optional<const ptree&> parameters = module.second.get_child_optional("parameters"))
      {
        BOOST_FOREACH(const ptree::value_type& parameter, parameters.get())
        {
          AddParameter(moduleConfig.Parameters, parameter.first, parameter.second);
        }
      }

      configuration.push_back(moduleConfig);
    }
  }
  return configuration;
}


void Common::SaveConfiguration(const Common::ModulesConfiguration& configuration, const std::string& configPath)
{
  ptree pt;
  pt.put("config.modules", "");
/*
  for (const auto configIt = configuration.begin(); configIt != configuration.end(); ++configIt)
  {
    const Common::AddonConfiguration& config = *configIt;

    config.
  }
*/
  write_xml(configPath, pt);
}

Common::AddonInformation Common::GetAddonInfomation(const Common::ModuleConfiguration& config)
{
  Common::AddonInformation info;
  info.ID = config.ID;
  info.Dependencies = config.Dependencies;
  info.Parameters = config.Parameters;
  info.Factory = Common::CreateDynamicAddonFactory(config.Path);
  return info;
}
