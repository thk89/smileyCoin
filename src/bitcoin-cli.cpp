// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "util.h"
#include "init.h"
#include "rpcclient.h"
#include "rpcprotocol.h"
#include "ui_interface.h" /* for _(...) */
#include "chainparams.h"

#include <boost/filesystem/operations.hpp>

static bool AppInitRPC(int argc, char* argv[])
{
    // Parameters
    ParseParameters(argc, argv);
    if (!boost::filesystem::is_directory(GetDataDir(false)))
    {
        fprintf(stderr, "Error: Specified data directory \"%s\" does not exist.\n", mapArgs["-datadir"].c_str());
        return false;
    }
    try {
        ReadConfigFile(mapArgs, mapMultiArgs);
    } catch(std::exception &e) {
        fprintf(stderr,"Error reading configuration file: %s\n", e.what());
        return false;
    }

    if (argc<2 || mapArgs.count("-?") || mapArgs.count("--help"))
    {
        // First part of help message is specific to RPC client
        std::string strUsage = _("Smileycoin Core RPC client version") + " " + FormatFullVersion() + "\n\n" +
            _("Usage:") + "\n" +
              "  smileycoin-cli [options] <command> [params]  " + _("Send command to Smileycoin Core") + "\n" +
              "  smileycoin-cli [options] help                " + _("List commands") + "\n" +
              "  smileycoin-cli [options] help <command>      " + _("Get help for a command") + "\n";

        strUsage += "\n" + HelpMessageCli(true);

        fprintf(stdout, "%s", strUsage.c_str());
        return false;
    }
    return true;
}

int evaluateCommands(int argc, char *argv[]) {
	int result = 0;

    try
    {
        if(!AppInitRPC(argc, argv))
            return abs(RPC_MISC_ERROR);
    }
    catch (std::exception& e) {
        PrintExceptionContinue(&e, "AppInitRPC()");
        return abs(RPC_MISC_ERROR);
    } catch (...) {
        PrintExceptionContinue(NULL, "AppInitRPC()");
        return abs(RPC_MISC_ERROR);
    }

    result = abs(RPC_MISC_ERROR);
    try
    {
        result = CommandLineRPC(argc, argv);
    }
    catch (std::exception& e) {
        PrintExceptionContinue(&e, "CommandLineRPC()");
    } catch (...) {
        PrintExceptionContinue(NULL, "CommandLineRPC()");
    }

	return result;
}

int main(int argc, char* argv[])
{
	int return_value = 0;
	bool interactive_repeat = false;
    SetupEnvironment();

	//NOTE: * interactive json parameter, kannski táknað með -i
	//		* robustness overhull
	
	if(argc > 1) {
		if(strcmp(argv[1], "interactive") == 0) {
			interactive_repeat = true;
		}
	}

	if(!interactive_repeat) {
		return_value = evaluateCommands(argc, argv);
	}else {
		printf("Interactive Mode(type quit to exit)\n\n");

		while(interactive_repeat) {
			printf("> ");
			std::vector<std::string> arguments;
			std::string line;
			std::getline(std::cin, line);

			std::istringstream lineStream(line);
			std::string token;

			//NOTE: gerum þetta þar sem fyrsta argumentið í argv er pwd
			arguments.push_back("");

			std::getline(lineStream, token, ' ');

			//TODO: betri meðhöndlun á interactive specific argument-um
			if(token != "quit") {
				if(token != "") {
					do {
						//athuga hvort argument er -i og þá fara í eitthvað specific json string interactive mode
						arguments.push_back(token);
					}while(std::getline(lineStream, token, ' '));

					unsigned int argument_count = 0;
					char *c_arguments[arguments.size()];

					for(; argument_count < arguments.size(); argument_count++) {
						c_arguments[argument_count] = (char *)arguments[argument_count].c_str();
					}

					return_value = evaluateCommands(argument_count, c_arguments);
				}
			}else {
				interactive_repeat = false;
			}
		}
	}


	return return_value;
}
