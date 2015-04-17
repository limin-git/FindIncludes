#pragma once
#include <boost/filesystem.hpp>


using namespace boost::filesystem;


struct FileIncludeFinder
{
public:

    FileIncludeFinder( const path& p, const path& current_path, const std::vector<path>& additional );
    void find_includes();
    static std::ostream& optput( std::ostream& os, const std::set<path>& paths );

private:

    static std::string get_string_from_file( const std::string& file_path );
    static std::vector<path> get_includes( const std::string& s );
    path find_existing_path( const path& include, const path& parent );
    void find_all_includes();

public:

    path m_path;
    path m_current_path;
    std::vector<path> m_additional_directories;
    std::queue<path> m_queue;
    std::set<path> m_includes;
};
