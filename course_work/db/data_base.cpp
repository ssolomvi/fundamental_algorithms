#include "data_base.h"

void
data_base::add_to_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct key, data_base::value_struct value)
{

}

const data_base::value_struct &
data_base::find_among_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct key)
{

}

data_base::value_struct
data_base::delete_from_collection
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::key_struct key)
{
    return data_base::value_struct();
}

void
data_base::add_to_structure
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::trees_types_ tree_type, data_base::allocator_types_ allocator_type)
{

}

void
data_base::delete_from_structure
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name)
{

}

void
data_base::change_the_structure
(const std::string &pull_name, const std::string &scheme_name, const std::string &collection_name,
 data_base::trees_types_ tree_type, data_base::allocator_types_ allocator_type)
{

}

void data_base::save_to_file(const std::string &filename)
{

}

void data_base::upload_from_file(const std::string &filename)
{

}
