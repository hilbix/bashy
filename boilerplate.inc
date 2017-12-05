#
# BASH boilerplate.  Adopt and change as needed
#
# This Works is placed under the terms of the Copyright Less License,
# see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.

UNLESS()  { local e=$?; [ ".$1" = ".$2" ] || "${@:3}"; return $e; }	# "UNLESS "$cmp" "$val" CMD args..", witout affecting $?
STDOUT()  { local e=$?; printf '#%q' "$1"; [ 1 = $# ] || printf ' %q' "${@:2}"; printf '\n'; return $e; }
STDERR()  { STDOUT "$@" >&2; }
DEBUG()   { UNLESS 0 $hub_debug   STDERR DEBUG\# "$@"; }
NOTE()    { UNLESS 0 $hub_noquiet STDOUT NOTE\# "$@"; }
VERBOSE() { UNLESS 0 $hub_verbose STDOUT NOTE\# "$@"; }
WARN()    { STDERR WARN\# "$@"; }
ERR()     { STDERR ERR\#$? "$@"; }

# Fatals
CALLER()  { local e="$(caller $((1+$1)))"; line="${e%% *}"; e="${e#* }"; file="${e#* }"; fn="${e%% *}"; }
OOPS()    { STDERR OOPS\# "$@"; exit 23; }
INTERNAL() { CALLER "${1:-0}"; OOPS "$file" line "$line" INTERNAL ERROR in function "$fn" "${@:2}"; }
INTERNAL1() { CALLER 0; INTERNAL 2 calling function "$fn"; }
NOTYET() { CALLER "${1:-0}"; OOPS "$file" line "$line" not-yet-implemeted function "$fn" "${@:2}"; }

# Things to support error checking
x() { DEBUG exec: "$@"; "$@"; DEBUG ret$?: "$@"; }	# "x command args.." allowed to fail
o() { x "$@" || OOPS fail $?: "$@"; }			# "o command args.." must not fail
O() { o "$@"; VERBOSE "$@"; }				# "O command args.." verbose o
N() { o "$@"; NOTE "$@"; }				# "N command args.." noted o
# v VAR cmd args..: set a variable to the output of the command (with error checking if "cmd" is not "x")
# V VAR VAL LOCAL..: set VAR to VAL, taking back "local LOCAL..", not affecting $?
U() { unset -v "$@"; }
V() { set -- $? "$@"; U "${@:4}"; printf -v "$2" "%s" "$3"; return $1; }	# see http://wiki.bash-hackers.org/commands/builtin/unset#unset2
v() { local v; v="$(U v; "${@:2}")" || UNLESS x "$2" OOPS fail $?: "${@:2}"; V "$1" "$v" v; }

# Simple tests
isAlpha() { case "$1" in '') return 1;; (*[^a-zA-Z]*) return 1;; esac; return 0; }
isalpha() { case "$1" in '') return 1;; (*[^a-z]*) return 1;; esac; return 0; }
isAlnum() { case "$1" in '') return 1;; (*[^a-zA-Z0-9]*) return 1;; esac; return 0; }
isalnum() { case "$1" in '') return 1;; (*[^a-z0-9]*) return 1;; esac; return 0; }
validname() { case "$1" in (*[^a-z0-9]*);; ([a-z]*) return 0;; esac; return 1; }
