#include <redirect/Rewrite.h>

#include <stdexcept>

namespace
{

template <class StringView>
std::string to_string_view(const StringView &value)
{
    return std::string(value.data(), value.size());
}

} // namespace

namespace redirect
{

Rewrite::Rewrite(const std::string &original_url)
{
    auto parsed = boost::urls::parse_uri_reference(original_url);
    if (!parsed)
    {
        throw std::runtime_error("invalid URL: " + parsed.error().message());
    }

    m_url = boost::urls::url(*parsed);
}

Rewrite &Rewrite::set_scheme(const std::string &scheme)
{
    if (scheme.empty())
    {
        return *this;
    }

    m_url.set_scheme(scheme);
    return *this;
}

Rewrite &Rewrite::set_host(const std::string &host)
{
    if (host.empty())
    {
        return *this;
    }

    m_url.set_host(host);
    return *this;
}

Rewrite &Rewrite::set_port(const std::string &port)
{
    if (port.empty())
    {
        return *this;
    }

    m_url.set_port(port);
    return *this;
}

Rewrite &Rewrite::set_path(const std::string &path)
{
    if (path.empty())
    {
        return *this;
    }

    m_url.set_path(path);
    return *this;
}

Rewrite &Rewrite::set_query_param(const std::string &key, const std::string &value)
{
    if (key.empty() || value.empty())
    {
        return *this;
    }

    m_url.params().set(key, value);
    return *this;
}

Rewrite &Rewrite::remove_query_param(const std::string &key)
{
    if (key.empty())
    {
        return *this;
    }

    auto params = m_url.params();
    for (auto iter = params.begin(); iter != params.end();)
    {
        if (to_string_view((*iter).key) == key)
        {
            iter = params.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    return *this;
}

Rewrite &Rewrite::remove_query()
{
    m_url.remove_query();
    return *this;
}

Rewrite &Rewrite::remove_query(bool remove)
{
    if (remove)
    {
        remove_query();
    }

    return *this;
}

Rewrite &Rewrite::set_fragment(const std::string &fragment)
{
    if (fragment.empty())
    {
        return *this;
    }

    m_url.set_fragment(fragment);
    return *this;
}

Rewrite &Rewrite::remove_fragment()
{
    m_url.remove_fragment();
    return *this;
}

Rewrite &Rewrite::remove_fragment(bool remove)
{
    if (remove)
    {
        remove_fragment();
    }

    return *this;
}

boost::urls::url Rewrite::operator()() const
{
    return m_url;
}

} // namespace redirect
