#pragma once
#include <boost/filesystem.hpp>


using namespace boost::filesystem;


class Solution
{
public:

    Solution( const path& p );

    static std::set<path> get_includes_in_thread( const path& file_path );

private:

    void extract_projects();

    static std::string get_string_from_file( const std::string& file_path );

public:

    path m_path;
    std::string m_str;
    path m_current_path;
    std::vector<path> m_projects;
};
