NAME=$1
if [ "$NAME" = "" ]; then
    NAME="FlappyBird"
fi

OUTDIR="./Release"
if [ -d "$OUTDIR" ]; then
    rm -rf $OUTDIR
fi
mkdir Release
cd Engine2D
./compileEngine.sh
cd ../
echo "Compiling the game"
g++ game.cpp Engine2D/*.cpp -o $NAME -lglfw -lpng -lGL -lGLEW -lalut -lopenal -laudio -ldl
rm -rf lib
mv Engine2D/lib ./Release
mv $NAME ./Release
cp -r images ./Release
cp -r audio ./Release
echo Done
