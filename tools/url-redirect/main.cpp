#include <redirect/UrlMapper.h>

#include <boost/url.hpp>

#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{

struct MappingOption
{
    std::string path_prefix;
    std::string target_base_url;
};

struct Options
{
    bool help = false;
    std::string incoming_url;
    std::vector<MappingOption> mappings;
};

void print_usage()
{
    std::cout << "usage: url-redirect --map <path-prefix>=<target-base-url> [--map ...] <incoming-url>\n"
              << "\n"
              << "Map an incoming proxy URL to an upstream URL.\n"
              << "\n"
              << "example:\n"
              << "  url-redirect --map /api=https://api.internal/v1 "
                 "\"https://proxy.example.com/api/users?id=42\"\n";
}

std::string required_value(int &index, int argc, char *argv[], const std::string &option)
{
    if (index + 1 >= argc)
    {
        throw std::runtime_error(option + " requires a value");
    }

    ++index;
    return argv[index];
}

MappingOption parse_mapping(std::string text)
{
    const auto separator = text.find('=');
    if (separator == std::string::npos)
    {
        throw std::runtime_error("--map expects path-prefix=target-base-url");
    }

    return {text.substr(0, separator), text.substr(separator + 1)};
}

Options parse_options(int argc, char *argv[])
{
    Options result;

    for (int index = 1; index < argc; ++index)
    {
        std::string arg = argv[index];

        if (arg == "--help" || arg == "-h")
        {
            result.help = true;
        }
        else if (arg == "--map")
        {
            result.mappings.push_back(parse_mapping(required_value(index, argc, argv, arg)));
        }
        else if (!arg.empty() && arg[0] == '-')
        {
            throw std::runtime_error("unknown option: " + arg);
        }
        else if (result.incoming_url.empty())
        {
            result.incoming_url = std::move(arg);
        }
        else
        {
            throw std::runtime_error("only one incoming URL argument is expected");
        }
    }

    return result;
}

boost::urls::url map_url(const Options &opts)
{
    redirect::UrlMapper mapper;

    for (const MappingOption &mapping : opts.mappings)
    {
        mapper.add_rule(mapping.path_prefix, mapping.target_base_url);
    }

    return mapper(opts.incoming_url);
}

}

int main(int argc, char *argv[])
{
    try
    {
        Options opts = parse_options(argc, argv);
        if (opts.help)
        {
            print_usage();
            return 0;
        }

        if (opts.incoming_url.empty() || opts.mappings.empty())
        {
            print_usage();
            return 1;
        }

        boost::urls::url mapped_url = map_url(opts);
        std::cout << mapped_url.buffer() << "\n";

        return 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "error: " << ex.what() << "\n";
        return 1;
    }
}
