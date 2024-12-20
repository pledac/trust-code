#!/bin/bash
# Source environment for packagespy and trustpy

root_dir=$TRUST_ROOT/Outils/trustpy/install

export PACKAGESPY_ROOT_DIR=${root_dir}/packagespy
export PYTHONPATH=${PYTHONPATH}:${PACKAGESPY_ROOT_DIR}/packagespy

export TRUSTPY_ROOT_DIR=${root_dir}/trustpy
export PYTHONPATH=${PYTHONPATH}:${TRUSTPY_ROOT_DIR}

# https://groups.google.com/g/qtcontribs/c/cWQgcrbKUCM useful to avoid QXcbConnection: XCB error: 3
export QT_LOGGING_RULES="*.debug=false"  

