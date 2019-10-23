#!/bin/bash

#
# Copyright (c)      2019 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

quo_tests_get_app() {
    t=$*

    echo "$t" | cut -f 1 -d ':'
}
export -f quo_tests_get_app

quo_tests_get_numpe() {
    t=$*

    echo "$t" | cut -f 2 -d ':'
}
export -f quo_tests_get_numpe

quo_tests_run() {
    tests=("$@")

    for test in "${tests[@]}"; do
        app=$(quo_tests_get_app "$test")
        for numpe in $(quo_tests_get_numpe "$test"); do
            # TODO(skg) FIXME Make nicer.
            echo "$QUO_TESTS_PRUN $QUO_TESTS_PRUN_N $numpe $QUO_TESTS_PRUN_OTHER_ARGS $app"
                  $QUO_TESTS_PRUN $QUO_TESTS_PRUN_N $numpe $QUO_TESTS_PRUN_OTHER_ARGS $app
        done
    done
}
export -f quo_tests_run


emitl() {
cat << EOF
============================================================================
EOF
}

emit_envars() {
    eval "declare -A envars=""${1#*=}"

    echo "# The following variables affect how tests are run."
    echo "# Exported key=value pairs will change test harness behavior."
    echo "# Here are the defaults."

    for key in "${!envars[@]}"; do
       echo "$key=${envars[$key]}"
    done
    echo
}

skip_export() {
    key=$1

    case $key in
        QUO_TESTS_EXPORT)
            return 0
            ;;
    esac
    return 1
}

exported_val() {
    key=$1

    echo "$(bash -c "echo $(eval echo \$"$key")")"
}

export_envars() {
    eval "declare -A envars=""${1#*=}"

    for key in "${!envars[@]}"; do
        if skip_export "$key"; then
            continue
        fi
        # Default value
        val="${envars[$key]}"
        # See if the variable if exported (that means the user wants a different
        # value).
        exval=$(exported_val "$key")
        if [[ "x" != "x${exval}" ]]; then
            echo "# User-defined key-value pair detected: $key=$exval"
            val=${exval}
        fi
        export "$key"="$val"
    done

    echo "# Below is the setup used for testing."
    env | grep -E '^QUO_TESTS_.*=' | sort

    exval=$(exported_val "QUO_TESTS_EXPORT")
    if [[ "x" != "x${exval}" ]]; then
        echo "# And here are the variables that we exported for you."
        for key in $(echo "$exval" | tr ':' '\n'); do
            echo "$key=$(eval echo \$"$key")"
            export "$key=$(eval echo \$"$key")"
        done
    fi
}

main() {
    echo
    emitl
    echo "Setting up environment for testing..."
    emitl

    declare -A envars
    # Things like mpiexec, srun.
    envars['QUO_TESTS_PRUN']='mpiexec'
    # The argument specifying how many processes to launch.
    envars['QUO_TESTS_PRUN_N']='-n'
    # Any other arguments passed to PRUN.
    envars['QUO_TESTS_PRUN_OTHER_ARGS']=''
    # Colon-delimited list of environment variables to export.
    envars['QUO_TESTS_EXPORT']=''

    emit_envars "$(declare -p envars)"
    export_envars "$(declare -p envars)"

    emitl
    echo
}

main
