#include "StdAfx.h"
#include "FileIncludeFinder.h"
#include "Vcproj.h"
#include "Solution.h"


void main(int argc, char* argv[])
{
    //FileIncludeFinder f;
    std::set<path> all_includes;
    Solution s( "C:\\Code\\PchBuild\\code\\tools\\build\\tools.build.Build_Base_Core.sln" );
    std::vector<path>& projects = s.m_projects;

    for ( size_t i = 0; i < projects.size(); ++i )
    {
        Vcproj p( projects[i] );
        std::vector<path>& files = p.m_files;

        for ( size_t i = 0; i < files.size(); ++i )
        {
            FileIncludeFinder f( files[i], p.m_current_path, p.m_additional_include_directories );
            all_includes.insert( f.m_includes.begin(), f.m_includes.end() );
        }
    }

    FileIncludeFinder::optput( std::cout, all_includes );
}
