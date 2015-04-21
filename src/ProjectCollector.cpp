#include "StdAfx.h"
#include "ProjectCollector.h"
#include "FileCollector.h"
#include "Utility.h"


ProjectCollector::ProjectCollector( const path& p, const std::string& configuration_name )
    : m_path( p ),
      m_configuration_name( configuration_name )
{
    std::cout << m_path.string() << std::endl;
    m_current_path = m_path.parent_path();
    m_str = Utility::get_string_from_file( m_path.string() );
    //std::cout << m_current_path.string() << std::endl;

    extract_files();
    extract_additional_include_directories();
}


void ProjectCollector::extract_files()
{
    if ( m_str.empty() )
    {
        return;
    }

    // extract files
    static const boost::regex file_regex
    (
        "(?x)"
        "<File \\s+"
        "    RelativePath=\" ( [^\"]+ ) \" \\s+ > "   //$1, file relative path
        "    ( .*? ) \\s+ "                           //$2, configurations
        "</File>"
    );

    static const boost::regex file_configuration_regex
    (
        "(?x)"
        "<FileConfiguration \\s+"
        "    Name= \" ( .+? )  \" \\s+"                 //$1, Name
        "    ExcludedFromBuild= \" ( .+? ) \" \\s+ "    //$2, ExcludedFromBuild
        ">"
    );

    boost::sregex_iterator it( m_str.begin(), m_str.end(), file_regex );
    boost::sregex_iterator end;

    for ( ; it != end; ++it )
    {
        path source_file_relative_path = it->str(1);
        std::string configurations = it->str(2);

        bool is_excluded = false;

        if ( false == configurations.empty() )
        {
            boost::sregex_iterator fc_it( configurations.begin(), configurations.end(), file_configuration_regex );
            boost::sregex_iterator end;

            for ( ; fc_it != end; ++fc_it )
            {
                std::string file_configuration_name = fc_it->str(1);
                std::string excluded_form_build = fc_it->str(2);

                std::stringstream match_strm;
                match_strm << m_configuration_name << "|Win32";

                if ( file_configuration_name == match_strm.str() )
                {
                    if ( "true" == excluded_form_build )
                    {
                        is_excluded = true;
                    }

                    break;
                }
            }
        }

        if ( false == is_excluded )
        {
            path p = boost::filesystem::system_complete( m_current_path / source_file_relative_path  );

            if ( boost::ends_with( p.string(), "cpp" ) )
            {
                //std::cout << p.string() << std::endl;
                m_files.push_back( p );
            }
        }
    }
}


void ProjectCollector::extract_additional_include_directories()
{
    if ( m_str.empty() )
    {
        return;
    }

    std::stringstream configuration_regex_strm;
    configuration_regex_strm
        << "(?x)"
        << "<Configuration \\s+"
        << "    Name=\"" << m_configuration_name << "\\|Win32\""
        << "    .+?"
        << "    AdditionalIncludeDirectories=\" ([^\"]*) \""
        << "    .+?"
        << "</Configuration>"
        ;
    static const boost::regex e( configuration_regex_strm.str() );
    boost::smatch m;

    if ( boost::regex_search( m_str, m, e ) )
    {
        std::string dirs = m.str(1);
        //std::cout << dirs << std::endl;

        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep( ";," );
        tokenizer tokens( dirs, sep );

        for ( tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it )
        {
            path p(*it);
            //std::cout << p.string() << std::endl;
            p.make_preferred();
            m_additional_include_directories.push_back( p );
        }
    }
}


void ProjectCollector::collect_in_thread( std::set<path>& project_includes, const path& p )
{
    ProjectCollector project( p );
    std::vector<path>& files = project.m_files;
    std::vector< std::set<path> > file_includes( files.size() );
    std::vector<boost::shared_ptr<boost::thread> > threads;

    for ( size_t i = 0; i < files.size(); ++i )
    {
        boost::shared_ptr<boost::thread> t = FileCollector::create_FileCollectorThread( file_includes[i], files[i], project.m_current_path, project.m_additional_include_directories );
        threads.push_back( t );
    }

    for ( size_t i = 0; i < threads.size(); ++i )
    {
        threads[i]->join();
    }

    for ( size_t i = 0; i < file_includes.size(); ++i )
    {
        project_includes.insert( file_includes[i].begin(), file_includes[i].end() );
    }
}
