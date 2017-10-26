# Maintainer: Fredrik <fredrikhansson_12345[at]yahoo[dot]com>
pkgname=cponysay
pkgver=1.0
pkgrel=1
pkgdesc="a basic C implementation of Ponysay"
url="https://github.com/FredrikHson/cponysay"
arch=('x86_64' 'i686')
license=('GPL3')
source=(
"git+https://github.com/FredrikHson/cponysay.git"
"git+https://github.com/erkin/ponysay.git"
)
sha256sums=('SKIP' 'SKIP')

prepare()
{
    cd "${srcdir}/${pkgname}"
    cp -rf "${srcdir}/ponysay" ./external
    mkdir -p build
    cd build
    cmake ../src -DCMAKE_INSTALL_PREFIX=/usr
}

build()
{
    cd "${srcdir}/${pkgname}"
    make
}

package()
{
    cd "${srcdir}/${pkgname}/build"
    make DESTDIR="${pkgdir}" install
}
