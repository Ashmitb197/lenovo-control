pkgname=lenovo-control
pkgver=0.4.0
pkgrel=1
pkgdesc='Unofficial Lenovo IdeaPad hardware controller with Qt 6 dashboard and Plasma integration'
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
  'pyside6: legacy Python dashboard'
  'plasma-workspace: KDE Plasma widget integration'
)

source=("git+https://github.com/Ashmitb197/lenovo-control.git#branch=dev/v0.4.0")
sha256sums=('SKIP')

build() {
  cmake -S "$srcdir/lenovo-control" \
    -B "$srcdir/build" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DLENOVO_CONTROL_TRAY=OFF \
    -DLENOVO_CONTROL_PLASMA=ON

  cmake --build "$srcdir/build" -j 4
}

package() {
  DESTDIR="$pkgdir" cmake --install "$srcdir/build"

  local source_root="$srcdir/lenovo-control"

  install -Dm755 "$source_root/assets/lenovo-control" \
    "$pkgdir/usr/bin/lenovo-control"

  install -Dm755 "$source_root/assets/lenovo-control-helper" \
    "$pkgdir/usr/lib/lenovo-control/lenovo-control-helper"

  install -Dm755 "$source_root/assets/lenovo-control-gui" \
    "$pkgdir/usr/bin/lenovo-control-gui"

  install -Dm644 "$source_root/assets/lenovo-control.service" \
    "$pkgdir/usr/lib/systemd/user/lenovo-control.service"

  install -Dm644 "$source_root/LICENSE" \
    "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
