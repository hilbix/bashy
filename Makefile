.PHONY: love all clean distclean test

love:	all
all:	test
clean:
distclean:
test:
	for a in *; do LC_ALL=C.UTF-8 bash -n "$$a" 2>&1 | awk -v N="$$a" -F: '$$1==N && $$2 ~ /^ line / { sub(/^.*: line /,"",$$0); n=$$1; gsub(/:.*$$/,"",n); sub(/^[^:]*:[[:space:]]*/,""); printf "#E#%s#%d#1#%s#\n", N, n, $$0; next; } { print }'; done
	for a in test.*; do bash ./"$$a" || exit 1; done

