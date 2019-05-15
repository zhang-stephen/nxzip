/**
 * The Main Cpp Source File in NXZIP
 * 2019-02-02
 */

#include <iostream>
#include <filesystem>
#include <cstdlib>

#include "tclap/CmdLine.h"
#include "nxzip.h"
#include "nxzutil.h"

namespace fs = std::filesystem;
namespace nxzutil = NXZIP::utility;

/**
 * @brief	Main Function 
 */
int main(int argc, char* argv[])
{
	try
	{
		/* Create a CommandLine Obj */
		TCLAP::CmdLine nxzCmd("NXZip Command Arguments & Options", ' ', NXZ_Version);

		/* Add Options and Flags */
		// unlabeled arg, Spcify your input file path(required)
		TCLAP::UnlabeledValueArg<std::string> iFileName("input", "Specify Input File Name", true, "string", "inputFileName");
		nxzCmd.add(iFileName);

		// -s/--size: the size of Dictionary in BWT Processing
		std::vector<int> allowed {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u,};
		TCLAP::ValuesConstraint<int> aSize(allowed);
		TCLAP::ValueArg<int> dictSize("s", "size", "BWT Dictionary size", false, 1u, &aSize);
		nxzCmd.add(dictSize);

		// -v/--visual: Output the Operation Log
		TCLAP::SwitchArg visualFLag("v", "verbose", "Show the Operation Log on Terminal", false);
		nxzCmd.add(visualFLag);

		// -r/--remove: delete source file or not after operation completed
		TCLAP::SwitchArg rmFlag("r", "remove", "Delete source file after operation completed", false);
		nxzCmd.add(rmFlag);

		// -m/--comment: Add comment to zipx file
		TCLAP::ValueArg<std::string> commnetStr("m", "comment", "Add comment to the Compressed file", false, "default", "string");
		nxzCmd.add(commnetStr);

		// -u/--unzip: Compress or Decompress
		TCLAP::SwitchArg unzipFlag("u", "unzip", "Decompress if this flag existed and ignore other flags", false);
		nxzCmd.add(unzipFlag);

		// -R/--read: read compressed file infomation
		TCLAP::SwitchArg readinfo("R", "read", "Show Compressed File Information and ignore other flags", false);
		nxzCmd.add(readinfo);

		/* Command Line Praser */
		nxzCmd.parse(argc, argv);
		
		std::string ifile = iFileName.getValue();

		nxzutil::CLIOPS cliops = {false, false};
		std::string comment;

		/* NXZIP Operations */
		// Data Compressing
		if(unzipFlag.getValue() == false)
		{
			NXZIP::NXZ_Compress(ifile, cliops, comment, (uint8_t)dictSize.getValue());
			return EXIT_SUCCESS;
		}

		// Data Decompressing
		if(unzipFlag.getValue() == true)
		{
			NXZIP::NXZ_Decompress(ifile);
			return EXIT_SUCCESS;
		}

		// Show info
		if(readinfo.getValue() == true)
		{
			return EXIT_SUCCESS;
		}
	}
	catch(TCLAP::ArgException &e)
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	return 0;
}

//EOF
