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

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a)[0])

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
	int result = 0;
	bool interactive_repeat = false;
    SetupEnvironment();

	//NOTE: gera do while hér, sem heldur áfram ef beðið var um repl
	
	if(argc > 1) {
		if(strcmp(argv[1], "interactive") == 0) {
			interactive_repeat = true;
		}
	}

	if(!interactive_repeat) {
		result = evaluateCommands(argc, argv);
	}else {
		printf("Interactive Mode\n\n");

		do {
			printf("> ");
			char line[4096];
			char *arguments[32];

			fgets(line, ARRAY_LENGTH(line), stdin);

			char *token = strtok(line, " ");
			int argument_count = 0;

			if(strcmp(token, "quit\n") != 0) {
				if(strcmp(token, "\n") != 0) {
					do
					{
						arguments[argument_count] = token;
						argument_count++;
					}while((token = strtok(NULL, " ")));
				}

				printf("%d\n", argument_count);

				//NOTE: + 1 þvi bitcoin core býst við að þettu séu 
				//cmdline argument
				result = evaluateCommands(argument_count+1, arguments);
			}else {
				interactive_repeat = false;
			}
		}while(interactive_repeat);
	}


	return result;
}
