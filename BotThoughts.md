# Bot Thoughts

Notable characteristics of Boost.URL:

- Zero-allocation parsing
  - Core parsers operate on `string_view`-like buffers.
  - A parsed URL can reference the original input without copying.
  - Useful for HTTP servers, proxies, routers, embedded systems.
- Grammar-oriented design
  - Closely tracks RFC 3986 URL grammar.
  - Exposes parsed substructures directly:
    - `scheme`
    - `authority`
    - `host`
    - `path`
    - `query`
    - `fragment`
  - Parsers are composable internally using Boost-style grammar rules.
- `url_view` vs `url`
  - `url_view`
    - non-owning
    - immutable
    - no allocations
  - `url`
    - owning
    - mutable
    - optimized mutation API
  - This split is unusually clean compared to many URL libraries.
- Percent-encoding as a first-class abstraction
  - Dedicated encoded/decoded views:
    - `segments_view`
    - `params_view`
    - encoded variants
  - Avoids accidental double-decoding or re-encoding bugs.
  - Preserves original escaping where required.
- Structured path/query access
  - Query parameters exposed as iterable key/value ranges.
  - Path segments exposed as iterable ranges.
  - No manual tokenization required.

Prompt:
> summarize the novel and interesting things about the boost.url library

ChatGPT Plus, May 13th, 2026
