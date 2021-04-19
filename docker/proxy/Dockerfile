FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y \
        iptables \
        simpleproxy \
        && \
    rm -rf /var/lib/apt/lists/*

VOLUME /code

WORKDIR /code

EXPOSE 5432

ENTRYPOINT scripts/run_proxy.sh
