# SuperSearch HTTP API

This host runs the **same C++ search pipeline** as the terminal `supersearch` program:

`HTTP request → SearchEngine → QueryEngine → IndexHandler (AVL inverted indexes)`

Only the UI layer differs: JSON over HTTP instead of `cin` / `cout`.

## Run locally (Docker)

```bash
docker compose up --build
# or:
docker build -t supersearch-api .
docker run --rm -p 8080:8080 supersearch-api
```

Open http://localhost:8080 and try http://localhost:8080/api/search?q=brexit

## Run locally (native)

From `Code/`:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target supersearch_api
# cwd can be build/ or Code/; sample_data is resolved automatically
./build/supersearch_api
```

Environment:

| Variable | Default | Meaning |
|----------|---------|---------|
| `PORT` | `8080` | Listen port |
| `HOST` | `0.0.0.0` | Bind address |
| `SUPERSEARCH_DATA_DIR` | auto (`sample_data`) | Directory of article JSON files |
| `SUPERSEARCH_CORS` | `*` | `Access-Control-Allow-Origin` for browser demos |

## Endpoints

### `GET /api/health`

Service liveness.

### `GET /api/stats`

Index sizes (`word_count`, `people_count`, `org_count`) after startup indexing.

### `GET /api/search?q=...&page=0&page_size=15`

Same query language as the CLI:

- `privacy`
- `PERSON: eric schweitzer`
- `brexit ORG: reuters` (use `ORG:` / `PERSON:` tokens as in the original project)

Response:

```json
{
  "query": "brexit",
  "total": 2,
  "page": 0,
  "page_size": 15,
  "took_seconds": 0.001,
  "results": [
    {
      "index": 0,
      "score": 12,
      "id": "coll_1/news_0064567.json",
      "title": "...",
      "publication": "www.reuters.com",
      "published": "2018-02-27T20:09:00.000+02:00"
    }
  ]
}
```

`score` is the SuperSearch score (term frequency sum across query terms).

### `GET /api/article?id=coll_1/news_0064567.json`

Returns article body text. `id` must resolve under the configured data directory (path traversal is rejected).

## Deploy (Fly.io)

1. Install the [Fly CLI](https://fly.io/docs/hands-on/install-flyctl/) and log in.
2. From the repo root, adjust `app` in `fly.toml` if the name is taken.
3. Deploy:

```bash
fly deploy
```

4. Note the public URL (e.g. `https://supersearch-api.fly.dev`) and point the personal website demo at it:

```js
// supersearch-demo.html
const API_BASE = "https://supersearch-api.fly.dev";
```

Cold starts: `fly.toml` sets `min_machines_running = 0` to save free-tier credit; the first request after idle may take a few seconds while the machine boots and re-indexes `sample_data`.

## Demo corpus

By default the host indexes `Code/sample_data` (6 finance news JSON files). To use a larger set, mount or copy more JSON files and set `SUPERSEARCH_DATA_DIR`.

Persistence files from the course project are optional for the API host: on startup it always builds the in-memory index from the JSON directory (same `DocParser` path as the CLI without persistence).
