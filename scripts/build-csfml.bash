#!/bin/bash

set +e

log() {
  echo -e $@
}

if [ -z "$PREFIX" ]; then
  log "PREFIX environment variable must be defined"
  exit 1
fi

log "Prefix is ${PREFIX}"

mkdir -p deps/CSFML/build

pushd deps/CSFML/build \
&& cmake \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DCSFML_LINK_SFML_STATICALLY=TRUE \
  -DCMAKE_PREFIX_PATH=$PREFIX \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  .. \
&& cmake --build . --target install \
&& popd
