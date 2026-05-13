#include <redirect/UrlMapper.h>

#include <gtest/gtest.h>

#include <stdexcept>

TEST(TestUrlMapper, maps_matching_path_prefix)
{
    redirect::UrlMapper mapper;

    boost::urls::url url = mapper.add_rule("/api", "https://api.internal/v1")
        ("https://proxy.example.com/api/users?id=42#row");

    ASSERT_EQ(url.buffer(), "https://api.internal/v1/users?id=42#row");
}

TEST(TestUrlMapper, uses_longest_matching_path_prefix)
{
    redirect::UrlMapper mapper;

    boost::urls::url url = mapper.add_rule("/api", "https://api.internal")
        .add_rule("/api/admin", "https://admin.internal/root")
        ("https://proxy.example.com/api/admin/users");

    ASSERT_EQ(url.buffer(), "https://admin.internal/root/users");
}

TEST(TestUrlMapper, preserves_encoded_query)
{
    redirect::UrlMapper mapper;

    boost::urls::url url = mapper.add_rule("/search", "https://search.internal")
        ("https://proxy.example.com/search?q=boost%20url&page=2");

    ASSERT_EQ(url.buffer(), "https://search.internal/?q=boost%20url&page=2");
}

TEST(TestUrlMapper, throws_when_no_rule_matches)
{
    redirect::UrlMapper mapper;

    ASSERT_THROW(mapper.add_rule("/api", "https://api.internal")("https://proxy.example.com/other"), std::runtime_error);
}

TEST(TestUrlMapper, throws_for_invalid_incoming_url)
{
    redirect::UrlMapper mapper;

    ASSERT_THROW(mapper.add_rule("/api", "https://api.internal")("https://proxy.example.com/%XX"), std::runtime_error);
}

TEST(TestUrlMapper, throws_for_invalid_rule)
{
    redirect::UrlMapper mapper;

    ASSERT_THROW(mapper.add_rule("", "https://api.internal"), std::runtime_error);
    ASSERT_THROW(mapper.add_rule("/api", "https://api.internal/%XX"), std::runtime_error);
}
