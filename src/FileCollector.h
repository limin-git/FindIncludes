#pragma once
#include "CommonDefs.h"


struct FileCollector
{
public:

    FileCollector( const path& p, const path& current_path, const std::vector<path>& additional );

    static boost::shared_ptr<boost::thread> create_FileCollectorThread( std::set<path>& includes, const path& p, const path& current_path, const std::vector<path>& additional );

private:

    path search_path( const path& include, const path& parent );
    void recursive_collect();
    const std::vector<path>& collect_from_file( const path& file_path ); 

public:

    path m_path;
    path m_current_path;
    std::vector<path> m_additional_directories;
    std::queue<path> m_queue;
    std::set<path> m_includes;
};
