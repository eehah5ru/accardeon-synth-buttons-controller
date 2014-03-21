all: clean-garbage build erase upload

clean-garbage:
	find . -type f -print0 | grep -FzZ ".AppleDouble/" | xargs --null -r rm
build:
	ino build -m gert328

erase:
	sudo avrdude -c gpio -p m328p -e -v

upload:
	sudo ino upload -m  gert328 -p /dev/ttyAMA0	