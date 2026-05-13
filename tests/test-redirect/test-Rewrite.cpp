#include <redirect/Rewrite.h>

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

TEST(TestRewrite, set_scheme)
{
    redirect::Rewrite rewrite("http://example.com/search");

    boost::urls::url url = rewrite.set_scheme("https")();

    ASSERT_EQ(url.buffer(), "https://example.com/search");
}

TEST(TestRewrite, set_host)
{
    redirect::Rewrite rewrite("https://example.com/search?q=boost%20url&page=2#results");

    boost::urls::url url = rewrite.set_host("api.example.com")();

    ASSERT_EQ(url.buffer(), "https://api.example.com/search?q=boost%20url&page=2#results");
}

TEST(TestRewrite, set_port)
{
    redirect::Rewrite rewrite("https://example.com/search");

    boost::urls::url url = rewrite.set_port("8443")();

    ASSERT_EQ(url.buffer(), "https://example.com:8443/search");
}

TEST(TestRewrite, set_path)
{
    redirect::Rewrite rewrite("https://example.com/search?q=boost%20url");

    boost::urls::url url = rewrite.set_path("/v1/search")();

    ASSERT_EQ(url.buffer(), "https://example.com/v1/search?q=boost%20url");
}

TEST(TestRewrite, set_query_param)
{
    redirect::Rewrite rewrite("https://example.com/search?q=boost%20url&page=2");

    boost::urls::url url = rewrite.set_query_param("page", "3")();

    ASSERT_EQ(url.buffer(), "https://example.com/search?q=boost%20url&page=3");
}

TEST(TestRewrite, remove_query_param)
{
    redirect::Rewrite rewrite("https://example.com/search?q=boost%20url&page=2&page=3");

    boost::urls::url url = rewrite.remove_query_param("page")();

    ASSERT_EQ(url.buffer(), "https://example.com/search?q=boost%20url");
}

TEST(TestRewrite, remove_query)
{
    redirect::Rewrite rewrite("https://example.com/search?q=boost%20url&page=2#results");

    boost::urls::url url = rewrite.remove_query()();

    ASSERT_EQ(url.buffer(), "https://example.com/search#results");
}

TEST(TestRewrite, set_fragment)
{
    redirect::Rewrite rewrite("https://example.com/search?q=boost%20url#results");

    boost::urls::url url = rewrite.set_fragment("details")();

    ASSERT_EQ(url.buffer(), "https://example.com/search?q=boost%20url#details");
}

TEST(TestRewrite, remove_fragment)
{
    redirect::Rewrite rewrite("https://example.com/search?q=boost%20url#results");

    boost::urls::url url = rewrite.remove_fragment()();

    ASSERT_EQ(url.buffer(), "https://example.com/search?q=boost%20url");
}

TEST(TestRewrite, empty_strings_do_nothing)
{
    redirect::Rewrite rewrite("https://example.com:8443/search?q=boost%20url&page=2#results");

    boost::urls::url url = rewrite.set_scheme("")
        .set_host("")
        .set_port("")
        .set_path("")
        .set_query_param("", "3")
        .set_query_param("page", "")
        .remove_query_param("")
        .set_fragment("")();

    ASSERT_EQ(url.buffer(), "https://example.com:8443/search?q=boost%20url&page=2#results");
}

TEST(TestRewrite, chains_range_and_conditional_methods)
{
    struct QueryChange
    {
        std::string key;
        std::string value;
    };

    const std::vector<QueryChange> set_query{{"page", "3"}};
    const std::vector<std::string> remove_param{"q"};

    boost::urls::url url = redirect::Rewrite("https://example.com/search?q=boost%20url&page=2#results")
        .set_host("api.example.com")
        .set_path("/v1/search")
        .remove_query(false)
        .remove_query_params(remove_param)
        .set_query_params(set_query)
        .remove_fragment(true)();

    ASSERT_EQ(url.buffer(), "https://api.example.com/v1/search?page=3");
}

TEST(TestRewrite, throws_for_invalid_url)
{
    ASSERT_THROW(redirect::Rewrite("http://example.com/%XX"), std::runtime_error);
}
