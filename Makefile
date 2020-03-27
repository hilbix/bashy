#

love:	push all

all:
	git remote update -p && git ff origin/master
	ok=:; cd .src && for a in *; do echo "$$a"; ( cd "$$a" && git remote update -p && git ff "origin/$$a" ) || ok=false; done; $$ok

push:
	git push origin HEAD:master
	ok=:; cd .src && for a in *; do echo "$$a"; ( cd "$$a" && git push origin "HEAD:$$a" ) || ok=false; done; $$ok

