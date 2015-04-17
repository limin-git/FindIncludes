#include "StdAfx.h"
#include "FileIncludeFinder.h"
#include "Vcproj.h"


void main(int argc, char* argv[])
{
    //FileIncludeFinder f;

    Vcproj p( "C:\\Code\\PchBuild\\code\\transactive\\core\\corba\\core.corba.TA_CorbaUtil.vcproj" );

    std::vector<path>& files = p.m_files;

    std::set<path> all_includes;

    for ( size_t i = 0; i < files.size(); ++i )
    {
        FileIncludeFinder f( files[i], p.m_current_path, p.m_additional_include_directories );
        all_includes.insert( f.m_includes.begin(), f.m_includes.end() );
    }

    FileIncludeFinder::optput( std::cout, all_includes );
}
