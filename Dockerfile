FROM python:3.7.0-alpine3.8

# add our user and group first to make sure their IDs get assigned consistently, regardless of whatever dependencies get added
RUN addgroup -g 503 -S bxextensions && \
	adduser -u 503 -S -gid bxextensions bxextensions && \
	mkdir -p /app/bxextensions && \ 
        chown -R bxextensions:bxextensions /app/bxextensions

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
    gfortran \
    python3 \
    libc6-compat \
    freetype-dev \
    openblas-dev \
    tcl \
    tk \
    python3-dev \
    pkgconfig \
    musl \
    libgfortran \
    libgcc \
       automake \
#      autogen \
       autoconf \
       libtool \
#      libtool-bin \
       openssl-dev \
       cmake \
       libexecinfo-dev

#upgrade pip
RUN pip3 install --upgrade pip setuptools

WORKDIR /app/bxextensions
COPY docker-entrypoint.sh /usr/local/bin/
COPY . .
RUN rm -rf build

ENV PYTHONPATH=/app/bxextensions
RUN python3 build_extensions.py --src-dir /app/bxextensions
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
