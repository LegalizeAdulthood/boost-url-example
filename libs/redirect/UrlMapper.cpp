#include <redirect/UrlMapper.h>

#include <stdexcept>

namespace
{

std::string to_string_view(boost::core::string_view value)
{
    return std::string(value.data(), value.size());
}

bool path_matches(const std::string &path, const std::string &prefix)
{
    if (prefix.empty() || path.size() < prefix.size())
    {
        return false;
    }

    if (path.substr(0, prefix.size()) != prefix)
    {
        return false;
    }

    return path.size() == prefix.size() || prefix.back() == '/' || path[prefix.size()] == '/';
}

std::string join_paths(const std::string &base_path, const std::string &suffix)
{
    if (base_path.empty() || base_path == "/")
    {
        return suffix.empty() ? "/" : suffix;
    }

    if (suffix.empty())
    {
        return base_path;
    }

    if (base_path.back() == '/' && suffix.front() == '/')
    {
        return base_path + suffix.substr(1);
    }

    if (base_path.back() != '/' && suffix.front() != '/')
    {
        return base_path + "/" + suffix;
    }

    return base_path + suffix;
}

} // namespace

namespace redirect
{

MappingRule::MappingRule(const std::string &path_prefix, const std::string &target_base_url) :
    m_path_prefix(path_prefix)
{
    if (m_path_prefix.empty())
    {
        throw std::runtime_error("path prefix must not be empty");
    }

    auto parsed = boost::urls::parse_uri_reference(target_base_url);
    if (!parsed)
    {
        throw std::runtime_error("invalid target URL: " + parsed.error().message());
    }

    m_target_base_url = boost::urls::url(*parsed);
}

const std::string &MappingRule::path_prefix() const
{
    return m_path_prefix;
}

const boost::urls::url &MappingRule::target_base_url() const
{
    return m_target_base_url;
}

UrlMapper &UrlMapper::add_rule(const std::string &path_prefix, const std::string &target_base_url)
{
    m_rules.emplace_back(path_prefix, target_base_url);
    return *this;
}

boost::urls::url UrlMapper::operator()(const std::string &incoming_url) const
{
    auto parsed = boost::urls::parse_uri_reference(incoming_url);
    if (!parsed)
    {
        throw std::runtime_error("invalid incoming URL: " + parsed.error().message());
    }

    boost::urls::url incoming(*parsed);
    std::string incoming_path = to_string_view(incoming.path());
    const MappingRule *match = nullptr;

    for (const MappingRule &rule : m_rules)
    {
        if (path_matches(incoming_path, rule.path_prefix()) &&
            (match == nullptr || rule.path_prefix().size() > match->path_prefix().size()))
        {
            match = &rule;
        }
    }

    if (match == nullptr)
    {
        throw std::runtime_error("no mapping rule matched incoming URL");
    }

    const boost::urls::url &target_base_url = match->target_base_url();
    std::string suffix = incoming_path.substr(match->path_prefix().size());
    boost::urls::url mapped = target_base_url;

    mapped.set_path(join_paths(to_string_view(target_base_url.path()), suffix));

    if (incoming.has_query())
    {
        mapped.set_encoded_query(incoming.encoded_query());
    }
    else
    {
        mapped.remove_query();
    }

    if (incoming.has_fragment())
    {
        mapped.set_fragment(to_string_view(incoming.fragment()));
    }
    else
    {
        mapped.remove_fragment();
    }

    return mapped;
}

} // namespace redirect
