#include "StdAfx.h"
#include "FileIncludeFinder.h"


FileIncludeFinder::FileIncludeFinder()
{
    //../../../../build/win32_n/transactive;../../../../build/win32_n/transactive/core/message/idl/src;../../../../build/win32_n/transactive/core/exceptions/idl/src;..\..\;..\..\..\cots\omniORB\omniORB_4.1.6\include;../../../cots/ACE/6_0_4/ACE_wrappers;..\..\..\cots\boost\boost_1_39_0
    m_path = "C:\\Code\\PchBuild\\code\\transactive\\core\\corba\\src\\CorbaUtil.cpp";
    m_current_path = "C:\\Code\\PchBuild\\code\\transactive\\core\\corba";
    //m_current_path = boost::filesystem::system_complete( m_current_path );
    //std::cout << m_current_path.string() << std::endl;
    m_additional_directories.push_back( path("../../") );
    m_additional_directories.push_back( path("../../../../build/win32_n/transactive") );
    m_additional_directories.push_back( path("../../../../build/win32_n/transactive/core/message/idl/src") );
    m_additional_directories.push_back( path("../../../../build/win32_n/transactive/core/exceptions/idl/src") );
    m_additional_directories.push_back( path("../../../cots/omniORB/omniORB_4.1.6/include") );
    m_additional_directories.push_back( path("../../../cots/ACE/6_0_4/ACE_wrappers") );
    m_additional_directories.push_back( path("../../../cots/boost\boost_1_39_0") );

    find_all_includes();
}


std::string FileIncludeFinder::get_string_from_file( const std::string& file_path )
{
    std::ifstream ifs( file_path.c_str() );

    if ( ifs )
    {
        return std::string( std::istreambuf_iterator< char >( ifs ), ( std::istreambuf_iterator< char >() ) );
    }

    return "";
}


std::vector<path> FileIncludeFinder::get_includes( const std::string& s )
{
    std::vector<path> paths;

    const char* include_str =
        "(?x)"
        "^ [ \\t]* \\#include [ \\t]+ [\"<] (.+?) (?!\\.inl) [\">]"
        ;

    static const boost::regex e( include_str );
    boost::sregex_iterator it( s.begin(), s.end(), e );
    boost::sregex_iterator end;

    for ( ; it != end; ++it )
    {
        paths.push_back( path(it->str(1)) );
        //std::cout << it->str(1) << std::endl;
    }

    return paths;
}


path FileIncludeFinder::find_existing_path( const path& include )
{
    if ( include.is_complete() )
    {
        return include;
    }

    path p = m_current_path / include;

    if ( boost::filesystem::exists( p ) )
    {
        //std::cout << boost::filesystem::system_complete(p).string() << std::endl;
        return boost::filesystem::system_complete(p);
    }

    for ( size_t i = 0; i < m_additional_directories.size(); ++i )
    {
        path p = m_current_path / m_additional_directories[i] / include;

        if ( boost::filesystem::exists( p ) )
        {
            //std::cout << boost::filesystem::system_complete(p).string() << std::endl;
            return boost::filesystem::system_complete(p);
        }
    }

    return path();
}


void FileIncludeFinder::find_all_includes()
{
    m_queue.push( m_path );

    while ( ! m_queue.empty() )
    {
        path p = m_queue.front();
        m_queue.pop();
        m_includes.insert( p );

        p = find_existing_path( p );

        if ( p.empty() )
        {
            continue;
        }

        std::string s = get_string_from_file( p.string() );
        std::vector<path> includes = get_includes( s );

        for ( size_t i = 0; i < includes.size(); ++i )
        {
            path& include = includes[i];

            path p = find_existing_path( include );

            if ( ! p.empty() )
            {
                std::cout << p.string() << std::endl;
                if ( m_includes.find( include ) == m_includes.end() )
                {
                    m_queue.push( include );
                }
            }
        }
    }

    return;

    std::string s = get_string_from_file( m_path.string() );
    //std::cout << s << std::endl;

    std::vector<path> includes = get_includes( s );

    for ( size_t i = 0; i < includes.size(); ++i )
    {
        find_existing_path( includes[i] );
    }

}
