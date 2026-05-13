#pragma once

#include <boost/url.hpp>

#include <string>
#include <vector>

namespace redirect
{

class UrlMapper
{
public:
    UrlMapper &add_rule(const std::string &path_prefix, const std::string &target_base_url);

    boost::urls::url operator()(const std::string &incoming_url) const;

private:
    struct MappingRule
    {
        std::string path_prefix;
        boost::urls::url target_base_url;
    };

    std::vector<MappingRule> m_rules;
};

} // namespace redirect
