compile() {
	FOLDER=$1
	LIBNAME=$2
	cd $FOLDER
	g++ -Wall -fPIC -c *.cpp -lglfw -lpng -lGL -lGLEW -lalut -lopenal -laudio
	g++ -shared -o $LIBNAME *.o -lglfw -lpng -lGL -lGLEW -lalut -lopenal -laudio
	rm -rf *.o
	mv $LIBNAME ../lib
	cd ../
}

rm -rf lib
mkdir lib
echo "Compiling the engine"
echo "Start compiling the Window.lib library"
compile Window Window.lib
echo "Start compiling the Event.lib library"
compile Event Event.lib
echo "Start compiling the Music.lib library"
compile Music Music.lib
echo "Start compiling the Graphics.lib library"
compile Graphics Graphics.lib
echo "Done"
