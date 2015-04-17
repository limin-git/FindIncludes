#include "StdAfx.h"
#include "Solution.h"


Solution::Solution( const path& p )
    : m_path( p )
{
    std::cout << m_path.string() << std::endl;
    m_current_path = m_path.parent_path();
    m_str = get_string_from_file( m_path.string() );
    //std::cout << m_current_path.string() << std::endl;

    extract_projects();
}


std::string Solution::get_string_from_file( const std::string& file_path )
{
    std::ifstream ifs( file_path.c_str() );

    if ( ifs )
    {
        return std::string( std::istreambuf_iterator< char >( ifs ), ( std::istreambuf_iterator< char >() ) );
    }

    return "";
}


void Solution::extract_projects()
{
    if ( m_str.empty() )
    {
        return;
    }

    // extract additional include directories
    static const boost::regex project_regex
    (
        "(?x)"
        "(?<=\\n) ^ [ \\t]*"                            // starts with spaces
        "\\< Project \\> \\s*"
        "( \\(\"\\{ .*? \\}\"\\) \\s* = \\s* )"         // $1,                                          ("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") =
        "( \" [^\"]+ \", \\s* )"                        // $2, project name :                           "core.alarm.idl",
        "( \" ( [^\"]+ ) \", \\s* )"                    // $3,4, project relative file path             "..\..\transactive\core\alarm\IDL\core.alarm.idl.TA_AlarmAgentCorbaDef.vcproj",
        "( \" [^\"]+ \" \\s* )"                         // $5, other things                             "{7C6902D2-CFA0-4E8B-B4A9-C68815297F8C}"
        " .*? "
        "(?<=\\n) ^ [ \\t]*"                            // starts with spaces
        "\\< EndProject \\>"
    );

    boost::sregex_iterator it( m_str.begin(), m_str.end(), project_regex );
    boost::sregex_iterator end;

    for ( ; it != end; ++it )
    {
        path project_relative_path = it->str(4);
        path p = boost::filesystem::system_complete( m_current_path / project_relative_path );
        std::cout << p.string() << std::endl;
        m_projects.push_back( p );
    }
}
