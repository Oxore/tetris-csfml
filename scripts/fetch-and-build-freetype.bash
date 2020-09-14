#!/bin/bash

set +e

FREETYPE_DIR=$(pwd)/deps/freetype
FREETYPE_VERSION="2.10.2"
FREETYPE_URL="https://download.savannah.gnu.org/releases/freetype/freetype-${FREETYPE_VERSION}.tar.xz"
FREETYPE_DIR_UNPACKED=${FREETYPE_DIR}/freetype-${FREETYPE_VERSION}

log() {
  echo -e $@
}

if [ -z "$PREFIX" ]; then
  log "PREFIX environment variable must be defined"
  exit 1
fi

log "Prefix is ${PREFIX}"

# Download and extract phase

if [ ! -d ${FREETYPE_DIR} ]; then
  log -n "Creating ${FREETYPE_DIR}..."
  mkdir -p ${FREETYPE_DIR}
  log "Done"
fi

if [ ! -f ${FREETYPE_DIR}/freetype-${FREETYPE_VERSION}.tar.xz ]; then
  log -n "Downloading ${FREETYPE_URL} to ${FREETYPE_DIR}/freetype-${FREETYPE_VERSION}.tar.xz..."
  curl ${FREETYPE_URL} \
    -Lo ${FREETYPE_DIR}/freetype-${FREETYPE_VERSION}.tar.xz
  log "Done"
fi

if [ ! -d ${FREETYPE_DIR}/freetype-${FREETYPE_VERSION} ]; then
  log -n "Unpacking..."
  tar xf ${FREETYPE_DIR}/freetype-${FREETYPE_VERSION}.tar.xz -C ${FREETYPE_DIR}
  log "Done"
fi

log -n "Assert unpacked dir ${FREETYPE_DIR_UNPACKED} exists..."
if [ -d ${FREETYPE_DIR_UNPACKED} ]; then
  log "Yes it is"
else
  log "No. Aborting"
fi

if [ ! -d ${FREETYPE_DIR_UNPACKED}/build ]; then
  log -n "Creating freetype's build directory ${FREETYPE_DIR}/build..."
  mkdir -p ${FREETYPE_DIR_UNPACKED}/build
  log "Done"
fi

# Build and install phase

log -n "Building with cmake in ${FREETYPE_DIR_UNPACKED}/build..."
pushd ${FREETYPE_DIR_UNPACKED}/build \
&& cmake .. -D CMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE \
            -D CMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE \
            -D CMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE \
            -D CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE \
            -D CMAKE_DISABLE_FIND_PACKAGE_BrotliDec=TRUE \
            -D CMAKE_INSTALL_PREFIX=$PREFIX \
&& cmake --build . --target install \
&& popd
log "Done"


