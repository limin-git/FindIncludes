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


const std::string& FileIncludeFinder::get_string_from_file( const std::string& file_path )
{
    static std::map<std::string, std::string> m_cache;
    static boost::mutex m_lock;
    boost::lock_guard<boost::mutex> guard(m_lock);

    std::map<std::string, std::string>::iterator it = m_cache.find( file_path );

    if ( it == m_cache.end() )
    {
        std::ifstream ifs( file_path.c_str() );

        if ( ifs )
        {
            m_cache[file_path] = std::string( std::istreambuf_iterator< char >( ifs ), ( std::istreambuf_iterator< char >() ) );
        }
    }

    return m_cache[file_path];
}


const std::vector<path>& FileIncludeFinder::get_includes( const std::string& s )
{
    static std::map<const char*, std::vector<path> > m_cache;
    static boost::mutex m_lock;
    boost::lock_guard<boost::mutex> guard(m_lock);

    std::map<const char*, std::vector<path> >::iterator it = m_cache.find( s.c_str() );

    if ( it == m_cache.end() )
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

        m_cache[s.c_str()] = paths;
    }

    return m_cache[s.c_str()];
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
        const std::string& s = get_string_from_file( p.string() );
        const std::vector<path>& includes = get_includes( s );

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


std::ostream& FileIncludeFinder::output( std::ostream& os, const std::set<path>& paths )
{
    for ( std::set<path>::const_iterator it = paths.begin(); it != paths.end(); ++it )
    {
        os << it->string() << std::endl;
    }

    return os;
}


boost::shared_ptr<boost::thread>  FileIncludeFinder::get_includes_thread( std::set<path>& includes, const path& p, const path& current_path, const std::vector<path>& additional )
{
    struct ThreadFinder
    {
        ThreadFinder( std::set<path>& includes, const path& p, const path& current_path, const std::vector<path>& additional )
            : m_includes( includes ),
              m_path( p ),
              m_current_path( current_path ),
              m_additional( additional )
        {
        }

        void operator()()
        {
            FileIncludeFinder f( m_path, m_current_path, m_additional );
            m_includes = f.m_includes;
        }

        std::set<path>& m_includes;
        const path& m_path;
        const path& m_current_path;
        const std::vector<path>& m_additional;
    };

    ThreadFinder thread_finder( includes, p, current_path, additional );
    return boost::shared_ptr<boost::thread>( new boost::thread( thread_finder ) );
}
