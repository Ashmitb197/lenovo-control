pkgname=lenovo-control
pkgver=0.5.0
pkgrel=1
pkgdesc='Unofficial Lenovo laptop hardware controller with Qt 6 dashboard and Plasma integration'
arch=('x86_64')
url='https://github.com/Ashmitb197/lenovo-control'
license=('MIT')

depends=(
  'python'
  'polkit'
  'libnotify'
  'qt6-base'
  'qt6-declarative'
)

makedepends=('cmake' 'ninja' 'git')

optdepends=(
  'plasma-workspace: KDE Plasma 6 widget integration'
  'nvidia-utils: NVIDIA GPU monitoring'
)

source=("lenovo-control::git+https://github.com/Ashmitb197/lenovo-control.git#commit=f6af4e5348720c649544411f39a83c03b0d0be4c")
sha256sums=('SKIP')

build() {
  cmake -S "$srcdir/lenovo-control" \
    -B "$srcdir/build" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DBUILD_TESTING=ON \
    -DLENOVO_CONTROL_TRAY=OFF \
    -DLENOVO_CONTROL_PLASMA=ON

  cmake --build "$srcdir/build" -j "$(nproc)"
}

check() {
  ctest --test-dir "$srcdir/build" --output-on-failure
}

package() {
  local source_root="$srcdir/lenovo-control"

  DESTDIR="$pkgdir" cmake --install "$srcdir/build"

  install -Dm755 "$source_root/assets/lenovo-control" \
    "$pkgdir/usr/bin/lenovo-control"

  install -Dm755 "$source_root/assets/lenovo-control-helper" \
    "$pkgdir/usr/lib/lenovo-control/lenovo-control-helper"

  install -Dm644 "$source_root/LICENSE" \
    "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
