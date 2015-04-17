#pragma once
#include <boost/filesystem.hpp>


using namespace boost::filesystem;


struct FileIncludeFinder
{
public:

    FileIncludeFinder();

    void find_includes();

private:

    static std::string get_string_from_file( const std::string& file_path );
    static std::vector<path> get_includes( const std::string& s );
    path find_existing_path( const path& include );
    void find_all_includes();

private:

    path m_path;
    path m_current_path;
    std::vector<path> m_additional_directories;
    std::queue<path> m_queue;
    std::set<path> m_includes;
};
