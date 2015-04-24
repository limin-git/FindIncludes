#include "StdAfx.h"
#include "FileCollector.h"
#include "ProjectCollector.h"
#include "SolutionCollector.h"
#include "Utility.h"


void main(int argc, char* argv[])
{
    if ( argc != 2 )
    {
        path p( argv[0] );

        std::cout
            << "Usage: \n"
            << "    " << p.filename().string() << " < .vcproj | .sln >"
            << std::endl;
        return;
    }

    std::set<path> includes;

    if ( boost::ends_with( argv[1], "vcproj" ) )
    {
        ProjectCollector::collect_in_thread( includes, argv[1] );
    }
    else if ( boost::ends_with( argv[1], "sln" ) )
    {
        SolutionCollector::collect_in_thread( includes, argv[1] );
    }
    else
    {
        std::cout << "can not recognize \'" << argv[1] << "\', only support .vcproj, .sln." << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "====================================================== includes ======================================================" << std::endl;
    Utility::output_paths( std::cout, includes );
}
