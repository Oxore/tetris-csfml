export PFX=../../prefix

mkdir -p $PFX deps/SFML/build deps/CSFML/build

# Enable CSFML static compilation
git -C deps/CSFML apply ../CSFML-static.patch

pushd deps/SFML/build

cmake \
  -G "Unix Makefiles" \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_INSTALL_PREFIX=$PFX \
  -DSFML_MISC_INSTALL_PREFIX=$PFX \
  -DSFML_DEPENDENCIES_INSTALL_PREFIX=$PFX \
  .. \
  && make install

popd

pushd deps/CSFML/build

cmake \
  -G "Unix Makefiles" \
  -DBUILD_SHARED_LIBS=OFF \
  -DCSFML_LINK_SFML_STATICALLY=TRUE \
  -DCMAKE_PREFIX_PATH=$PFX \
  -DCMAKE_INSTALL_PREFIX=$PFX \
  .. \
  && make install

popd

make PREFIX=deps/prefix SFML_STATIC=y Q=
