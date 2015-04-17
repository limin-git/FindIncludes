#include "StdAfx.h"
#include "FileIncludeFinder.h"
#include "Vcproj.h"
#include "Solution.h"


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
        includes = Vcproj::get_includes_in_thread( argv[1] );
    }
    else if ( boost::ends_with( argv[1], "sln" ) )
    {
        includes = Solution::get_includes_in_thread( argv[1] );
    }
    else
    {
        std::cout << "can not recognize \'" << argv[1] << "\', only support .vcproj, .sln." << std::endl;
        return;
    }

    //Vcproj p( "C:\\Code\\PchBuild\\code\\transactive\\core\\alarm\\core.alarm.TA_Alarm.vcproj" );
    //std::set<path> paths = Vcproj::get_includes_in_thread( "C:\\Code\\PchBuild\\code\\transactive\\core\\alarm\\core.alarm.TA_Alarm.vcproj" );
    //std::set<path> paths = Solution::get_includes_in_thread( "C:\\Code\\PchBuild\\code\\tools\\build\\tools.build.Build_Base_Bus.sln" );

    system( "CLS" );
    std::cout << "================================================= INCLUDES =================================================" << std::endl;
    FileIncludeFinder::output( std::cout, includes );
}
