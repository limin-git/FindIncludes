#include "StdAfx.h"
#include "Utility.h"


std::string Utility::get_string_from_file( const path& p )
{
    std::ifstream ifs( p.string().c_str() );

    if ( ! ifs )
    {
        std::cout << "\t" << "cannot open file: " << p.string() << std::endl;
        return "";
    }

    return std::string( std::istreambuf_iterator< char >( ifs ), ( std::istreambuf_iterator< char >() ) );
}


std::ostream& Utility::output_paths( std::ostream& os, const std::set<path>& paths )
{
    for ( std::set<path>::const_iterator it = paths.begin(); it != paths.end(); ++it )
    {
        os << it->string() << std::endl;
    }

    return os;
}
