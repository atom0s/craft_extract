/**
 * craft_extract - Copyright (c) 2022 atom0s [atom0s@live.com]
 *
 * This file is part of craft_extract.
 *
 * craft_extract is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * craft_extract is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with craft_extract.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "defines.hpp"
#include "v66.hpp"
#include "v67.hpp"

#include "cxxopts.hpp"

/**
 * Application entry point.
 *
 * @param {int32_t} argc - The argument count.
 * @param {char*[]} argv - The argument array.
 * @return {int32_t} 0 on success, 1 on general error, 2 on exception.
 */
int32_t __cdecl main(int32_t argc, char* argv[])
{
    ::printf_s("Dark Age of Camelot Craft Information Extractor.\n");
    ::printf_s("(c) 2022 atom0s [atom0s@live.com]\n\n");
    ::printf_s("Contact  : https://atom0s.com/\n");
    ::printf_s("Contact  : https://twitter.com/atom0s\n");
    ::printf_s("Contact  : https://discord.gg/UmXNvjq - atom0s#0001\n");
    ::printf_s("Donations: https://www.paypal.me/atom0s\n");
    ::printf_s("Donations: https://github.com/sponsors/atom0s\n");
    ::printf_s("Donations: https://patreon.com/atom0s\n\n");

    // Prepare supported parsers map..
    std::map<int32_t, std::tuple<craft_extract::parse_f, craft_extract::save_f>> parsers = {
        // v1.86 to v1.124b
        {0x66, std::make_tuple<craft_extract::parse_f, craft_extract::save_f>(craft_extract::parser::v66::parse, craft_extract::parser::v66::save)},

        // v1.127e
        {0x67, std::make_tuple<craft_extract::parse_f, craft_extract::save_f>(craft_extract::parser::v67::parse, craft_extract::parser::v67::save)},
    };

    try
    {
        std::string path_input;
        std::string path_output;
        auto mode  = craft_extract::output_mode::none;
        auto mode_ = 0;

        cxxopts::Options options("craft_extract", "Extracts binary serialized craft information for DAoC.");
        options.custom_help("[options...]");
        options.add_options()
            /**/ ("f,file", "The input file to extract craft information from. (ie. tdl.crf)", cxxopts::value<std::string>(path_input))
            /**/ ("o,out", "The output file to save the extracted craft information to.", cxxopts::value<std::string>(path_output))
            /**/ ("m,mode", "The output file saving mode.", cxxopts::value<int32_t>(mode_)->default_value("0"));

        options.parse(argc, argv);

        // Obtain the mode value..
        mode = static_cast<craft_extract::output_mode>(mode_);

        // Check for valid arguments..
        if (argc <= 1 || path_input.size() == 0 || path_output.size() == 0 || mode == craft_extract::output_mode::none)
        {
            std::cout << options.help() << std::endl;
            std::cout << "Modes:" << std::endl
                      << "  0 - none; will cause help info to display." << std::endl
                      << "  1 - csv     - Information saved into a comma-separated value file." << std::endl
                      << "  2 - json    - Information saved into a JSON formatted file." << std::endl
                      << "  3 - sqlite  - Information saved into an SQLite database file." << std::endl
                      << "  4 - text    - Information saved into a plain-text file." << std::endl;

            return 1;
        }

        // Ensure the input file exists..
        if (::GetFileAttributesA(path_input.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            std::cout << "[!] Error: Invalid input file given." << std::endl;
            return 1;
        }

        // Open the input file for reading..
        FILE* f = nullptr;
        if (::fopen_s(&f, path_input.c_str(), "rb") != ERROR_SUCCESS)
        {
            std::cout << "[!] Error: Failed to open input file for reading." << std::endl;
            return 1;
        }

        // Obtain and validate the file size..
        ::fseek(f, 0, SEEK_END);
        const auto size = ::_ftelli64(f);
        ::fseek(f, 0, SEEK_SET);

        if (size < 4)
        {
            ::fclose(f);
            std::cout << "[!] Error: Input file too small; cannot parse." << std::endl;
            return 1;
        }

        // Read and validate the header version..
        uint32_t version = 0;
        ::fread(&version, sizeof(version), 1, f);
        ::fseek(f, 0, SEEK_SET);

        if (!parsers.contains(version))
        {
            ::fclose(f);
            std::cout << std::format("[!] Error: Unsupported header version: {:08X}", version) << std::endl;
            return 1;
        }

        // Parse and save the read data..
        if (!(std::get<0>(parsers[version])(f, size)) ||
            !(std::get<1>(parsers[version])(path_output, mode)))
        {
            ::fclose(f);
            return 1;
        }

        ::fclose(f);
        std::cout << "[!] Done!" << std::endl;
        return 0;
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        std::cout << "[!] Error: Caught exception parsing arguments:"
                  << std::endl
                  << std::endl
                  << e.what()
                  << std::endl;

        return 2;
    }
}
