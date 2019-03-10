FROM python:3.7.0-alpine3.8

# add our user and group first to make sure their IDs get assigned consistently, regardless of whatever dependencies get added
RUN addgroup -g 501 -S bxextensions && \
	adduser -u 501 -S -G bxextensions bxextensions && \
	mkdir -p /app/bxextensions && \
	chown -R bxextensions:bxextensions /app/bxextensions && \
 	chmod o+w /usr/local/lib/python3.7/site-packages
RUN apk update \
 && apk add --no-cache \
# grab su-exec for easy step-down from root
        'su-exec>=0.2' \
	tini \
# grab bash for the convenience
        bash \
# grab requirements for bxextenstions
        gcc libc-dev unixodbc-dev make \
	build-base \
	automake \
# 	autogen \
	autoconf \
	libtool \
#	libtool-bin \
	openssl-dev \
	cmake \
	libexecinfo-dev

WORKDIR /app/bxextensions
COPY docker-entrypoint.sh /usr/local/bin/
COPY . .
# RUN rm -rf build

ENV PYTHONPATH=/app/bxextensions
RUN python setup.py develop

# ENTRYPOINT ["/sbin/tini", "--", "/bin/sh", "/usr/local/bin/docker-entrypoint.sh"]
