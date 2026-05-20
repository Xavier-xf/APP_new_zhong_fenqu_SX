all:
	cd build ; cmake .;make
	cp build/CDV1004QT.BIN ./
	tar -cvf CDV1004QTOS CDV1004QT.BIN
	rm -rf CDV1004QT.BIN
ui:
	# cd build ; cmake .;make
	# cp build/CDV1004QT.BIN ./
	# cp res/rom.bin  ./ 
	# cp res/sat_leo.ttf ./
	# cp res/rings ./ -r
	# cp res/wallpaper ./ -r
	# cd upgrade/;./make_image.sh
	# tar -cvf CDV1004QTOS CDV1004QT.BIN rom.bin sat_leo.ttf rings wallpaper
	# rm -rf CDV1004QT.BIN rom.bin rings sat_leo.ttf wallpaper

	cd build ; cmake .;make -j16
	mkdir -p upgrade/app
	cp -f build/CDV1004QT.BIN 	upgrade/app
	cp -f res/rom.bin 			upgrade/app
	cp -f res/sat_leo.ttf 		upgrade/app
	cp -rf res/rings 			upgrade/app
	cp -rf res/wallpaper 		upgrade/app
	cd upgrade/;./make_image.sh
	rm -rf upgrade/app
kernel:
	cd build ; cmake .;make
	cp build/CDV1004QT.BIN ./
	cp res/rom.bin  ./ 
	cp res/sat_leo.ttf ./
	cp res/rings ./ -r
	cp res/wallpaper ./ -r
	cp upgrade/platform ./ -r
	tar -cvf CDV1004QTOS CDV1004QT.BIN rom.bin sat_leo.ttf rings wallpaper platform
	rm -rf CDV1004QT.BIN rom.bin rings sat_leo.ttf wallpaper platform
clean:              
	mv ./build/CMakeLists.txt ./
	rm build/* -rf
	mv CMakeLists.txt ./build/
rootfs:
	cp upgrade/platform ./ -r
	tar -cvf CDV1004QTOS platform
	rm -rf platform

	
