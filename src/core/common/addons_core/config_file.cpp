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
#include <boost/filesystem.hpp>
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


Common::Configuration Common::ParseConfiguration(const std::string& configPath)
{
  ptree pt;
  read_xml(configPath, pt);
  Configuration configuration;
  BOOST_FOREACH(const ptree::value_type& config, pt)
  {
    if (config.first != "config")
    {
      std::cerr << "Unknown root tag " << config.first << " in the config file '" << configPath << "'" << std::endl;
      continue;
    }

    BOOST_FOREACH(const ptree::value_type& param, config.second)
    {
      if (param.first != "modules")
      {
        AddParameter(configuration.Parameters, param.first, param.second);
        continue;
      }

      BOOST_FOREACH(const ptree::value_type& module, param.second)
      {
        if (module.first != "module")
        {
          std::cerr << "Unknown tag " << module.first << " inside 'modules' in the config file '" << configPath << "'" << std::endl;
          continue;
        }

        Common::ModuleConfiguration moduleConfig;
        moduleConfig.Id = module.second.get<std::string>("id");
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

        configuration.Modules.push_back(moduleConfig);
      }
    }
  }
  return configuration;
}

Common::Configuration Common::ParseConfigurationFiles(const std::string& directory)
{
  using namespace boost::filesystem;
  Common::Configuration configuration;
  path p(directory);
  directory_iterator dirIt(p);
  directory_iterator end;

  std::for_each(dirIt, end, [&configuration](const directory_entry& entry)
  {
    if  (entry.path().filename().extension() == ".conf")
    {
      std::cout << "Parsing config file: " << entry.path().native() << std::endl;
      Common::Configuration tmp = Common::ParseConfiguration(entry.path().string());
      configuration.Modules.insert(configuration.Modules.end(), tmp.Modules.begin(), tmp.Modules.end());
      configuration.Parameters.Groups.insert(configuration.Parameters.Groups.end(), tmp.Parameters.Groups.begin(), tmp.Parameters.Groups.end());
      configuration.Parameters.Parameters.insert(configuration.Parameters.Parameters.end(), tmp.Parameters.Parameters.begin(), tmp.Parameters.Parameters.end());
    }
  });
  return configuration;
}


namespace
{
  void AddDependencies(ptree& moduleTree, const std::vector<Common::AddonId>& ids)
  {
    if (ids.empty())
    {
      return;
    }

    ptree& deps = moduleTree.add("depends_on", "");
    for (auto idIt = ids.begin(); idIt != ids.end(); ++idIt)
    {
      deps.add("id", *idIt);
    }
  }

  void AddGroup(ptree& moduleTree, const Common::ParametersGroup& group)
  {
    ptree& groupTree = moduleTree.add(group.Name, "");
    for (auto paramIt = group.Parameters.begin(); paramIt != group.Parameters.end(); ++paramIt)
    {
      groupTree.add(paramIt->Name, paramIt->Value);
    }
    for (auto groupIt = group.Groups.begin(); groupIt != group.Groups.end(); ++groupIt)
    {
      AddGroup(groupTree, *groupIt);
    }
  }

  void AddParameters(ptree& moduleTree, const Common::AddonParameters& params, const char* groupName)
  {
    if (params.Parameters.empty() && params.Groups.empty())
    {
      return;
    }

    ptree& paramsTree = moduleTree.add(groupName, "");
    for (auto paramIt = params.Parameters.begin(); paramIt != params.Parameters.end(); ++paramIt)
    {
      paramsTree.add(paramIt->Name, paramIt->Value);
    }
    for (auto groupIt = params.Groups.begin(); groupIt != params.Groups.end(); ++groupIt)
    {
      AddGroup(paramsTree, *groupIt);
    }
  }
}

void Common::SaveConfiguration(const Common::ModulesConfiguration& modules, const std::string& configPath)
{
  ptree pt;
  ptree& modulesPt = pt.put("config.modules", "");

  for (auto configIt = modules.begin(); configIt != modules.end(); ++configIt)
  {
    ptree& moduleTree = modulesPt.add("module", "");
    const Common::ModuleConfiguration& config = *configIt;
    moduleTree.add("id", config.Id);
    moduleTree.add("path", config.Path);
    AddDependencies(moduleTree, config.Dependencies);
    AddParameters(moduleTree, config.Parameters, "parameters");
  }

  write_xml(configPath, pt);
}

Common::AddonInformation Common::GetAddonInfomation(const Common::ModuleConfiguration& config)
{
  Common::AddonInformation info;
  info.Id = config.Id;
  info.Dependencies = config.Dependencies;
  info.Parameters = config.Parameters;
  info.Factory = Common::CreateDynamicAddonFactory(config.Path);
  return info;
}
