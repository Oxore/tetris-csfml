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

mkdir -p deps/SFML/build

pushd deps/SFML/build \
&& cmake \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DSFML_MISC_INSTALL_PREFIX=$PREFIX \
  -DSFML_DEPENDENCIES_INSTALL_PREFIX=$PREFIX \
  .. \
&& cmake --build . --target install \
&& popd
