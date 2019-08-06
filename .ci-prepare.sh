#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -eux

# Show information about CI runner environment
nproc && grep Mem /proc/meminfo && df -hT .

# Install Linux build dependencies
apk add build-base bison findutils flex gmp-dev mpc1-dev mpfr-dev openssl-dev perl

if [ -n "${DRONE-}" ]; then
	# Workaround problem with faccessat2() on Drone CI
	wget https://gist.githubusercontent.com/TravMurav/36c83efbc188115aa9b0fc7f4afba63e/raw/faccessat.c -P /opt
	gcc -O2 -shared -o /opt/faccessat.so /opt/faccessat.c
	export LD_PRELOAD=/opt/faccessat.so
fi

# Setup compiler
case "$1" in
gcc)
	case "$ARCH" in
		arm64)	gcc_toolchain=aarch64-none-elf ;;
		arm)	gcc_toolchain=arm-none-eabi ;;
	esac
	apk add "gcc-$gcc_toolchain"
	MAKE_OPTS="CROSS_COMPILE=$gcc_toolchain-"
	;;
clang)
	apk add clang lld llvm
	MAKE_OPTS="LLVM=1"
	;;
esac
MAKE_OPTS="-j$(nproc) $MAKE_OPTS"

# Write build script
cat > .ci-build.sh <<EOF
#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -eux

cat arch/arm64/configs/msm8916_defconfig arch/arm/configs/msm8916_defconfig.part > arch/arm/configs/msm8916_defconfig
make $MAKE_OPTS msm8916_defconfig
echo CONFIG_WERROR=y >> .config
make $MAKE_OPTS
EOF
chmod +x .ci-build.sh
