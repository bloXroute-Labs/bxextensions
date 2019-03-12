#FROM python:3.7.0-alpine3.8
FROM oryxprod/python-3.7

# add our user and group first to make sure their IDs get assigned consistently, regardless of whatever dependencies get added
RUN addgroup -gid 503 -Sys bxextensions && \
	adduser -uid 503 -Sys -gid 503 bxextensions && \
	mkdir -p /app/bxextensions && \
	chown -R bxextensions:bxextensions /app/bxextensions 

RUN apt update \
 && apt upgrade -y \
 && apt install -y build-essential automake autogen autoconf libtool libtool-bin python python-pip libssl-dev cmake

#upgrade pip
RUN pip3 install --upgrade pip setuptools

WORKDIR /app/bxextensions
COPY docker-entrypoint.sh /usr/local/bin/
COPY . .
RUN rm -rf build

ENV PYTHONPATH=/app/bxextensions
RUN python3 build_extensions.py --src-dir /app/bxextensions
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
