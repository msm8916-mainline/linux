#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -eux

case "$1" in
build)
	nproc && grep Mem /proc/meminfo && df -hT .
	apk add build-base bison findutils flex gmp-dev mpc1-dev mpfr-dev openssl-dev perl
	make msm8916_defconfig
	make KCFLAGS="-Wall -Werror" -j$(nproc)
	;;
check)
	apk add git perl
	git format-patch origin/$DRONE_TARGET_BRANCH
	scripts/checkpatch.pl --strict --color=always *.patch || :
	! scripts/checkpatch.pl --strict --color=always --terse --show-types *.patch \
		| grep -Ff .drone-checkpatch.txt
	;;
*)
	exit 1
	;;
esac
