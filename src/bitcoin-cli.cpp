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

#include "linenoise.c"//notum þetta fyrir command history og tab completion

#define arrayLength(a) (sizeof(a)/sizeof(a)[0])

static const char *commands[] =
{ 
    /* Overall control/query calls */
     "getinfo",
     "help",
     "stop",

    /* P2P networking */
     "getnetworkinfo",
     "addnode",
     "getaddednodeinfo",
     "getconnectioncount",
     "getnettotals",
     "getpeerinfo",
     "ping",

    /* Block chain and UTXO */
     "getaddressinfo",
     "getrichaddresses",
     "getblockchaininfo",
     "getbestblockhash",
     "getblockcount",
     "getblock",
     "getblockhash",
     "getdifficulty",
     "getrawmempool",
     "gettxout",
     "gettxoutsetinfo",
     "verifychain",

    /* Mining */
     "getblocktemplate",
     "getmininginfo",
//    { "getnetworkhashps",       &getnetworkhashps,       true,      false,      false },
     "submitblock",

    /* Raw transactions */
     "createrawtransaction",
     "decoderawtransaction",
     "decodescript",
     "getrawtransaction",
     "sendrawtransaction",
     "signrawtransaction",

    /* Utility functions */
     "createmultisig",
     "validateaddress",
     "verifymessage",

#ifdef ENABLE_WALLET
    /* Wallet */
     "addmultisigaddress",
     "backupwallet",
     "dumpprivkey",
     "dumpwallet",
     "encryptwallet",
     "getaccountaddress",
     "getaccount",
     "getaddressesbyaccount",
     "getbalance",
     "getnewaddress",
     "getrawchangeaddress",
     "getreceivedbyaccount",
     "getreceivedbyaddress",
     "gettransaction",
     "getunconfirmedbalance",
     "getwalletinfo",
     "importprivkey",
     "importwallet",
     "keypoolrefill",
     "listaccounts",
     "listaddressgroupings",
     "listlockunspent",
     "listreceivedbyaccount",
     "listreceivedbyaddress",
     "listsinceblock",
     "listtransactions",
     "listunspent",
     "lockunspent",
     "move",
     "sendfrom",
     "sendmany"
     "sendtoaddress",
     "setaccount",
     "settxfee",
     "signmessage",
     "walletlock",
     "walletpassphrasechange",
     "walletpassphrase",

    /* Wallet-enabled mining */
     "getgenerate",
     "gethashespersec",
     "getwork",
     "setgenerate",
#endif // ENABLE_WALLET
};

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

void completion(const char *input, linenoiseCompletions *completions) {

	for(int i = 0; i < arrayLength(commands); i++) {
		const char *command = commands[i];
		int inputIndex = 0;
		bool substringMatch = true;

		for(; input[inputIndex] != 0; inputIndex++) {
			if(command[inputIndex] == 0) break;
			if(input[inputIndex] != command[inputIndex]) {
				substringMatch = false;
				break;
			}
		}

		if(substringMatch) linenoiseAddCompletion(completions, command);
	}
}

int main(int argc, char* argv[])
{
	int returnValue = 0;
	char *line;
	char *historyFilename = ".cli_history";

    SetupEnvironment();

	//NOTE: * 	interactive send
	//		*	búa til contact-a
	//		*	robustness overhull
	

	if(argc > 1 && strcmp(argv[1], "interactive") == 0) {
		printf("Interactive Mode(type quit to exit)\n\n");

		linenoiseHistoryLoad(historyFilename);
    	linenoiseSetCompletionCallback(completion);

		while((line = linenoise("> "))) {
			char *arguments[32];
			int argumentCount = 0;

			arguments[argumentCount++] = "";

			if(line[0] != 0) {
				char *token = strtok(line, " ");

				if(strcmp(token, "quit") != 0) {
					if(strcmp(token, "clear") == 0) {
						system("clear");
						continue;
					}

					do {
						arguments[argumentCount++] = token;
					}while((token = strtok(NULL, " ")));

				}else {
					break;
				}
			}

			returnValue = evaluateCommands(argumentCount, arguments);

			linenoiseHistoryAdd(line);
			linenoiseHistorySave(historyFilename);

			free(line);
		}
	}else {
		returnValue = evaluateCommands(argc, argv);
	}

	return returnValue;
}
