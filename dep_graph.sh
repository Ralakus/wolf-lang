#!/usr/bin/env bash

cargo deps \
    --include-orphans \
    --subgraph wolf \
    --subgraph-name "Wolf" \
    | dot -Tpng > dep_graph.png 