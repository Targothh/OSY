#! /bin/bash
case $1 in 
-h)
echo "Usage:
    script.sh [FLAG]
Flags:
    -h          help for script.sh
    -z          script compresses every FILE found into an output.tgz archive"
    exit 0
;;

-a)
FILES=$(ls -a)
echo "$FILES" | grep -Ei "\.pdf$"
exit 0
;;

-b)
grep -E '^[+-]?[0-9]+' | sed -E 's/^[+-]?[0-9]+//' <&0
exit 0
;;

-c)
tr '\n' ' ' | grep -o '[[:upper:]][^.?!]*[.?!]' <&0
exit 0
;;

-d)
while IFS= read -r INPUT; do
echo "$INPUT" | sed -E "s|( *# *include[[:space:]]*)\"([a-zA-Z]+.h)\"|\1\"$2\2\"|; s|( *# *include[[:space:]]*)<([a-zA-Z]+.h)>|\1<$2\2>|"
        done
exit 0
;;
*)
exit 1
esac
