#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -eux

git format-patch "$1"
scripts/checkpatch.pl --strict --color=always *.patch || :
! scripts/checkpatch.pl --strict --color=always --terse --show-types *.patch \
	| grep -Ff .ci-checkpatch.txt
