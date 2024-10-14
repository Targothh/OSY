#! /bin/bash

if [[ "$1" == "-h" || "$2" == "-h" ]]; then
echo "Usage:
    script.sh [FLAG]
Flags:
    -h          help for script.sh
    -z          script compresses every FILE found into an output.tgz archive"
    exit 0
elif [[ "$1" == "-z" &&  $# -gt 1  ]]; then
    exit 2
elif [[ "$1" != "-z" && "$1" != "-h" && "$1" != "" ]]; then
    exit 2
fi
declare -a FARRAY
RETURNVAL=0
while read -r INPUT; do
if [[ $INPUT =~ ^"PATH " ]]; then
    unset FPATH
    FPATH=${INPUT#PATH }
    if [[ -L $FPATH ]]; then
        LINKPATH=$(readlink "${FPATH}")
        LINKBASE=$(basename "${LINKPATH}")
        echo "LINK '${FPATH}' '${LINKBASE}'"
    elif [[ -d $FPATH ]]; then 
        echo "DIR '${FPATH}'"
    elif [[ -f $FPATH ]]; then
        if [[ ! -r "${FPATH}" || ! -w "${FPATH}" ]]; then
        exit 2;
        fi
        echo "FILE '${FPATH}' $(wc -l < "${FPATH}") '$(head -1 "${FPATH}")'"
        FARRAY+=("${FPATH}")
    else 
        RETURNVAL=1
        echo "ERROR '${FPATH}'" >&2
    fi
fi
done
if [[ "$1" == "-z" ]]; then
    if ! tar czf output.tgz "${FARRAY[@]}"; then
    exit 2
    fi
fi
exit $RETURNVAL