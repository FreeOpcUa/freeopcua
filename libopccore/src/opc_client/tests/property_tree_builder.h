/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Preparing of tests
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __property_tree_builder_h__18a29acf_0eba_4ac3_aaa4_e2687f5b5664
#define __property_tree_builder_h__18a29acf_0eba_4ac3_aaa4_e2687f5b5664

/// Registers property tree manager
/// starts addons core
/// Builds property tree for tests

void PrepareTests(bool needRegisterCommonAddons = true);
void StopTests();

#endif // __property_tree_builder_h__18a29acf_0eba_4ac3_aaa4_e2687f5b5664

