#
# BASH boilerplate.  Adopt and change as needed.
#
# Suggested use:
# . "$(dirname "$(readlink -e -- "$0")")/boilerplate.inc" '' || exit
#
# Give variable-name-prefix, output prefix and output-suffix as args, like
# . "$(dirname "$(readlink -e -- "$0")")/boilerplate.inc" 'myflags' '#OUTPUT# ' '' || exit
# myflagsdebug=1
#
# This Works is placed under the terms of the Copyright Less License,
# see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.

###
### Protect against multiple includes
###

declare -gr _BOILER_SOURCE_ONLY_ONCE_="$(readlink -e -- "$BASH_SOURCE" 2>&1)" || return

###
### Allow friendly names
###

[ -z "$1" ] || declare -grn _BOILER_DEBUG_="${1}debug" _BOILER_NO_NOTE_="${1}noquiet" _BOILER_VERBOSE_="${1}verbose"

###
### Constants
###

declare -gr _BOILER_PREFIX_="${2-#}"
declare -gr _BOILER_SUFFIX_="${3-$_BOILER_PREFIX_}"

###
### Variables
###

: "${_BOILER_DEBUG_:=0}${_BOILER_NO_NOTE_:=0}${_BOILER_VERBOSE_:=0}"

###
### ONELINERS
###

