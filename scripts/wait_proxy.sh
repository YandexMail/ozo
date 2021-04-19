#!/bin/bash -e

while ! pg_isready -h ${PROXY_HOST:?} -p 5432 -U ${POSTGRES_USER:?}; do
    echo "waiting until postgres at ${PROXY_HOST:?}:5432 is accepting connections..."
    sleep 1
done
