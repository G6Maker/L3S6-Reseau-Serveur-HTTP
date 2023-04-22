tar:
	$(MAKE) -C server clean
	$(MAKE) -C socketTCP clean
	$(MAKE) -C adresse_internet clean
	tar -zcf "$(CURDIR)_$(shell date +'%y.%m.%d-%Hh%M').tar.gz" *