# Following oneliners are very helpful and expressive
# (and do not change the error code, BTW)
UNLESS()  { local e=$?; [ ".$1" = ".$2" ] || "${@:3}"; return $e; }	# "UNLESS "$cmp" "$val" CMD args..", witout affecting $?
STDOUT()  { local e=$?; printf '%s%q' "${_BOILER_PREFIX_}" "$1"; [ 1 = $# ] || printf ' %q' "${@:2}"; printf '\n'; return $e; }
STDERR()  { STDOUT "$@" >&2; }
DEBUG()   { UNLESS 0 "${_BOILER_DEBUG_}" STDERR DEBUG\# "$@"; }
NOTE()    { UNLESS 0 "${_BOILER_NO_NOTE_}" NOTE\# "$@"; }
VERBOSE() { UNLESS 0 "${_BOILER_VERBOSE_}" STDOUT NOTE\# "$@"; }
WARN()    { STDERR "WARN${_BOILER_SUFFIX_}" "$@"; }
ERR()     { STDERR "ERR${_BOILER_SUFFIX_}$?" "$@"; }

# Fatals
CALLER()  { local e="$(caller $((1+$1)))"; line="${e%% *}"; e="${e#* }"; file="${e#* }"; fn="${e%% *}"; }
OOPS()    { STDERR "OOPS${_BOILER_SUFFIX_}" "$@"; exit 23; }
INTERNAL() { CALLER "${1:-0}"; OOPS "$file:" line "$line:" INTERNAL ERROR in function "$fn" "${@:2}"; }
INTERNAL1() { CALLER 0; INTERNAL 2 calling function "$fn"; }
NOTYET() { CALLER "${1:-0}"; OOPS "$file:" line "$line:" not-yet-implemeted function "$fn" "${@:2}"; }

# Things to support error checking
x() { DEBUG exec: "$@"; "$@"; DEBUG ret$?: "$@"; }	# "x command args.." allowed to fail
o() { x "$@" || OOPS fail $?: "$@"; }			# "o command args.." must not fail
O() { o "$@"; VERBOSE "$@"; }				# "O command args.." verbose o
N() { o "$@"; NOTE "$@"; }				# "N command args.." noted o
# v VAR cmd args..: set a variable to the output of the command (with error checking if "cmd" is not "x")
# V VAR VAL LOCAL..: set VAR to VAL, taking back "local LOCAL..", not affecting $?
U() { unset -v -- "$@"; }	# see unset2 from https://wiki.bash-hackers.org/commands/builtin/unset
V() { set -- $? "$@"; U "${@:4}"; printf -v "$2" "%s" "$3"; return $1; }
v() { local v; v="$(U v; "${@:2}"; STDOUT dummy)" || UNLESS x "$2" OOPS fail $?: "${@:2}"; set -- $? "$1" "${v%"${_BOILER_PREFIX_}dummy"}"; V "$2" "${3%$'\n'}" v; return $1; }

# not good	o cmd args.. < file		o cmd args.. > file		o cmd args.. >> file
# catch IOerr	o Reader file cmd args..	o Writer file cmd args..	o Appender file cmd args..
# combined	o Reader input Writer output Writerr errorfile READER 4 anotherfile cmd args..
Reader()	{          "${@:2}"   0<     "$1"; }	# stdin
Writer()	{          "${@:2}"   1>     "$1"; }	# stdout write
Appender()	{          "${@:2}"   1>>    "$1"; }	# stdout append
Writerr()	{          "${@:2}"   2>     "$1"; }	# stderr write
Appenderr()	{          "${@:2}"   2>>    "$1"; }	# stderr append
READER()	{ eval -- '"${@:3}"' "$1<"  '"$2"'; }	# any read
WRITER()	{ eval -- '"${@:3}"' "$1>"  '"$2"'; }	# any write
APPENDER()	{ eval -- '"${@:3}"' "$1>>" '"$2"'; }	# any append


# https://stackoverflow.com/a/47556292/490291
_PASSBACK() { while [ 1 -lt $# ]; do printf '%q=%q;' "$1" "${!1}"; shift; done; return $1; }
PASSBACK() { _PASSBACK "$@" "$?"; }
_CAPTURE() { { out="$("${@:2}" 3<&-; "$2_" >&3)"; ret=$?; printf "%q=%q;" "$1" "$out"; } 3>&1; echo "(exit $ret)"; }
CAPTURE() { eval "$(_CAPTURE "$@")"; }
#
# Example use for a function which passes back two variables x and y
#
#fails_() { PASSBACK x y; }	# Annotate d() with what is passed back
#fails() { x=$1; y=69; echo FAIL; return 23; }	# A naturally written function
#
# x=0
# y=0
# CAPTURE out fails 42
# echo $? $x $y $out
#
# # outputs: 23 42 69 FAIL


# Simple tests
_isALPHA()    { case "$1" in	('')	return 1;;	(*[^A-Z]*)	return 1;;	esac; return 0; }
_isAlpha()    { case "$1" in	('')	return 1;;	(*[^a-zA-Z]*)	return 1;;	esac; return 0; }
_isalpha()    { case "$1" in	('')	return 1;;	(*[^a-z]*)	return 1;;	esac; return 0; }

_isALPHA_()   { case "$1" in	('')	return 1;;	(*[^A-Z_]*)	return 1;;	esac; return 0; }
_isAlpha_()   { case "$1" in	('')	return 1;;	(*[^a-zA-Z_]*)	return 1;;	esac; return 0; }
_isalpha_()   { case "$1" in	('')	return 1;;	(*[^a-z_]*)	return 1;;	esac; return 0; }

_isALNUM()    { case "$1" in	('')	return 1;;	(*[^A-Z0-9]*)	return 1;;	esac; return 0; }
_isAlnum()    { case "$1" in	('')	return 1;;	(*[^a-zA-Z0-9]*) return 1;;	esac; return 0; }
_isalnum()    { case "$1" in	('')	return 1;;	(*[^a-z0-9]*)	return 1;;	esac; return 0; }

_isALNUM_()   { case "$1" in	('')	return 1;;	(*[^A-Z0-9_]*)	return 1;;	esac; return 0; }
_isAlnum_()   { case "$1" in	('')	return 1;;	(*[^a-zA-Z0-9_]*) return 1;;	esac; return 0; }
_isalnum_()   { case "$1" in	('')	return 1;;	(*[^a-z0-9_]*)	return 1;;	esac; return 0; }

_validname()  { case "$1" in	(*[^a-z0-9]*)	;;	([a-z]*)	return 0;;	esac; return 1; }
_VALIDNAME()  { case "$1" in	(*[^A-Z0-9]*)	;;	([A-Z]*)	return 0;;	esac; return 1; }
_Validname()  { case "$1" in	(*[^A-Za-z0-9]*);;	([A-Z]*)	return 0;;	esac; return 1; }
_validName()  { case "$1" in	(*[^A-Za-z0-9]*);;	([a-z]*)	return 0;;	esac; return 1; }
_ValidName()  { case "$1" in	(*[^A-Za-z0-9]*);;	([a-zA-Z]*)	return 0;;	esac; return 1; }

_validname_() { case "$1" in	(*[^a-z0-9_]*)	;;	([a-z_]*)	return 0;;	esac; return 1; }
_VALIDNAME_() { case "$1" in	(*[^A-Z0-9_]*)	;;	([A-Z_]*)	return 0;;	esac; return 1; }
_Validname_() { case "$1" in	(*[^A-Za-z0-9_]*);;	([A-Z_]*)	return 0;;	esac; return 1; }
_validName_() { case "$1" in	(*[^A-Za-z0-9_]*);;	([a-z_]*)	return 0;;	esac; return 1; }
_ValidName_() { case "$1" in	(*[^A-Za-z0-9_]*);;	([a-zA-Z_]*)	return 0;;	esac; return 1; }

# This does not check the length restriction of 63 characters
_validasciidompart() { case "$1" in (*[^a-z0-9-]*|''|-*|*-|*--*--*|*---*);; (xn--*) return 0;; (*--*);; (*) return 0;; esac; return 1; }

#EOF#
