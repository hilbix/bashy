#

love:	all

all:
	cd .src && for a in *; do ( cd "$$a" && git remote update -p && git ff "origin/$$a" ); done

