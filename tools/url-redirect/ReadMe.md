# url-redirect

`url-redirect` maps an incoming proxy URL to an upstream URL by applying one or
more path-prefix mapping rules. It is a small command-line wrapper around the
`redirect::UrlMapper` library code in `libs/redirect`.

# Usage

```
url-redirect --map <path-prefix>=<target-base-url> [--map ...] <incoming-url>
```

Each `--map` value has two parts:

- `<path-prefix>` is the incoming URL path prefix to match.
- `<target-base-url>` is the upstream base URL to receive the rewritten request.

For example:

```
url-redirect --map /api=https://api.internal/v1 "https://proxy.example.com/api/users?id=42"
```

This prints:

```
https://api.internal/v1/users?id=42
```

# Mapping Behavior

When more than one rule matches, `url-redirect` uses the longest matching path
prefix. This lets more specific rules override broader ones:

```
url-redirect --map /api=https://api.internal --map /api/admin=https://admin.internal/root "https://proxy.example.com/api/admin/users"
```

This maps to:

```
https://admin.internal/root/users
```

The tool preserves the incoming URL's query string and fragment when present.
It reports an error when the incoming URL is invalid, a mapping rule is invalid,
or no rule matches the incoming path.
