#include <iostream>
#include <conio.h> // for _getch
#include "Debugger.h"
#include "Disassembler.h"
#include "PatternScanner.h"

enum Game
{
	none,
	ModernWarfare,
	Vanguard,
	ModernWarfare2steam,
	ModernWarfare2bnet,
};

void print_usage()
{
	std::cout << "Usage: DecryptionDumper [game] [exe_path] [sleep_ms]\n"
			  << "  game: 1 (Modern Warfare), 2 (Vanguard), 3 (Modern Warfare 2 (steam)), 4 (Modern Warfare 2 (bnet))\n"
			  << "  exe_path: Full path to the executable (default: cod.exe)\n"
			  << "  sleep_ms: Sleep duration before starting debugger in milliseconds (default: 3)\n";
}

int main(int argc, char *argv[])
{
	Debugger debug = Debugger();
	Game selected_game = Game::none;
	std::string exe_path = "cod.exe"; // Default executable path
	int sleep_ms = 3;				  // Default sleep duration

	if (argc > 1)
	{
		// Non-interactive mode
		if (argc < 2 || argc > 4)
		{
			print_usage();
			return 1;
		}

		// Parse game selection
		selected_game = static_cast<Game>(std::stoi(argv[1]));
		if (selected_game < ModernWarfare || selected_game > ModernWarfare2bnet)
		{
			std::cerr << "Invalid game selection.\n";
			print_usage();
			return 1;
		}

		// Parse executable path (if provided)
		if (argc >= 3)
		{
			exe_path = argv[2];
		}

		// Parse sleep duration (if provided)
		if (argc == 4)
		{
			sleep_ms = std::stoi(argv[3]);
		}
	}
	else
	{
		// Interactive mode
		while (true)
		{
			printf("Select game: \n"
				   "\t1: Modern Warfare\n"
				   "\t2: Vanguard\n"
				   "\t3: Modern Warfare 2 (steam)\n"
				   "\t4: Modern Warfare 2 (bnet)\n");
			selected_game = (Game)((int)_getch() - '0');

			printf("Enter the full path to the executable (default: cod.exe): ");
			std::cin.ignore(); // Clear any leftover input
			std::getline(std::cin, exe_path);
			if (exe_path.empty())
			{
				exe_path = "cod.exe"; // Default executable path
			}

			printf("Enter sleep duration before starting debugger (in ms, default: 3): ");
			std::string sleep_input;
			std::getline(std::cin, sleep_input);
			if (sleep_input.empty())
			{
				sleep_ms = 3; // Default sleep duration
			}
			else
			{
				sleep_ms = std::stoi(sleep_input);
			}

			if (selected_game >= ModernWarfare && selected_game <= ModernWarfare2bnet)
			{
				break;
			}
			else
			{
				system("cls");
				printf("Not a valid input.\n");
			}
		}
	}

	debug.Init(exe_path, sleep_ms);

	system("cls");
	Disassembler dis = Disassembler(&debug);
	switch (selected_game)
	{
	case ModernWarfare:
	case ModernWarfare2steam:
	case ModernWarfare2bnet:
		dis.Dump_ClientBase(debug.scanner->Find_Pattern("4C 8B 83 ? ? ? ? 90 C6 44 24 ? ? 0F B6 44 24"));
		dis.Dump_ClientInfo_MW(debug.scanner->Find_Pattern("48 8B 4C 24 ? BA ? ? ? ? 0F B7"));
		dis.Dump_Cbuff(debug.scanner->Find_Pattern("48 8B 0D ? ? ? ? 48 8B F0 C6 44 24"));
		break;
	case Vanguard:
		dis.Dump_ClientInfo_Vanguard(debug.scanner->Find_Pattern("48 8B 83 ?? ?? ?? ?? C6 44 24 ?? ?? 0F B6 4C 24 ?? C0"));
		dis.Dump_ClientBase(debug.scanner->Find_Pattern("FF 90 ? ? ? ? 48 8B 13 48 8B CB 48 89 85 ? ? ? ? 8B 87 ? ? ? ? 89 44 24 48 4C 8B 82 ? ? ? ? 8B D0 41 FF D0 "));
		break;
	}
	dis.Dump_BoneBase(debug.scanner->Find_Pattern("0F BF B4 ?? ?? ?? ?? ?? 89 ?? 24 ?? 85"));
	dis.Dump_BoneIndex(debug.scanner->Find_Pattern("84 ?? 0F 84 ?? ?? ?? ?? 48 ?? ?? C8 13 00 00"));

	if (selected_game != Game::Vanguard)
		dis.Dump_Offsets_MW();
	std::getchar();
}