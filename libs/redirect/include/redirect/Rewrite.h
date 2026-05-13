#pragma once

#include <boost/url.hpp>

#include <string>

namespace redirect
{

class Rewrite
{
public:
    explicit Rewrite(const std::string &original_url);

    Rewrite &set_scheme(const std::string &scheme);
    Rewrite &set_host(const std::string &host);
    Rewrite &set_port(const std::string &port);
    Rewrite &set_path(const std::string &path);
    Rewrite &set_query_param(const std::string &key, const std::string &value);
    Rewrite &remove_query_param(const std::string &key);
    Rewrite &remove_query();
    Rewrite &remove_query(bool remove);
    Rewrite &set_fragment(const std::string &fragment);
    Rewrite &remove_fragment();
    Rewrite &remove_fragment(bool remove);

    template <class QueryChanges>
    Rewrite &set_query_params(const QueryChanges &changes)
    {
        for (const auto &change : changes)
        {
            set_query_param(change.key, change.value);
        }

        return *this;
    }

    template <class Keys>
    Rewrite &remove_query_params(const Keys &keys)
    {
        for (const auto &key : keys)
        {
            remove_query_param(key);
        }

        return *this;
    }

    boost::urls::url operator()() const;

private:
    boost::urls::url m_url;
};

} // namespace redirect
