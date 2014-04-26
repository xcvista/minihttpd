#!/bin/bash

cd "$(dirname $0)"

cat << EOF

/**
 * @file    version.h
 * @author  Maxthon Chan
 *
 * Version information of minihttpd.
 */

#ifndef _VERSION_H_
#define _VERSION_H_

#define MINIHTTPD_MAJOR 1
#define MINIHTTPD_MINOR 0
EOF

if [ -e .git ]; then
    echo "#define MINIHTTPD_BUILD \"git $(git rev-parse --short HEAD)\""
else
    echo "#define MINIHTTPS_BUILD \"vendor build\""
fi

cat << EOF

#endif

EOF

