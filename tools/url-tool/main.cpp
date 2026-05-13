#include <boost/url.hpp>
#include <redirect/Rewrite.h>

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace urls = boost::urls;

namespace
{

struct QueryChange
{
    std::string key;
    std::string value;
};

struct Options
{
    bool help = false;
    bool remove_fragment = false;
    bool remove_query = false;
    std::string input;
    std::string scheme;
    std::string host;
    std::string port;
    std::string path;
    std::string fragment;
    std::vector<QueryChange> set_query;
    std::vector<std::string> remove_param;
};

template <class StringView>
std::string to_string_view(const StringView &value)
{
    return std::string(value.data(), value.size());
}

void print_usage()
{
    std::cout << "usage: url-tool [options] <url>\n"
              << "\n"
              << "Inspect and safely rewrite a URL with Boost.URL.\n"
              << "\n"
              << "options:\n"
              << "  --set-scheme <scheme>       Replace the scheme, such as https\n"
              << "  --set-host <host>           Replace the host, including IPv4/IPv6 names\n"
              << "  --set-port <port>           Replace the port\n"
              << "  --set-path <path>           Replace the path\n"
              << "  --set-query <key=value>     Set one query parameter, repeatable\n"
              << "  --remove-param <key>        Remove all matching query parameters, repeatable\n"
              << "  --remove-query              Remove the whole query string\n"
              << "  --set-fragment <fragment>   Replace the fragment\n"
              << "  --remove-fragment           Remove the fragment\n"
              << "  --help                      Show this help\n"
              << "\n"
              << "examples:\n"
              << "  url-tool \"https://example.com/search?q=boost%20url&page=2#results\"\n"
              << "  url-tool --set-host api.example.com --set-path /v1/search --set-query page=3 --remove-fragment "
                 "\"https://example.com/search?q=boost%20url&page=2#results\"\n";
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

QueryChange parse_query_change(std::string text)
{
    const auto separator = text.find('=');
    if (separator == std::string::npos)
    {
        throw std::runtime_error("--set-query expects key=value");
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
        else if (arg == "--set-scheme")
        {
            result.scheme = required_value(index, argc, argv, arg);
        }
        else if (arg == "--set-host")
        {
            result.host = required_value(index, argc, argv, arg);
        }
        else if (arg == "--set-port")
        {
            result.port = required_value(index, argc, argv, arg);
        }
        else if (arg == "--set-path")
        {
            result.path = required_value(index, argc, argv, arg);
        }
        else if (arg == "--set-query")
        {
            result.set_query.push_back(parse_query_change(required_value(index, argc, argv, arg)));
        }
        else if (arg == "--remove-param")
        {
            result.remove_param.push_back(required_value(index, argc, argv, arg));
        }
        else if (arg == "--remove-query")
        {
            result.remove_query = true;
        }
        else if (arg == "--set-fragment")
        {
            result.fragment = required_value(index, argc, argv, arg);
        }
        else if (arg == "--remove-fragment")
        {
            result.remove_fragment = true;
        }
        else if (!arg.empty() && arg[0] == '-')
        {
            throw std::runtime_error("unknown option: " + arg);
        }
        else if (result.input.empty())
        {
            result.input = std::move(arg);
        }
        else
        {
            throw std::runtime_error("only one URL argument is expected");
        }
    }

    return result;
}

std::string host_type_name(urls::host_type type)
{
    switch (type)
    {
    case urls::host_type::none:
        return "none";
    case urls::host_type::name:
        return "name";
    case urls::host_type::ipv4:
        return "ipv4";
    case urls::host_type::ipv6:
        return "ipv6";
    case urls::host_type::ipvfuture:
        return "ipvfuture";
    }

    return "unknown";
}

bool has_rewrites(const Options &opts)
{
    return !opts.scheme.empty() || !opts.host.empty() || !opts.port.empty() || !opts.path.empty() ||
        !opts.fragment.empty() || opts.remove_fragment || opts.remove_query || !opts.set_query.empty() ||
        !opts.remove_param.empty();
}

urls::url rewrite_url(const Options &opts)
{
    return redirect::Rewrite(opts.input)
        .set_scheme(opts.scheme)
        .set_host(opts.host)
        .set_port(opts.port)
        .set_path(opts.path)
        .remove_query(opts.remove_query)
        .remove_query_params(opts.remove_param)
        .set_query_params(opts.set_query)
        .set_fragment(opts.fragment)
        .remove_fragment(opts.remove_fragment)();
}

void print_url(const urls::url_view_base &url)
{
    std::cout << "input:    " << to_string_view(url.buffer()) << "\n"
              << "scheme:   " << to_string_view(url.scheme()) << "\n"
              << "relative: " << (url.has_scheme() ? "no" : "yes") << "\n";

    if (url.has_authority())
    {
        std::cout << "userinfo: " << (url.has_userinfo() ? to_string_view(url.userinfo()) : "") << "\n"
                  << "host:     " << to_string_view(url.host()) << "\n"
                  << "hosttype: " << host_type_name(url.host_type()) << "\n"
                  << "port:     " << (url.has_port() ? to_string_view(url.port()) : "") << "\n";
    }
    else
    {
        std::cout << "authority: <none>\n";
    }

    std::cout << "path:     " << to_string_view(url.path()) << "\n"
              << "query:    " << (url.has_query() ? to_string_view(url.query()) : "") << "\n";

    if (url.has_query())
    {
        for (const auto param : url.params())
        {
            std::cout << "  " << to_string_view(param.key);
            if (param.has_value)
            {
                std::cout << " = " << to_string_view(param.value);
            }
            std::cout << "\n";
        }
    }

    std::cout << "fragment: " << (url.has_fragment() ? to_string_view(url.fragment()) : "") << "\n";
}

} // namespace

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

        if (opts.input.empty())
        {
            print_usage();
            return 1;
        }

        urls::url url = redirect::Rewrite(opts.input)();
        print_url(url);

        if (has_rewrites(opts))
        {
            urls::url rewritten_url = rewrite_url(opts);
            std::cout << "\nrewritten:\n" << to_string_view(rewritten_url.buffer()) << "\n";
        }

        return 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "error: " << ex.what() << "\n";
        return 1;
    }
}
