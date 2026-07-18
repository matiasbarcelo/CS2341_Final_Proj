# SuperSearch API host — builds and runs the real C++ engine over HTTP.
# Local:  docker build -t supersearch-api . && docker run --rm -p 8080:8080 supersearch-api
# Deploy: fly deploy  (see fly.toml)

FROM debian:bookworm-slim AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY Code/ /src/

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --target supersearch_api -j"$(nproc)"

FROM debian:bookworm-slim AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Runtime assets the engine needs (stopwords + article JSON corpus)
COPY Code/sample_data /app/sample_data
COPY Code/stopwords /app/stopwords
COPY --from=build /src/build/supersearch_api /app/supersearch_api

ENV PORT=8080 \
    HOST=0.0.0.0 \
    SUPERSEARCH_DATA_DIR=/app/sample_data \
    SUPERSEARCH_CORS=*

EXPOSE 8080
CMD ["/app/supersearch_api"]
