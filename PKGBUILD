# Maintainer: Community contributors
pkgname=lenovo-control
pkgver=0.3.0
pkgrel=1
pkgdesc='Unofficial Lenovo IdeaPad power and battery controller with Qt 6 GUI'
arch=('any')
url='https://github.com/YOUR_USERNAME/lenovo-control'
license=('MIT')
depends=('python' 'polkit' 'libnotify' 'pyside6')
source=("${pkgname}-${pkgver}.tar.gz")
sha256sums=('5227e6b3a889556c717e9c0bcaa99997902ccdb4f3804570175d77eb8b1501c0')

package() {
  local root="$srcdir/${pkgname}-${pkgver}"
  install -Dm755 "$root/assets/lenovo-control" "$pkgdir/usr/bin/lenovo-control"
  install -Dm755 "$root/assets/lenovo-control-helper" "$pkgdir/usr/lib/lenovo-control/lenovo-control-helper"
  install -Dm755 "$root/assets/lenovo-control-gui" "$pkgdir/usr/bin/lenovo-control-gui"
  install -Dm644 "$root/assets/lenovo-control.service" "$pkgdir/usr/lib/systemd/user/lenovo-control.service"
  install -Dm644 "$root/assets/lenovo-control.desktop" "$pkgdir/usr/share/applications/lenovo-control.desktop"
  install -Dm644 "$root/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
