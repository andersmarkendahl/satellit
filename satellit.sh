#! /bin/sh
##
## satellit.sh --
##
##	Script for the satellit game.
##
##	Preparation;
##
##	Install the following packages:
##	Ubuntu
##		sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
##		doxygen libconfig-dev libsdl2-mixer-dev libcunit1-dev graphviz
##	Fedora (only development tested)
##		sudo yum install SDL2-devel SDL2_image-devel SDL2_ttf-devel \
##		doxygen libconfig-devel SDL2_mixer-devel CUnit-devel
##	macOS (via Homebrew)
##		brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer libconfig cunit doxygen graphviz
##
##	Build Satellit;
##
##		satellit build
##
##	Start and stop Satellit;
##
##		satellit start
##		satellit stop
##
## Commands;
##

prg=$(basename $0)
dir=$(cd "$(dirname $0)" && pwd -P)
me=$dir/$prg
tmp=/tmp/${prg}_$$
sshopt='-q -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no'

die() {
	echo "ERROR: $*" >&2
	rm -rf $tmp
	exit 1
}
help() {
	grep '^##' $0 | cut -c3-
	rm -rf $tmp
	exit 0
}
test -n "$1" || help
echo "$1" | grep -qi "^help\|-h" && help

test -n "$SAT_WS" || die 'Not set [$SAT_WS]'
test -n "$SAT_GIT" || die 'Not set [$SAT_GIT]'

##	build [--clean] [--debug]
##		Compiles satellit
##		debug adds logging
##
cmd_build() {

	path=`pwd`
	cd $SAT_GIT/src
	if test "$__clean" = "yes"; then
		make clean
	fi
	if test "$__debug" = "yes"; then
		echo "Compiling (debug)"
		make DEBUG=1
		ret=$?
	else
		echo "Compiling (no debug)"
		make
		ret=$?
	fi
	cd $path
	return $ret
}

##	start [map-number]
##		Starts satellit
##		Setting map number only if debug compiled
##
cmd_start() {

	path=`pwd`
	cd $SAT_GIT
	test -x "satellit" || die 'No executable satellit'
	./satellit $1
	ret=$?
	cd $path
	return $ret

}

##	utest_build [--clean]
##		Compiles unit test binary
##		Debug adds logging
##
cmd_utest_build() {

	path=`pwd`
	cd $SAT_GIT/test/unittest
	if test "$__clean" = "yes"; then
		make clean
	fi
	echo "Compiling unittest"
	make DEBUG=1
	ret=$?
	cd $path
	return $ret
}

##	utest_start
##		Starts unittest
##
cmd_utest_start() {

	path=`pwd`
	cd $SAT_GIT
	test -x "utest" || die 'No executable utest'
	./utest
	ret=$?
	cd $path
	return $ret

}

##	generate_doc
##		Generate source documentation
##
cmd_generate_doc() {

	doxygen $SAT_GIT/doc/doc.conf

}

# Get the command
cmd=$1
shift
grep -q "^cmd_$cmd()" $0 $hook || die "Invalid command [$cmd]"

while echo "$1" | grep -q '^--'; do
	if echo $1 | grep -q =; then
		o=$(echo "$1" | cut -d= -f1 | sed -e 's,-,_,g')
		v=$(echo "$1" | cut -d= -f2-)
		eval "$o=\"$v\""
	else
		o=$(echo "$1" | sed -e 's,-,_,g')
		eval "$o=yes"
	fi
	shift
done
unset o v
long_opts=`set | grep '^__' | cut -d= -f1`

# Execute command
trap "die Interrupted" INT TERM
cmd_$cmd "$@"
status=$?
rm -rf $tmp
exit $status
