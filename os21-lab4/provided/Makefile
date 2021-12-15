all:
ifneq (,$(wildcard 01-sanitization/Makefile))
	$(MAKE) -C 01-sanitization/ all
endif
ifneq (,$(wildcard 02-fuzzing/Makefile))
	$(MAKE) -C 02-fuzzing/ all
endif

clean:
ifneq (,$(wildcard 01-sanitization/Makefile))
	$(MAKE) -C 01-sanitization/ clean
endif
ifneq (,$(wildcard 02-fuzzing/Makefile))
	$(MAKE) -C 02-fuzzing/ clean
endif

feedback:
	docker pull atrib/cs323_lab4:w12
	-docker run --rm --name=cs323 --cap-add=SYS_PTRACE -v ${shell pwd}:/done atrib/cs323_lab4:w12

