# url-tool

`url-tool` inspects a URL and can rewrite selected URL components. It uses
Boost.URL for parsing and the `redirect::Rewrite` helper from `libs/redirect`
for mutation.

# Usage

```
url-tool [options] <url>
```

With no rewrite options, the tool prints the parsed URL parts:

```
url-tool "https://example.com/search?q=boost%20url&page=2#results"
```

Printed fields include scheme, relative status, authority, userinfo, host,
host type, port, path, query parameters, and fragment.

# Rewrite Options

```
--set-scheme <scheme>       Replace the scheme, such as https
--set-host <host>           Replace the host, including IPv4/IPv6 names
--set-port <port>           Replace the port
--set-path <path>           Replace the path
--set-query <key=value>     Set one query parameter, repeatable
--remove-param <key>        Remove all matching query parameters, repeatable
--remove-query              Remove the whole query string
--set-fragment <fragment>   Replace the fragment
--remove-fragment           Remove the fragment
--help                      Show help
```

When rewrite options are present, the tool prints the original URL analysis,
then prints the rewritten URL.

# Example

```
url-tool --set-host api.example.com --set-path /v1/search --set-query page=3 --remove-fragment "https://example.com/search?q=boost%20url&page=2#results"
```

The rewritten URL is:

```
https://api.example.com/v1/search?q=boost%20url&page=3
```
