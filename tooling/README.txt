busybox.exe: BusyBox v1.37.0-FRP-5301-gda71f7c57, UPXed

	-> https://frippery.org/busybox/index.html#downloads


build/make.exe: GNU make 4.4.90 built from source + UPXed

	-> https://lwn.net/Articles/913253/

	64-bit .exe built from https://git.savannah.gnu.org/git/make.git with w64devkit
	(`bootstrap && build_w32.bat --without-guile gcc`, then `strip gnumake.exe`).

	IT'S EVEN *CONSIDERABLY* SMALLER THAN 4.3! :-o :)

	NOTE: failed to compile with the 32-bit version of w64devkit, with mismatches in
	dir.h... (Instant googling -> https://stackoverflow.com/questions/35215109/struct-dirent-does-not-have-de-type-in-header-file
	-> https://stackoverflow.com/a/35215258/1479945: its addendum section gave a
	hint about 64-bit, so I just tried that without even finishing the sentence,
	and it worked!)
