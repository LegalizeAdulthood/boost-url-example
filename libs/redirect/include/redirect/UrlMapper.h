#pragma once

#include <boost/url.hpp>

#include <string>
#include <vector>

namespace redirect
{

class MappingRule
{
public:
    MappingRule(const std::string &path_prefix, const std::string &target_base_url);

    const std::string &path_prefix() const;
    const boost::urls::url &target_base_url() const;

private:
    std::string m_path_prefix;
    boost::urls::url m_target_base_url;
};

class UrlMapper
{
public:
    UrlMapper &add_rule(const std::string &path_prefix, const std::string &target_base_url);

    boost::urls::url operator()(const std::string &incoming_url) const;

private:
    std::vector<MappingRule> m_rules;
};

}
