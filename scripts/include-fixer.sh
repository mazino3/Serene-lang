#! /bin/bash

set -e

# function get_changed_files() {
#     local add_modified
#     local renamed
#     add_modified=$(git diff --cached --name-status |egrep '*\.(cpp|h|hpp|cpp.inc|h.inc)'|awk '($1 != "D" && $1 !~ /R.+/) { print $2 }')
#     renamed=$(git diff --cached --name-status |egrep '*\.(cpp|h|hpp|cpp.inc|h.inc)'|awk '$1 ~ /^R.+/ { print $3 }')

#     echo $add_modified | sed '/^[[:space:]]*$/d'
#     echo $renamed | sed '/^[[:space:]]*$/d'
# }

function fix_includes(){
    #local files=$(get_changed_files)

    echo "Fixing '#include' syntax in '$1'"
    sed -i -E '/^#include "(serene|\.)/!s/^.include \"(.*)\"/#include <\1>/g' "$1"
}

# function clang_format_staged() {
#     local files=$(get_changed_files)

#     if [[ "$files" ]];
#     then
#         for file in $(get_changed_files)
#         do
#             echo "Reformatting $file..."
#             clang-format -i $file
#         done
#     fi

# }

# function git_add_changes() {
#     local files=$(get_changed_files)

#     if [[ "$files" ]];
#     then
#         git add $files
#     fi
# }

fix_includes "$1"
# clang_format_staged
# git_add_changes
