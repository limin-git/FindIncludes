#include "StdAfx.h"
#include "FileIncludeFinder.h"


FileIncludeFinder::FileIncludeFinder( const path& p, const path& current_path, const std::vector<path>& additional )
    : m_path( p ),
      m_current_path( current_path ),
      m_additional_directories( additional )
{
    std::cout << m_path.string() << std::endl;
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
    static const boost::regex e
    (
        "(?x)"
        "^ [ \\t]* \\#include [ \\t]+ [\"<] (.+?) (?!\\.inl) [\">]"
    );
    boost::sregex_iterator it( s.begin(), s.end(), e );
    boost::sregex_iterator end;

    for ( ; it != end; ++it )
    {
        paths.push_back( path(it->str(1)) );
        //std::cout << it->str(1) << std::endl;
    }

    return paths;
}


path FileIncludeFinder::find_existing_path( const path& include, const path& parent )
{
    if ( include.is_complete() )
    {
        return include;
    }

    {
        path p = parent / include;

        if ( boost::filesystem::exists( p ) && ! is_directory(p) )
        {
            //std::cout << boost::filesystem::system_complete(p).string() << std::endl;
            return boost::filesystem::system_complete(p);
        }
    }

    {
        path p = m_current_path / include;

        if ( boost::filesystem::exists( p ) && ! is_directory(p) )
        {
            //std::cout << boost::filesystem::system_complete(p).string() << std::endl;
            return boost::filesystem::system_complete(p);
        }
    }

    for ( size_t i = 0; i < m_additional_directories.size(); ++i )
    {
        path p;
        path add = m_additional_directories[i];

        if ( add.is_absolute() )
        {
            p = add / include;
        }
        else
        {
            p = m_current_path / add / include;
        }

        if ( boost::filesystem::exists( p ) && ! is_directory(p) )
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

        if ( p.empty() )
        {
            continue;
        }

        //std::cout << p.string() << std::endl;
        path parent = p.parent_path();
        std::string s = get_string_from_file( p.string() );
        std::vector<path> includes = get_includes( s );

        for ( size_t i = 0; i < includes.size(); ++i )
        {
            path p = find_existing_path( includes[i], parent );

            if ( ! p.empty() )
            {
                if ( m_includes.find( p ) == m_includes.end() )
                {
                    m_includes.insert( p );
                    m_queue.push( p );
                }
            }
            else
            {
                if ( true == m_includes.insert( includes[i] ).second )
                {
                    //std::cout << includes[i].string() << std::endl;
                }
            }
        }
    }
}


std::ostream& FileIncludeFinder::optput( std::ostream& os, const std::set<path>& paths )
{
    for ( std::set<path>::const_iterator it = paths.begin(); it != paths.end(); ++it )
    {
        os << it->string() << std::endl;
    }

    return os;
}

